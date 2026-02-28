//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include <services/SettingsService.h>
#include <services/WeatherService.h>
#include <services/RadarService.h>
#include <services/FilterService.h>

namespace CE::App
{
    static const char* TAG = "App";

    inline bool Init()
    {
        if (!Services::Settings::Init())
        {
            ESP_LOGE(TAG, "Settings init failed");
            return false;
        }

        if (!Services::Weather::Init())
        {
            ESP_LOGE(TAG, "Weather init failed");
            return false;
        }

        if (!Services::Radar::Init())
        {
            ESP_LOGE(TAG, "Radar init failed");
            return false;
        }

        if (!Services::Filter::Init())
        {
            ESP_LOGE(TAG, "Filter init failed");
            return false;
        }

        ESP_LOGI(TAG, "Init OK");
        return true;
    }

    inline void loop()
    {
        // A nice pattern: keep logging / UI / networking in the main loop
        // and keep acquisition/filtering in tasks.

        float filtered = .0f;
        if (Services::Filter::TryGetLatestFilteredCm(filtered))
        {
            ESP_LOGI(TAG, "Distance: %.2fcm.", filtered);
        }

        Domain::WeatherSample sample = {};
        if (Services::Weather::TryGetLatest(sample))
        {
            ESP_LOGI(TAG, "Humidity: %.0f%% Temperature: %.0f°C Heat: %.0f°C.", sample.humidity, sample.tempC, sample.heatIndexC);
        }
    }

} // namespace CE::App