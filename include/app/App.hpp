//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include <Services/NTP.hpp>
#include <services/Filter.h>
#include "services/Pump.h"
#include <services/Radar.h>
#include <services/Settings.h>
#include <services/Weather.h>
#include "services/WiFi.hpp"

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

        if (!Services::Connection::Setup())
        {
            ESP_LOGE(TAG, "Connection Setup failed");
            return false;
        }

        if (!Services::NTP::Setup())
        {
            ESP_LOGE(TAG, "NTP Setup failed");
            return false;
        }

        if (!Services::Pump::Setup())
        {
            ESP_LOGE(TAG, "Pump Setup failed");
            return false;
        }

        if (!Services::Weather::Setup())
        {
            ESP_LOGE(TAG, "Weather Setup failed");
            return false;
        }

        if (!Services::Radar::Setup())
        {
            ESP_LOGE(TAG, "Radar Setup failed");
            return false;
        }

        if (!Services::Filter::Setup())
        {
            ESP_LOGE(TAG, "Filter Setup failed");
            return false;
        }

        ESP_LOGI(TAG, "Setup OK");
        return true;
    }

    inline void Loop()
    {
        Domain::WeatherSample sample = {};
        if (Services::Weather::TryGetLatest(sample))
        {
            ESP_LOGI(TAG, "Humidity: %.1f%% Temperature: %.1f°C Heat: %.1f°C.", sample.humidity, sample.tempC, sample.heatIndexC);
        }
    }

} // namespace CE::App