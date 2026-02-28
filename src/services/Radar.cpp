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

    [[noreturn]] static void Task(void* pvParameters)
    {
        Drivers::Radar us(Config::Pins::kTrigPin, Config::Pins::kEchoPin, Config::Build::kPulseInTimeoutUs);

        us.Setup();

        while (true)
        {
            const auto& settings = Settings::Get();

            float cm = .0f;
            if (us.ReadDistanceCm(Weather::g_speed_cm_per_us, cm))
            {
                OS::Queues::Overwrite(g_queue, cm);
                ESP_LOGD(TAG, "raw_cm=%.2f", cm);
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
        g_queue = OS::Queues::CreateLatestQueue(sizeof(float));
        if (!g_queue)
            return false;

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackRadarTask, nullptr, Config::Build::kPrioRadar, nullptr);
    }

    bool TryGetLatestRawCm(float& out_cm)
    {
        if (!g_queue)
            return false;

        return OS::Queues::Receive(g_queue, out_cm, 0);
    }

} // namespace CE::Services::RadarService