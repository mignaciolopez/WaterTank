//
// Created by lmart on 2/28/2026.
//

#pragma once

#include <Arduino.h>
#include <WiFi.h>
#include "secrets.h"

namespace CE::Services::Connection
{
    static auto TAG = "ConnectionService";

    [[nodiscard]] static bool Setup()
    {
        ESP_LOGI(TAG, "Connecting to WiFi SSID: %s", WIFI_SSID);

        WiFiClass::mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASS);

        int retry = 0;
        constexpr int maxRetries = 40; // 20 seconds

        while (WiFiClass::status() != WL_CONNECTED && retry < maxRetries)
        {
            delay(500);
            ESP_LOGV(TAG, "Connecting... (%d/%d)", retry + 1, maxRetries);
            retry++;
        }

        if (WiFiClass::status() == WL_CONNECTED)
        {
            ESP_LOGI(TAG, "Connected to WiFi");
            ESP_LOGI(TAG, "IP Address: %s", ::WiFi.localIP().toString().c_str());
            ESP_LOGI(TAG, "Signal Strength (RSSI): %d dBm", ::WiFi.RSSI());
            return true;
        }
        else
        {
            ESP_LOGE(TAG, "Failed to connect to WiFi");
            return false;
        }
    }
}
