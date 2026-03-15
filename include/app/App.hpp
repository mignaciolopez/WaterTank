//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include <services/Filter.h>
#include <Services/NTP.hpp>
#include "services/Pump.h"
#include <services/Radar.h>
#include <os/Settings.h>
#include <services/Watchdog.h>
#include <services/Weather.h>
#include "services/WiFi.hpp"
#include "services/WSServer.h"

namespace CE::App
{
    static auto TAG = "App";

    inline bool Setup()
    {
        if (!OS::Settings::Setup())
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

        if (!Services::WSServer::Setup())
        {
            ESP_LOGE(TAG, "WSServer Setup failed");
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

        if (!Services::Watchdog::Setup())
        {
            ESP_LOGE(TAG, "Watchdog Setup failed");
            return false;
        }

        ESP_LOGI(TAG, "Setup OK");
        return true;
    }

    inline void Loop()
    {
        Services::WSServer::Loop();

        Domain::WeatherSample sample = {};
        if (Services::Weather::ReadLast(sample))
        {
            ESP_LOGI(TAG, "Humidity: %.1f%% Temperature: %.1f°C Heat: %.1f°C.",
                sample.humidity, sample.temperatureC, sample.heatIndexC);
        }

        delay(OS::Settings::Get().weatherDelayS * 1000);
    }

}   // namespace CE::App