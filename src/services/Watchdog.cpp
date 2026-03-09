//
// Created by lmartinignacio@gmail.com on 3/3/2026.
//

#include "services/Watchdog.h"
#include "config/BuildConfig.hpp"
#include "os/Tasks.hpp"
#include "os/Time.hpp"
#include "../../include/os/Settings.h"
#include <Arduino.h>
#include <WiFi.h>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <esp_system.h>

#include "ESP32Ping.h"
#include "config/Pins.hpp"

using namespace CE::OS;
using namespace CE::Config;

namespace CE::Services
{
    const char* Watchdog::TAG = "Watchdog-Service";
    const char* Watchdog::REPORTS_FILENAME = "/WatchdogReports.json";
    QueueHandle_t Watchdog::errorQueue_ = nullptr;
    unsigned short Watchdog::errorCount_ = 0;
    uint8_t Watchdog::restartCount_ = 0;
    uint8_t Watchdog::restartVotes_ = 0;
    std::map<const char*, Watchdog::MonitoredTask>  Watchdog::tasks_;

    Watchdog::Watchdog() = default;

    bool Watchdog::Setup()
    {
        ESP_LOGV(TAG, "Setup");

        // Create an error queue (ring buffer for last N errors)
        const auto& s = Settings::Get();
        errorQueue_ = OS::Queues::CreateQueue<Domain::ErrorReport>(s.maxErrorReports);
        if (!errorQueue_)
        {
            ESP_LOGE(TAG, "Failed to create error queue");
            return false;
        }

        // Load previous error reports from SPIFFS
        LoadReports();

        return OS::Tasks::Start(Task, TAG, Build::kStackWatchdogTask, nullptr, Build::kPrioWatchdog, nullptr);
    }

    bool Watchdog::RegisterTask(const char* name, const unsigned short timeout_ms)
    {
        if (tasks_.find(name) == tasks_.end())
        {
            ESP_LOGD(TAG, "Registering Task %s.", name);
            auto task = MonitoredTask();
            task.timeout_s = static_cast<float>(timeout_ms) / 1000.0f * Build::kWatchdogMaxMissedNotifies;
            task.timestamp = Time::Get();
            tasks_.emplace(name, task);
            return true;
        }

        ESP_LOGW(TAG, "Task %s already registered!", name);
        return false;
    }

    void Watchdog::NotifyTaskAlive(const char* name)
    {
        const auto task = tasks_.find(name);
        if (task == tasks_.end())
        {
            ESP_LOGE(TAG, "Task %s not registered!!!", name);
        }
        else
        {
            task->second.timestamp = Time::Get();
            ESP_LOGD(TAG, "Task %s is alive. Timestamp: %ld.", name, task->second.timestamp);
        }
    }

    void Watchdog::Task(void*)
    {
        ESP_LOGV(TAG, "Task started");
        const auto& s = Settings::Get();

        while (true)
        {
            // Sleep before the next check
            Time::SleepMs(s.watchdogCheckInterval_s * 1000);
            digitalWrite(Pins::kBlueLed, Build::kLedOff);

            // Perform health checks
            CheckMemoryHealth();
            CheckWiFiHealth();
            CheckTaskHealth();

            const auto queueCount = Queues::GetCount(errorQueue_);
            for (size_t i = 0; i < queueCount; ++i)
            {
                Domain::ErrorReport report{};
                if (Queues::Peek(errorQueue_, report, 0))
                {
                    Troubleshoot(report);
                }
            }

            // Save error reports if any new ones
            if (errorCount_ > 0)
            {
                SaveReports();
            }

            if (restartVotes_ > 0)
            {
                TriggerESPRestart("Program restart requested");
            }
        }
    }

    void Watchdog::CheckTaskHealth()
    {
        for (const auto task : tasks_)
        {
            const eTaskState state = eTaskGetState(xTaskGetHandle(task.first));

            if (state == eDeleted)
            {
                ReportError(Domain::ErrorSeverity::Critical, Domain::ErrorType::TaskDeleted, task.first, "Task has been deleted");
                continue;
            }

            if (Time::Get() - task.second.timestamp > task.second.timeout_s)
            {
                // Task has not updated heartbeat in time
                // Consider it hung or unhealthy
                ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::ServiceStuck, task.first, "Task is stuck");
            }
        }
    }

    void Watchdog::ReportError(const Domain::ErrorSeverity severity, const Domain::ErrorType type, const char* serviceName, const char* message)
    {
        if (!errorQueue_)
            return;

        digitalWrite(Pins::kBlueLed, Build::kLedOn);

        Domain::ErrorReport report = {};
        report.timestamp = time(nullptr);
        report.severity = severity;
        report.type = type;
        report.freeHeap = ESP.getFreeHeap();
        report.minFreeHeap = ESP.getMinFreeHeap();
        report.taskStackWatermark = 0; // Can be set by caller if needed
        report.restartCount = restartCount_;

        strncpy(report.serviceName, serviceName, sizeof(report.serviceName) - 1);
        strncpy(report.message, message, sizeof(report.message) - 1);

        // Try to send it to the queue (non-blocking)
        if (!OS::Queues::Send(errorQueue_, report, 0))
        {
            ESP_LOGW(TAG, "Error queue full, dropping oldest report");
            // Remove oldest, then add new
            Domain::ErrorReport dummy{};
            OS::Queues::Receive(errorQueue_, dummy, 0);
            OS::Queues::Send(errorQueue_, report, 0);
        }

        errorCount_++;

        ESP_LOGI(TAG, "Error reported: [%s] %s: %s",
                Domain::ErrorSeverityToString(severity),
                serviceName,
                message);

        // Handle critical errors immediately
        if (severity == Domain::ErrorSeverity::Critical)
        {
            Troubleshoot(report);
        }
    }

    unsigned short Watchdog::GetErrorCount()
    {
        return errorCount_;
    }

    void Watchdog::ClearErrors()
    {
        if (!errorQueue_)
            return;

        Domain::ErrorReport report{};
        while (OS::Queues::Receive(errorQueue_, report, 0))
        {
        }
        errorCount_ = 0;
        ESP_LOGI(TAG, "Error reports cleared");
    }

    bool Watchdog::Troubleshoot(const Domain::ErrorReport &report)
    {
        ESP_LOGV(TAG, "Troubleshoot");

        bool handled = false;

        switch (report.type)
        {
            case Domain::ErrorType::LowMemory:
            case Domain::ErrorType::StackOverflow:
            case Domain::ErrorType::ServiceStuck:
            case Domain::ErrorType::ServiceCrash:
            case Domain::ErrorType::TaskDeleted:
                ProgramESPRestart(report.message);
                handled = true;
                break;
            case Domain::ErrorType::WiFiDisconnect:
                AttemptWiFiReconnect();
                handled = true;
                break;
            case Domain::ErrorType::QueueEmpty:
            case Domain::ErrorType::QueueFull:
                handled = false;
                break;
            case Domain::ErrorType::SensorFailure:
            case Domain::ErrorType::ConfigError:
            case Domain::ErrorType::WaterLevel:
            case Domain::ErrorType::Unknown:
            default:
                Notify(report);
                handled = true;
                break;
        }

        return handled;
    }

    void Watchdog::Notify(const Domain::ErrorReport &report)
    {

    }

    void Watchdog::CheckMemoryHealth()
    {
        const auto freeHeap = ESP.getFreeHeap();
        const auto& s = Settings::Get();
        const uint32_t thresholdBytes = s.minFreeHeapThreshold_kb * 1024u;

        ESP_LOGD(TAG, "Free heap: %u bytes, Min free heap: %u bytes", freeHeap, ESP.getMinFreeHeap());

        if (freeHeap < thresholdBytes)
        {
            char msg[64];
            snprintf(msg, sizeof(msg), "Free heap: %u KB", freeHeap / 1024);
            ReportError(Domain::ErrorSeverity::Warning,
                       Domain::ErrorType::LowMemory,
                       TAG,
                       msg);
        }
    }

    void Watchdog::CheckWiFiHealth()
    {
        if (WiFiClass::status() != WL_CONNECTED)
        {
            ReportError(Domain::ErrorSeverity::Error,
                       Domain::ErrorType::WiFiDisconnect,
                       TAG,
                       "WiFi disconnected");
            AttemptWiFiReconnect();
        }

        const IPAddress target(192, 168, 1, 1);
        const bool ok = Ping.ping(target);
        if (ok)
        {
            ESP_LOGI(TAG, "Ping success");
        }
        else
        {
            ReportError(Domain::ErrorSeverity::Error,
                       Domain::ErrorType::WiFiDisconnect,
                       TAG,
                       "WiFi disconnected");
            AttemptWiFiReconnect();
        }
    }

    void Watchdog::AttemptWiFiReconnect()
    {
        ESP_LOGI(TAG, "Attempting WiFi reconnect...");

        const auto& s = Settings::Get();
        const auto startTime = millis();
        const auto timeout = s.wifiReconnectTimeout_s * 1000u;

        WiFi.reconnect();

        while (WiFiClass::status() != WL_CONNECTED && (millis() - startTime) < timeout)
        {
            delay(500);
            ESP_LOGV(TAG, "Reconnecting...");
        }

        if (WiFiClass::status() == WL_CONNECTED)
        {
            ESP_LOGI(TAG, "WiFi reconnected successfully");
            ESP_LOGI(TAG, "IP Address: %s", WiFi.localIP().toString().c_str());
        }
        else
        {
            ESP_LOGE(TAG, "WiFi reconnect failed");
            TriggerESPRestart("WiFi reconnect failed");
        }
    }

    void Watchdog::TriggerESPRestart(const char* reason)
    {
        ESP_LOGI(TAG, "Triggering ESP restart: %s", reason);

        // Increment restart counter
        restartCount_++;

        // Save error reports before restart
        SaveReports();

        // Wait a bit to ensure logs are flushed
        delay(1000);

        // Restart the ESP
        esp_restart();
    }

    void Watchdog::ProgramESPRestart(const char *reason)
    {
        ESP_LOGI(TAG, "Programming ESP restart: %s", reason);
        restartVotes_++;
    }

    bool Watchdog::LoadReports()
    {
        ESP_LOGV(TAG, "Loading error reports from SPIFFS");

        File file = SPIFFS.open(REPORTS_FILENAME, "r");
        if (!file)
        {
            ESP_LOGI(TAG, "No previous error reports found");
            return true;
        }

        JsonDocument doc;
        const DeserializationError error = deserializeJson(doc, file);
        file.close();

        if (error)
        {
            ESP_LOGE(TAG, "Failed to parse error reports: %s", error.c_str());
            return false;
        }

        restartCount_ = doc["restartCount"] | 0;
        errorCount_ = doc["errorCount"] | 0;

        const JsonArray reports = doc["reports"];
        for (JsonVariant v : reports)
        {
            Domain::ErrorReport report = {};
            report.timestamp = v["timestamp"] | 0;
            report.severity = static_cast<Domain::ErrorSeverity>(v["severity"] | 0);
            report.type = static_cast<Domain::ErrorType>(v["type"] | 0);
            report.freeHeap = v["freeHeap"] | 0;
            report.minFreeHeap = v["minFreeHeap"] | 0;
            report.taskStackWatermark = v["taskStackWatermark"] | 0;
            report.restartCount = v["restartCount"] | 0;

            strncpy(report.serviceName, v["serviceName"] | "Unknown", sizeof(report.serviceName) - 1);
            strncpy(report.message, v["message"] | "", sizeof(report.message) - 1);

            OS::Queues::Send(errorQueue_, report, 0);
        }

        ESP_LOGI(TAG, "Loaded %u error reports (restart count: %u)", errorCount_, restartCount_);
        return true;
    }

    bool Watchdog::SaveReports()
    {
        ESP_LOGV(TAG, "Saving error reports to SPIFFS");

        JsonDocument doc;
        doc["restartCount"] = restartCount_;
        doc["errorCount"] = errorCount_;

        const JsonArray reports = doc["reports"].to<JsonArray>();

        // Read all errors from the queue and put them into JSON
        const auto queueCount = OS::Queues::GetCount(errorQueue_);
        for (size_t i = 0; i < queueCount; ++i)
        {
            Domain::ErrorReport report{};
            if (OS::Queues::Peek(errorQueue_, report, 0))
            {
                auto r = reports.add<JsonObject>();
                r["timestamp"] = report.timestamp;
                r["severity"] = static_cast<uint8_t>(report.severity);
                r["type"] = static_cast<uint8_t>(report.type);
                r["serviceName"] = report.serviceName;
                r["message"] = report.message;
                r["freeHeap"] = report.freeHeap;
                r["minFreeHeap"] = report.minFreeHeap;
                r["taskStackWatermark"] = report.taskStackWatermark;
                r["restartCount"] = report.restartCount;
            }
        }

        File file = SPIFFS.open(REPORTS_FILENAME, "w");
        if (!file)
        {
            ESP_LOGE(TAG, "Failed to open %s for writing", REPORTS_FILENAME);
            return false;
        }

        if (serializeJson(doc, file) == 0)
        {
            ESP_LOGE(TAG, "Failed to write error reports");
            file.close();
            return false;
        }

        file.close();
        ESP_LOGD(TAG, "Saved %u error reports", queueCount);
        return true;
    }
}   // namespace CE::Services
