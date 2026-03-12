//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <ArduinoJson.h>
#include <os/Settings.h>
#include <SPIFFS.h>

namespace CE::OS
{
    const char* Settings::TAG = "Settings";
    Domain::Settings* Settings::_settings = nullptr;

    Settings::Settings()
    {
        ESP_LOGV(TAG, "Constructor");
    }

    bool Settings::Setup()
    {
        ESP_LOGV(TAG, "Setup");
        if (!SPIFFS.begin(true))
        {
            ESP_LOGE(TAG, "SPIFFS mount failed");
            return false;
        }

        _settings = new Domain::Settings();

        if (!Load())
        {
            if (!Save())
            {
                return false;
            }
        }

        return true;
    }

    Domain::Settings& Settings::Get()
    {
        if (!_settings)
            _settings = new Domain::Settings();

        return *_settings;
    }

    bool Settings::Load()
    {
        ESP_LOGV(TAG, "Load");
        File file = SPIFFS.open("/settings.json", "r");
        if (!file)
        {
            ESP_LOGW(TAG, "settings.json not found; using defaults");
            return false;
        }

        JsonDocument doc;
        const auto err = deserializeJson(doc, file);
        file.close();

        if (err)
        {
            ESP_LOGW(TAG, "settings.json parse error; using previous values");
            return false;
        }

        _settings->fromJson(doc.as<JsonObject>());

        return true;
    }

    [[nodiscard]] bool Settings::Save()
    {
        ESP_LOGV(TAG, "Save");
        File file = SPIFFS.open("/settings.json", "w");
        if (!file)
        {
            ESP_LOGE(TAG, "Failed to open settings.json for writing");
            return false;
        }

        ESP_LOGV(TAG, "doc");
        JsonDocument doc;
        ESP_LOGV(TAG, "toJson(doc)");
        _settings->toJson(doc.as<JsonVariant>());
        ESP_LOGV(TAG, "serializeJson(doc, file)");
        const auto bytesWritten = serializeJson(doc, file);
        ESP_LOGV(TAG, "file.close()");
        file.close();

        if (bytesWritten == 0)
        {
            ESP_LOGE(TAG, "Failed to write settings.json");
            return false;
        }

        return true;
    }

}   // namespace CE::Services