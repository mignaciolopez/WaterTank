#pragma once

#include <ArduinoJson.h>
#include <SPIFFS.h>

namespace CE
{
    namespace Settings
    {
        static const char *TAG = "Settings";

        struct Data
        {
            float height = 250.0f;
            unsigned short radarDelay = 1000;
            unsigned short median = 10;
            unsigned short weatherDelay = 10000;
            unsigned short reloadInterval = 60000;
        } data;

        inline void loadSettings()
        {
            ESP_LOGV(TAG, "loadSettings");

            File file = SPIFFS.open("/settings.json", "r");
            if (!file)
            {
                ESP_LOGE(TAG, "Failed to open settings file");
                return;
            }

            JsonDocument doc;
            const DeserializationError error = deserializeJson(doc, file);

            if (error)
            {
                ESP_LOGE(TAG, "JSON Error: %s", error.c_str());
                return;
            }

            data.height         = doc["height"]         | 250.0f;
            data.radarDelay     = doc["radarDelay"]     | 1000;
            data.median         = doc["median"]         | 10;
            data.weatherDelay   = doc["weatherDelay"]   | 10000;
            data.reloadInterval = doc["reloadInterval"] | 60000;

            file.close();
            ESP_LOGD(TAG, "Settings reloaded");
        }

        [[noreturn]] inline void Task(void* pvParameters)
        {
            ESP_LOGV(TAG, "Task");

            while(true)
            {
                loadSettings();
                vTaskDelay(pdMS_TO_TICKS(data.reloadInterval));
                ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
            }
        }

        inline bool Init()
        {
            ESP_LOGV(TAG, "Init");

            if (!SPIFFS.begin(true))
            {
                ESP_LOGE(TAG, "SPIFFS failed");
                return false;
            }

            xTaskCreate(Task, "SettingsTask", 4096, nullptr, 1, nullptr); //1236~1220

            return true;
        }
    }
}