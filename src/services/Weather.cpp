//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/Weather.h>
#include <services/Settings.h>
#include <drivers/Weather.h>
#include <config/Pins.hpp>
#include <os/Queues.hpp>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <config/BuildConfig.hpp>

#include "domain/Global.hpp"

namespace CE::Services::Weather
{
    static QueueHandle_t g_queue = nullptr;

    [[noreturn]] static void Task(void*)
    {
        ESP_LOGV(TAG, "Task");
        Drivers::Weather dht(Config::Pins::kDhtPin, DHT11);
        dht.Setup();

        while (true)
        {
            const auto& s = Settings::Get();

            Domain::WeatherSample sample{};
            if (dht.Read(sample))
            {
                Domain::g_speed_m_per_s = 331.3f + 0.606f * sample.heatIndexC;
                Domain::g_speed_cm_per_us = Domain::g_speed_m_per_s / 10000.0f;

                OS::Queues::Overwrite(g_queue, sample);
                ESP_LOGD(TAG, "h=%.2f t=%.2f hic=%.2f", sample.humidity, sample.tempC, sample.heatIndexC);
            }
            else
            {
                ESP_LOGD(TAG, "Weather read invalid");
            }

            OS::Time::SleepMs(s.weatherDelay_ms);
        }
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");
        g_queue = OS::Queues::CreateLatestQueue(sizeof(Domain::WeatherSample));
        if (!g_queue)
            return false;

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackWeatherTask, nullptr, Config::Build::kPrioWeather, nullptr);
    }

    bool TryGetLatest(Domain::WeatherSample& out)
    {
        ESP_LOGV(TAG, "TryGetLatest");
        if (!g_queue)
            return false;

        return OS::Queues::Receive(g_queue, out, 0);
    }

} // namespace CE::Services::WeatherService