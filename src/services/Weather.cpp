//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <config/BuildConfig.hpp>
#include <config/Pins.hpp>

#include <domain/Global.hpp>

#include <os/Queues.hpp>
#include <os/Settings.h>
#include <os/Tasks.hpp>
#include <os/Time.hpp>

#include <services/Watchdog.h>
#include <services/Weather.h>

#include "services/WSServer.h"


using namespace CE::OS;
using namespace CE::Config;

namespace CE::Services
{
    const char* Weather::TAG = "Weather-Service";
    QueueHandle_t Weather::gWeatherQueue = nullptr;
    Drivers::DHTDevice* Weather::driver_ = nullptr;

    Weather::Weather() = default;

    bool Weather::Setup()
    {
        ESP_LOGV(TAG, "Setup");
        gWeatherQueue = Queues::CreateLatestQueue(sizeof(Domain::WeatherSample));
        if (!gWeatherQueue)
            return false;

        if (!driver_)
            driver_ = new Drivers::DHTDevice(Pins::kDhtPin, Pins::kDhtType);

        driver_->Setup();

        const bool taskResult = Tasks::Start(Task, TAG, Build::kStackWeatherTask, nullptr, Build::kPrioWeather, nullptr);
        const bool watchdogResult = Watchdog::RegisterTask(TAG, Settings::Get().WeatherDelayS * 1000);
        return taskResult && watchdogResult;
    }

    [[noreturn]] void Weather::Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        while (true)
        {
            Domain::WeatherSample sample{};
            if (driver_->Read(sample))
            {
                Domain::g_speed_m_per_s = 331.3f + 0.606f * sample.heatIndexC;
                Domain::g_speed_cm_per_us = Domain::g_speed_m_per_s / 10000.0f;

                Queues::Overwrite(gWeatherQueue, sample);
                WSServer::Broadcast("weather", sample);
            }
            else
            {
                ESP_LOGE(TAG, "Weather read invalid");
                Watchdog::ReportError(Domain::ErrorSeverity::Warning, Domain::ErrorType::SensorFailure, TAG, "Weather read invalid");
            }

            Watchdog::NotifyTaskAlive(TAG);
            Time::SleepMs(Settings::Get().WeatherDelayS * 1000);
        }
    }

    bool Weather::TryGetLatest(Domain::WeatherSample& out)
    {
        ESP_LOGV(TAG, "TryGetLatest");
        if (!gWeatherQueue)
            return false;

        return Queues::Receive(gWeatherQueue, out, 0);
    }

    bool Weather::ReadLast(Domain::WeatherSample& out)
    {
        ESP_LOGV(TAG, "TryGetLatest");
        if (!gWeatherQueue)
            return false;

        return Queues::Peek(gWeatherQueue, out);
    }

}   // namespace CE::Services