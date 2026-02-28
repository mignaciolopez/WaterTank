//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/Settings.h>
#include <os/Time.hpp>
#include <os/Tasks.hpp>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <config/BuildConfig.hpp>

namespace CE::Services::Settings
{
    static Domain::Settings g_settings;

    static void PrintSettings()
    {
        ESP_LOGD(TAG, "Settings loaded: height_cm=%u, criticalLevel_cm=%u, minLevel_cm=%u, maxLevel_cm=%u, radarDelay_ms=%u, medianWindow=%u, weatherDelay_ms=%u",
               g_settings.height_cm,
               g_settings.criticalLevel_cm,
               g_settings.minLevel_cm,
               g_settings.maxLevel_cm,
               g_settings.radarDelay_ms,
               g_settings.medianWindow,
               g_settings.weatherDelay_ms);
    }

    [[nodiscard]] static bool Load()
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

        g_settings.height_cm            = doc["height_cm"]          | g_settings.height_cm;
        g_settings.criticalLevel_cm     = doc["criticalLevel_cm"]   | g_settings.criticalLevel_cm;
        g_settings.minLevel_cm          = doc["minLevel_cm"]        | g_settings.minLevel_cm;
        g_settings.maxLevel_cm          = doc["maxLevel_cm"]        | g_settings.maxLevel_cm;
        g_settings.radarDelay_ms        = doc["radarDelay_ms"]      | g_settings.radarDelay_ms;
        g_settings.medianWindow         = doc["medianWindow"]       | g_settings.medianWindow;
        g_settings.weatherDelay_ms      = doc["weatherDelay_ms"]    | g_settings.weatherDelay_ms;

        PrintSettings();

        return true;
    }

    [[nodiscard]] static bool Save()
    {
        ESP_LOGV(TAG, "Save");
        File file = SPIFFS.open("/settings.json", "w");
        if (!file)
        {
            ESP_LOGE(TAG, "Failed to open settings.json for writing");
            return false;
        }

        JsonDocument doc;
        doc["height_cm"]        = g_settings.height_cm;
        doc["criticalLevel_cm"] = g_settings.criticalLevel_cm;
        doc["minLevel_cm"]      = g_settings.minLevel_cm;
        doc["maxLevel_cm"]      = g_settings.maxLevel_cm;
        doc["radarDelay_ms"]    = g_settings.radarDelay_ms;
        doc["medianWindow"]     = g_settings.medianWindow;
        doc["weatherDelay_ms"]  = g_settings.weatherDelay_ms;

        const auto bytesWritten = serializeJson(doc, file);
        file.close();

        if (bytesWritten == 0)
        {
            ESP_LOGE(TAG, "Failed to write settings.json");
            return false;
        }

        PrintSettings();

        return true;
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");
        if (!SPIFFS.begin(true))
        {
            ESP_LOGE(TAG, "SPIFFS mount failed");
            return false;
        }

        if (!Load())
            if (!Save())
                return false;

        return true;
    }

    const Domain::Settings& Get()
    {
        return g_settings;
    }

} // namespace CE::Services::SettingsService