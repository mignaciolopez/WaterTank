#pragma once
#include <Arduino.h>
#include <Settings.hpp>
#include <vector>
#include "Radar.hpp"

namespace CE
{
    namespace Filters
    {
        static const char *TAG = "Filters";

        QueueHandle_t distanceQueue;

        static float median(const float *arr)
        {
            ESP_LOGV(TAG, "median");

            float temp[Settings::data.median];

            for (unsigned short i = 0; i < Settings::data.median; i++)
                temp[i] = arr[i];

            // Simple bubble sort (small array, very fast)
            for (unsigned short i = 0; i < Settings::data.median - 1; i++)
            {
                for (unsigned short j = i + 1; j < Settings::data.median; j++)
                {
                    if (temp[j] < temp[i])
                    {
                        float t = temp[i];
                        temp[i] = temp[j];
                        temp[j] = t;
                    }
                }
            }

            return temp[2]; // middle value
        }

        [[noreturn]] inline void Task(void *pvParameters)
        {
            ESP_LOGV(TAG, "Task");

            std::vector<float> window(Settings::data.median);
            float distance;
            int index = 0;
            bool filled = false;

            while (true)
            {
                ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
                if (xQueueReceive(Radar::rawDistanceQueue, &distance, portMAX_DELAY) == pdTRUE)
                {
                    if (distance > 0 && distance < Settings::data.height)
                    {
                        window[index] = distance;
                        index = (index + 1) % Settings::data.median;

                        if (index == 0)
                            filled = true;

                        if (filled)
                        {
                            float filtered = median(window.data());
                            xQueueOverwrite(distanceQueue, &filtered);
                        }
                    }
                }
                vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.radarDelay));
            }
        }

        [[noreturn]] inline void OutputTask(void *pvParameters)
        {
            ESP_LOGV(TAG, "OutputTask");

            float distance;

            while (true)
            {
                ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
                if (xQueueReceive(distanceQueue, &distance, portMAX_DELAY) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Distance: %.2fcm.", distance);
                }
                vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.radarDelay));
            }
        }

        inline bool Init()
        {
            ESP_LOGV(TAG, "Init");

            distanceQueue = xQueueCreate(1, sizeof(float));
            xTaskCreate(Task, "FilterTask",4096,nullptr, 2, nullptr); //384~568
            xTaskCreate(OutputTask, "OutputFilterTask",4096,nullptr, 1, nullptr);
            return true;
        }
    }
}