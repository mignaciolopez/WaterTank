//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <services/SettingsService.h>
#include <os/Time.hpp>
#include <os/Tasks.hpp>
#include <SPIFFS.h>
#include <ArduinoJson.h>
#include <config/BuildConfig.hpp>

namespace CE::Services::Settings
{
    static const char* TAG = "SettingsService";

    static Domain::Settings g_settings;

    static void Load()
    {
      File file = SPIFFS.open("/settings.json", "r");
      if (!file)
      {
        ESP_LOGW(TAG, "settings.json not found; using defaults");
        return;
      }

      JsonDocument doc;
      const auto err = deserializeJson(doc, file);
      file.close();

      if (err)
      {
        ESP_LOGW(TAG, "settings.json parse error; using previous/default values");
        return;
      }

      g_settings.height_cm          = doc["height_cm"]         | g_settings.height_cm;
      g_settings.radarDelay_ms      = doc["radarDelay_ms"]     | g_settings.radarDelay_ms;
      g_settings.medianWindow       = doc["medianWindow"]      | g_settings.medianWindow;
      g_settings.weatherDelay_ms    = doc["weatherDelay_ms"]   | g_settings.weatherDelay_ms;
      g_settings.reloadInterval_ms  = doc["reloadInterval_ms"] | g_settings.reloadInterval_ms;

      ESP_LOGD(TAG, "Settings loaded: height_cm=%u radarDelay_ms=%u medianWindow=%u weatherDelay_ms=%u reloadInterval_ms=%u",
               g_settings.height_cm,
               g_settings.radarDelay_ms,
               g_settings.medianWindow,
               g_settings.weatherDelay_ms,
               g_settings.reloadInterval_ms);
    }

    [[noreturn]] static void Task(void*)
    {
      while (true)
      {
        Load();
        OS::Time::SleepMs(g_settings.reloadInterval_ms);
      }
    }

    bool Init()
    {
      if (!SPIFFS.begin(true))
      {
        ESP_LOGE(TAG, "SPIFFS mount failed");
        return false;
      }

      OS::Tasks::Start(Task, TAG, Config::Build::kStackSettingsTask, nullptr, Config::Build::kPrioSettings, nullptr);
      return true;
    }

    const Domain::Settings& Get()
    {
      return g_settings;
    }

} // namespace CE::Services::SettingsService