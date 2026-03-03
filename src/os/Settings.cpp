//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <ArduinoJson.h>
#include <os/Settings.h>
#include <SPIFFS.h>

namespace CE::OS
{
    const char* Settings::TAG = "Settings";
    Domain::Settings* Settings::settings = nullptr;

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

        settings = new Domain::Settings();

        if (!Load())
            if (!Save())
                return false;

        return true;
    }

    Domain::Settings& Settings::Get()
    {
        if (!settings)
            settings = new Domain::Settings();

        return *settings;
    }

    void Settings::Print()
    {
        if (!settings)
        {
            ESP_LOGW(TAG, "settings not initialized");
            return;
        }

        ESP_LOGD(TAG, "height_cm=%u, criticalLevel_cm=%u, minLevel_cm=%u, maxLevel_cm=%u, radarDelay_ms=%u, medianWindow=%u, weatherDelay_ms=%u",
               settings->height_cm,
               settings->criticalLevel_cm,
               settings->minLevel_cm,
               settings->maxLevel_cm,
               settings->radarDelay_ms,
               settings->medianWindow,
               settings->weatherDelay_ms);
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
            ESP_LOGW(TAG, "settings.json parse error; using previous/default values");
            return false;
        }

        auto& s = Get();

        s.height_cm            = doc["height_cm"]          | s.height_cm;
        s.criticalLevel_cm     = doc["criticalLevel_cm"]   | s.criticalLevel_cm;
        s.minLevel_cm          = doc["minLevel_cm"]        | s.minLevel_cm;
        s.maxLevel_cm          = doc["maxLevel_cm"]        | s.maxLevel_cm;
        s.radarDelay_ms        = doc["radarDelay_ms"]      | s.radarDelay_ms;
        s.medianWindow         = doc["medianWindow"]       | s.medianWindow;
        s.weatherDelay_ms      = doc["weatherDelay_ms"]    | s.weatherDelay_ms;

        Print();

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

        const auto s = Get();

        JsonDocument doc;
        doc["height_cm"]        = s.height_cm;
        doc["criticalLevel_cm"] = s.criticalLevel_cm;
        doc["minLevel_cm"]      = s.minLevel_cm;
        doc["maxLevel_cm"]      = s.maxLevel_cm;
        doc["radarDelay_ms"]    = s.radarDelay_ms;
        doc["medianWindow"]     = s.medianWindow;
        doc["weatherDelay_ms"]  = s.weatherDelay_ms;

        const auto bytesWritten = serializeJson(doc, file);
        file.close();

        if (bytesWritten == 0)
        {
            ESP_LOGE(TAG, "Failed to write settings.json");
            return false;
        }

        Print();

        return true;
    }

}   // namespace CE::Services