//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/Filter.h>
#include <services/Settings.h>
#include <services/Radar.h>
#include <config/BuildConfig.hpp>
#include <os/Queues.hpp>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <array>
#include <algorithm>
#include <cstddef>

#include "domain/Global.hpp"

namespace CE::Services::Filter
{
    static QueueHandle_t g_queue = nullptr;

    static unsigned short MedianInPlace(unsigned short* buf, const std::size_t n)
    {
        ESP_LOGV(TAG, "MedianInPlace");
        // For small n, nth_element is efficient and avoids full sort.
        const auto mid = buf + n / 2;
        std::nth_element(buf, mid, buf + n);
        return *mid;
    }

    [[noreturn]] static void Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        std::array<unsigned short, Config::Build::kMedianMaxWindow> window{};
        std::size_t index = 0;
        bool filled = false;

        while (true)
        {
            const auto& s = Settings::Get();
            const std::size_t winSize = std::min<std::size_t>(s.medianWindow, window.size());
            ESP_LOGD(TAG, "winSize=%d", winSize);

            unsigned short raw = 0;
            if (Radar::TryGetLatestRawCm(raw))
            {
                if (raw > 0 && raw < s.height_cm * 100 && winSize >= 3)
                {
                    window[index] = raw;
                    index = (index + 1) % winSize;
                    if (index == 0)
                        filled = true;

                    if (filled)
                    {
                        // copy to temp for median (nth_element mutates)
                        std::array<unsigned short, Config::Build::kMedianMaxWindow> tmp = window;
                        unsigned short filtered = MedianInPlace(tmp.data(), winSize) + Domain::kFilteredDistanceOffset;
                        OS::Queues::Overwrite(g_queue, filtered);
                        ESP_LOGD(TAG, "filtered_cm=%.2f", filtered / 100.0f);
                    }
                }
            }
            else
            {
                ESP_LOGW(TAG, "Radar::TryGetLatestRawCm Failed!");
            }

            OS::Time::SleepMs(s.radarDelay_ms);
        }
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");
      g_queue = OS::Queues::CreateLatestQueue(sizeof(unsigned short));
      if (!g_queue)
          return false;

      return OS::Tasks::Start(Task, TAG, Config::Build::kStackFilterTask, nullptr, Config::Build::kPrioFilter, nullptr);
    }

    bool TryGetLatestFilteredCm(unsigned short& out_cm)
    {
        ESP_LOGV(TAG, "TryGetLatestFilteredCm");
      if (!g_queue)
          return false;

      return OS::Queues::Receive(g_queue, out_cm, 0);
    }

} // namespace CE::Services::FilterService