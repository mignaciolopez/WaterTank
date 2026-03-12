//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <algorithm>
#include <array>
#include <config/BuildConfig.hpp>
#include <os/Queues.hpp>
#include <os/Settings.h>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <services/Filter.h>
#include <services/Radar.h>
#include <services/Watchdog.h>

using namespace CE::OS;

namespace CE::Services
{
    const char* Filter::TAG = "Filter-Service";
    QueueHandle_t Filter::gMedianDistanceQueue = nullptr;

    Filter::Filter() = default;

    bool Filter::Setup()
    {
        ESP_LOGV(TAG, "Setup");
        gMedianDistanceQueue = OS::Queues::CreateLatestQueue(sizeof(unsigned short));
        if (!gMedianDistanceQueue)
            return false;

        const bool taskResult = OS::Tasks::Start(Task, TAG, Config::Build::kStackFilterTask, nullptr, Config::Build::kPrioFilter, nullptr);
        const bool watchdogResult = Watchdog::RegisterTask(TAG, Settings::Get().RadarDelayS * 1000);

        return taskResult && watchdogResult;
    }

    unsigned short Filter::MedianInPlace(unsigned short* buf, const std::size_t n)
    {
        ESP_LOGV(TAG, "MedianInPlace");
        // For small n, nth_element is efficient and avoids full sort.
        const auto mid = buf + n / 2;
        std::nth_element(buf, mid, buf + n);
        return *mid;
    }

    void Filter::Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        std::array<unsigned short, Config::Build::kMedianMaxWindow> window{};
        std::size_t index = 0;
        bool filled = false;

        while (true)
        {
            const std::size_t winSize = std::min<std::size_t>(Settings::Get().MedianWindow, window.size());
            ESP_LOGD(TAG, "winSize=%d", winSize);

            unsigned short raw = 0;
            if (Radar::TryGetLatestRawCm(raw))
            {
                if (raw > 0 && raw < Settings::Get().heightCm * 100 && winSize >= 3)
                {
                    window[index] = raw;
                    index = (index + 1) % winSize;
                    if (index == 0)
                        filled = true;

                    if (filled)
                    {
                        // copy to temp for median (nth_element mutates)
                        std::array<unsigned short, Config::Build::kMedianMaxWindow> tmp = window;
                        unsigned short filtered = MedianInPlace(tmp.data(), winSize) + Settings::Get().FilteredDistanceOffsetCm * 100;
                        OS::Queues::Overwrite(gMedianDistanceQueue, filtered);
                        ESP_LOGD(TAG, "filtered_cm=%.2f", filtered / 100.0f);
                    }
                }
            }
            else
            {
                ESP_LOGE(TAG, "Radar::TryGetLatestRawCm Failed!");
                Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::SensorFailure, TAG, "Radar::TryGetLatestRawCm Failed!");
            }

            Watchdog::NotifyTaskAlive(TAG);
            Time::SleepMs(Settings::Get().RadarDelayS * 1000);
        }
    }

    bool Filter::TryGetLatestFilteredCm(unsigned short& out_cm)
    {
        ESP_LOGV(TAG, "TryGetLatestFilteredCm");
        if (!gMedianDistanceQueue)
            return false;

        return Queues::Peek(gMedianDistanceQueue, out_cm, 0);
    }

}   // namespace CE::Services