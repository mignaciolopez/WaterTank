//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include <services/Settings.h>
#include <services/Weather.h>
#include <services/Radar.h>
#include <services/Filter.h>

namespace CE::App
{
    static const char* TAG = "App";

    inline bool Setup()
    {
        if (!Services::Settings::Setup())
        {
            ESP_LOGE(TAG, "Settings Setup failed");
            return false;
        }
        delay(500);

        if (!Services::Weather::Setup())
        {
            ESP_LOGE(TAG, "Weather Setup failed");
            return false;
        }
        delay(500);

        if (!Services::Radar::Setup())
        {
            ESP_LOGE(TAG, "Radar Setup failed");
            return false;
        }
        delay(500);

        if (!Services::Filter::Setup())
        {
            ESP_LOGE(TAG, "Filter Setup failed");
            return false;
        }
        delay(500);

        ESP_LOGI(TAG, "Setup OK");
        return true;
    }

    inline void Loop()
    {
        // A nice pattern: keep logging / UI / networking in the main loop
        // and keep acquisition/filtering in tasks.

        unsigned short filtered = 0;
        if (Services::Filter::TryGetLatestFilteredCm(filtered))
        {
            ESP_LOGI(TAG, "Distance: %.1fcm.", filtered / 100.0f);
        }

        Domain::WeatherSample sample = {};
        if (Services::Weather::TryGetLatest(sample))
        {
            ESP_LOGI(TAG, "Humidity: %.1f%% Temperature: %.1f°C Heat: %.1f°C.", sample.humidity, sample.tempC, sample.heatIndexC);
        }
    }

} // namespace CE::App