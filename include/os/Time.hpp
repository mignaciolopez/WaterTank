//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <ctime>

namespace CE::OS::Time
{
    inline TickType_t MsToTicks(const uint32_t ms)
    {
        const TickType_t t = pdMS_TO_TICKS(ms);
        return (t == 0) ? 1 : t;
    }

    inline void SleepMs(const uint32_t ms)
    {
        vTaskDelay(MsToTicks(ms));
    }

    inline time_t Get()
    {
        return time(nullptr);
    }

    inline tm GetLocalTime()
    {
        const time_t now = Get();
        tm timeInfo = {};
        localtime_r(&now, &timeInfo);
        return timeInfo;
    }

    inline bool IsNightTime(const uint8_t nightStartHour = 22, const uint8_t nightEndHour = 8)
    {
        const tm timeInfo = GetLocalTime();
        const uint8_t hour = timeInfo.tm_hour;

        if (nightStartHour > nightEndHour)
        {
            // Night spans midnight (e.g., 22:00 to 8:00)
            return hour >= nightStartHour || hour < nightEndHour;
        }
        else
        {
            // Night doesn't span midnight (e.g., 1:00 to 8:00)
            return hour >= nightStartHour && hour < nightEndHour;
        }
    }

    inline void FormatTime(char* buffer, size_t size, const char* format = "%Y/%m/%d %H:%M:%S")
    {
        tm timeInfo = GetLocalTime();
        strftime(buffer, size, format, &timeInfo);
    }

    inline const char* GetFormattedTime(const char* format = "%Y/%m/%d %H:%M:%S")
    {
        static char buffer[64];
        FormatTime(buffer, sizeof(buffer), format);
        return buffer;
    }

}   // namespace CE::OS::Time