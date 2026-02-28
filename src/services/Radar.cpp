//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/Radar.h>
#include <services/Settings.h>
#include <services/Weather.h>
#include <drivers/Radar.h>
#include <config/Pins.hpp>
#include <config/BuildConfig.hpp>
#include <os/Queues.hpp>
#include <os/Tasks.hpp>
#include <os/Time.hpp>

namespace CE::Services::Radar
{
    static QueueHandle_t g_queue = nullptr;

    const Drivers::Radar driver(Config::Pins::kTrigPin, Config::Pins::kEchoPin);

    [[noreturn]] static void Task(void* pvParameters)
    {
        ESP_LOGV(TAG, "Task");
        while (true)
        {
            const auto& settings = Settings::Get();

            unsigned short cm = 0;
            if (driver.ReadDistanceCm(cm))
            {
                OS::Queues::Overwrite(g_queue, cm);
                ESP_LOGD(TAG, "raw_cm=%.2f", cm / 100.0f);
            }
            else
            {
                ESP_LOGD(TAG, "timeout/no-echo");
            }

            OS::Time::SleepMs(settings.radarDelay_ms);
        }
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");
        g_queue = OS::Queues::CreateLatestQueue(sizeof(unsigned short));
        if (!g_queue)
            return false;

        driver.Setup();

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackRadarTask, nullptr, Config::Build::kPrioRadar, nullptr);
    }

    bool TryGetLatestRawCm(unsigned short& out_cm)
    {
        ESP_LOGV(TAG, "TryGetLatestRawCm");
        if (!g_queue)
            return false;

        return OS::Queues::Receive(g_queue, out_cm, 0);
    }

} // namespace CE::Services::RadarService