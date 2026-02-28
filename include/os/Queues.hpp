//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/queue.h>

namespace CE::OS::Queues
{
    // Creates a queue sized for "latest sample" pattern (len=1).
    inline QueueHandle_t CreateLatestQueue(const std::size_t itemSize)
    {
        return xQueueCreate(1, itemSize);
    }

    template <typename T>
    inline bool Overwrite(const QueueHandle_t q, const T& value)
    {
        return xQueueOverwrite(q, &value) == pdTRUE;
    }

    template <typename T>
    inline bool Receive(const QueueHandle_t q, T& out, const TickType_t wait = portMAX_DELAY)
    {
        return xQueueReceive(q, &out, wait) == pdTRUE;
    }

} // namespace CE::OS::Queues