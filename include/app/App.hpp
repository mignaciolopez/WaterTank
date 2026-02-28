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
        delay(1000);

        if (!Services::Weather::Setup())
        {
            ESP_LOGE(TAG, "Weather Setup failed");
            return false;
        }
        delay(1000);

        if (!Services::Radar::Setup())
        {
            ESP_LOGE(TAG, "Radar Setup failed");
            return false;
        }
        delay(1000);

        if (!Services::Filter::Setup())
        {
            ESP_LOGE(TAG, "Filter Setup failed");
            return false;
        }
        delay(1000);

        ESP_LOGI(TAG, "Setup OK");
        return true;
    }

    inline void Loop()
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