//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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

} // namespace CE::OS::Time