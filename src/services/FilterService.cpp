//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/FilterService.h>
#include <services/SettingsService.h>
#include <services/RadarService.h>
#include <config/BuildConfig.hpp>
#include <os/Queues.hpp>
#include <os/Tasks.hpp>
#include <os/Time.hpp>
#include <esp_log.h>
#include <array>
#include <algorithm>
#include <cstddef>

namespace CE::Services::Filter
{
    static const char* TAG = "FilterService";

    static QueueHandle_t g_queue = nullptr;

    static float MedianInPlace(float* buf, std::size_t n)
    {
      // For small n, nth_element is efficient and avoids full sort.
      const auto mid = buf + n / 2;
      std::nth_element(buf, mid, buf + n);
      return *mid;
    }

    [[noreturn]] static void Task(void*)
    {
      std::array<float, Config::Build::kMedianMaxWindow> window{};
      std::size_t index = 0;
      bool filled = false;

      while (true)
      {
        const auto& s = Settings::Get();
        const std::size_t winSize = std::min<std::size_t>(s.medianWindow, window.size());

        float raw = 0;
        if (Radar::TryGetLatestRawCm(raw))
        {
          if (raw > 0.0f && raw < s.height_cm && winSize >= 3)
          {
            window[index] = raw;
            index = (index + 1) % winSize;
            if (index == 0)
                filled = true;

            if (filled)
            {
              // copy to temp for median (nth_element mutates)
              std::array<float, Config::Build::kMedianMaxWindow> tmp = window;
              float filtered = MedianInPlace(tmp.data(), winSize);
              OS::Queues::Overwrite(g_queue, filtered);
              ESP_LOGD(TAG, "filtered_cm=%.2f", filtered);
            }
          }
        }

        OS::Time::SleepMs(s.radarDelay_ms);
      }
    }

    bool Init()
    {
      g_queue = OS::Queues::CreateLatestQueue(sizeof(float));
      if (!g_queue)
          return false;

      return OS::Tasks::Start(Task, TAG, Config::Build::kStackFilterTask, nullptr, Config::Build::kPrioFilter, nullptr);
    }

    bool TryGetLatestFilteredCm(float& out_cm)
    {
      if (!g_queue)
          return false;

      return OS::Queues::Receive(g_queue, out_cm, 0);
    }

} // namespace CE::Services::FilterService