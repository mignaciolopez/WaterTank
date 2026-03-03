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
    inline bool Overwrite(QueueHandle_t q, const T& value)
    {
        return xQueueOverwrite(q, &value) == pdTRUE;
    }

    template <typename T>
    inline bool Receive(QueueHandle_t q, T& out, const TickType_t wait = portMAX_DELAY)
    {
        return xQueueReceive(q, &out, wait) == pdTRUE;
    }

    // Creates a queue with specified length for multi-item storage.
    template <typename T>
    inline QueueHandle_t CreateQueue(const std::size_t length)
    {
        return xQueueCreate(length, sizeof(T));
    }

    // Sends (pushes) an item to the back of the queue.
    template <typename T>
    inline bool Send(QueueHandle_t q, const T& value, const TickType_t wait = portMAX_DELAY)
    {
        return xQueueSend(q, &value, wait) == pdTRUE;
    }

    // Peeks at the front item without removing it from the queue.
    template <typename T>
    inline bool Peek(QueueHandle_t q, T& out, const TickType_t wait = 0)
    {
        return xQueuePeek(q, &out, wait) == pdTRUE;
    }

    // Gets the number of items currently in the queue.
    inline UBaseType_t GetCount(QueueHandle_t q)
    {
        return uxQueueMessagesWaiting(q);
    }

    // Gets the number of free spaces in the queue.
    inline UBaseType_t GetAvailableSpaces(QueueHandle_t q)
    {
        return uxQueueSpacesAvailable(q);
    }

}   // namespace CE::OS::Queues