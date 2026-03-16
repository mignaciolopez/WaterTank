//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <config/BuildConfig.hpp>
#include <config/Pins.hpp>
#include <os/Queues.hpp>
#include <os/Settings.h>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <services/Radar.h>
#include <services/Watchdog.h>

using namespace CE::OS;

namespace CE::Services
{
    const char* Radar::TAG = "Radar-Service";
    QueueHandle_t Radar::gRadarQueue = nullptr;
    Drivers::HC_SR04* Radar::driver_ = nullptr;

    Radar::Radar() = default;

    bool Radar::Setup()
    {
        ESP_LOGV(TAG, "Setup");
        gRadarQueue = OS::Queues::CreateLatestQueue(sizeof(unsigned short));
        if (!gRadarQueue)
            return false;

        if (!driver_)
            driver_ = new Drivers::HC_SR04(Config::Pins::kTrigPin, Config::Pins::kEchoPin);

        driver_->Setup();

        const bool taskResult = Tasks::Start(Task, TAG, Config::Build::kStackRadarTask, nullptr, Config::Build::kPrioRadar, nullptr);
        const bool watchdogResult = Watchdog::RegisterTask(TAG, Settings::Get().radarDelayS * 1000);

        return taskResult && watchdogResult;
    }

    void Radar::Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        while (true)
        {
            unsigned short cm = 0;
            if (driver_->ReadDistanceCm(cm))
            {
                OS::Queues::Overwrite(gRadarQueue, cm);
                ESP_LOGD(TAG, "raw_cm=%.2f", cm / 100.0f);
            }
            else
            {
                ESP_LOGE(TAG, "timeout/no-echo");
                Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::SensorFailure, TAG, "timeout/no-echo");
            }

            Watchdog::NotifyTaskAlive(TAG);
            Time::SleepMs(Settings::Get().radarDelayS * 1000);
        }
    }

    bool Radar::TryGetLatestRawCm(unsigned short& out_cm)
    {
        ESP_LOGV(TAG, "TryGetLatestRawCm");
        if (!gRadarQueue)
            return false;

        return Queues::Receive(gRadarQueue, out_cm, 0);
    }

}   // namespace CE::Services