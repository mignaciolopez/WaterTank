//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once

#include <Arduino.h>
#include <ctime>
#include <esp_sntp.h>
#include <os/Time.hpp>


namespace CE::Services::NTP
{
    static auto TAG = "NPTService";

    static bool Setup()
    {
        // Configure timezone (e.g., EST: "EST5EDT,M3.2.0,M11.1.0" or simple offset)
        setenv("TZ", "ART3", 1);  // UTC-5 for EST, or use your timezone
        tzset();

        // Initialize SNTP
        esp_sntp_setoperatingmode(static_cast<esp_sntp_operatingmode_t>(SNTP_OPMODE_POLL));
        esp_sntp_setservername(0, "pool.ntp.org");
        esp_sntp_init();

        // Wait for time to be set
        ESP_LOGI(TAG, "Waiting for NTP time sync...");
        time_t now = 0;
        struct tm timeInfo = {};
        int retry = 0;
        constexpr int retry_count = 15;

        while (timeInfo.tm_year < (2020 - 1900) && ++retry < retry_count)
        {
            vTaskDelay(pdMS_TO_TICKS(2000));
            time(&now);
            localtime_r(&now, &timeInfo);
        }

        if (retry < retry_count)
        {
            timeInfo = OS::Time::GetLocalTime();
            char buffer[64];
            strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeInfo);
            ESP_LOGI(TAG, "Time synchronized: %s", buffer);
            return true;
        }
        else
        {
            ESP_LOGW(TAG, "Failed to sync time");
            return false;
        }
    }
}
