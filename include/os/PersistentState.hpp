//
// Created by lmart on 3/8/2026.
//

#pragma once

#include <FS.h>
#include <SPIFFS.h>

#include "ArduinoJson.h"

namespace CE::OS
{
    template <typename T>
    class PersistentState
    {
    public:
        const char* TAG = "PersistentState";

        explicit PersistentState(const T& data) : data_(data)
        {
            ESP_LOGV(TAG, "Constructor");
        }

        bool Read()
        {
            ESP_LOGD(TAG, "Reading from %s.", data_.filename);

            File file = SPIFFS.open(data_.filename, "r");
            if (!file)
            {
                ESP_LOGE(TAG, "File %s not found!!!", data_.filename);
                return false;
            }

            JsonDocument doc;
            const DeserializationError err = deserializeJson(doc, file);
            file.close();

            if (err)
            {
                ESP_LOGE(TAG, "Failed to parse %s: %s!!!", data_.filename, err.c_str());
                return false;
            }

            data_.fromJson(doc.as<JsonObject>());
            ESP_LOGI(TAG, "Successfully read from %s.", data_.filename);
            return true;
        }

        bool Save()
        {
            ESP_LOGD(TAG, "Saving to %s.", data_.filename);

            File file = SPIFFS.open(data_.filename, "w");
            if (!file)
            {
                ESP_LOGE(TAG, "Failed to open %s for writing!!!", data_.filename);
                return false;
            }

            JsonDocument doc;
            data_.toJson(doc);

            const size_t bytesWritten = serializeJson(doc, file);
            file.close();

            if (bytesWritten == 0)
            {
                ESP_LOGE(TAG, "Failed to write to %s!!!", data_.filename);
                return false;
            }

            ESP_LOGI(TAG, "Saved %u bytes to %s.", bytesWritten, data_.filename);
            return true;
        }

        // Get reference to internal data
        T& Get()
        {
            return data_;
        }

        // Get const reference to internal data
        const T& Get() const
        {
            return data_;
        }

        // Set internal data
        void Set(const T& newData)
        {
            data_ = newData;
        }

    private:
        T data_;
    };
} // CE::OS