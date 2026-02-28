//
// Created by lmart on 2/27/2026.
//

#include <services/WeatherService.h>
#include <services/SettingsService.h>
#include <drivers/DhtDriver.h>
#include <config/Pins.hpp>
#include <os/Queues.hpp>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <config/BuildConfig.hpp>

namespace CE::Services::Weather
{
    static const char* TAG = "WeatherService";

    static QueueHandle_t g_queue = nullptr;

    float GetSpeedCmPerUs() { return g_speed_cm_per_us; }

    [[noreturn]] static void Task(void*)
    {
        Drivers::DhtDriver dht(Config::Pins::kDhtPin, DHT11);
        dht.Begin();

        while (true)
        {
            const auto& s = Settings::Get();

            Domain::WeatherSample sample{};
            if (dht.Read(sample))
            {
                g_speed_cm_per_us = 331.3f + 0.606f * sample.heatIndexC;

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

    bool Init()
    {
        g_queue = OS::Queues::CreateLatestQueue(sizeof(Domain::WeatherSample));
        if (!g_queue)
            return false;

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackWeatherTask, nullptr, Config::Build::kPrioWeather, nullptr);
    }

    bool TryGetLatest(Domain::WeatherSample& out)
    {
        if (!g_queue)
            return false;

        return OS::Queues::Receive(g_queue, out, 0);
    }

} // namespace CE::Services::WeatherService