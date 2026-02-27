#pragma once
#include <Arduino.h>
#include <DHT.h>
#include <Settings.hpp>

namespace CE
{
    namespace Weather
    {
        static const char *TAG = "Weather";

        #define DHT_PIN 7
        #define DHT_TYPE DHT11
        QueueHandle_t WeatherQueue;
        struct Data { float h, t, hic; };

        [[noreturn]] inline void Task(void *pvParameters)
        {
            ESP_LOGV(TAG, "Task");

            DHT dht(DHT_PIN, DHT_TYPE);
            Data data{};
            dht.begin();

            vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.weatherDelay));

            while (true)
            {
                data.h = dht.readHumidity(true);
                data.t = dht.readTemperature(false, true);
                data.hic = dht.computeHeatIndex(data.t, data.h, false);

                if (!isnan(data.h) && !isnan(data.t) && data.h > 0)
                {
                    Globals::SPEED_CM_US = (331.3f + 0.606f * data.hic) / 10000.0f;
                    xQueueOverwrite(WeatherQueue, &data);
                }

                ESP_LOGD(TAG, "Humidity: %.2f%% Temperature: %.2f°C ", data.h, data.t);

                vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.weatherDelay));
                ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
            }
        }

        [[noreturn]] inline void OutputTask(void *pvParameters)
        {
            ESP_LOGV(TAG, "OutputTask");
            Data data{};

            while (true)
            {
                if (xQueueReceive(WeatherQueue, &data, portMAX_DELAY) == pdTRUE)
                {
                    ESP_LOGI(TAG, "Humidity: %.2f%% Temperature: %.2f°C Heat Index: %.2f°C", data.h, data.t, data.hic);
                }
                vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.weatherDelay));
            }
        }

        inline bool Init()
        {
            ESP_LOGV(TAG, "Init");

            WeatherQueue = xQueueCreate(1, sizeof(Data));

            xTaskCreate(Task, "WeatherTask", 4096, nullptr, 2, nullptr); //832~712
            xTaskCreate(OutputTask, "OutputWeatherTask",4096, nullptr, 1, nullptr);

            return true;
        }
    }
}