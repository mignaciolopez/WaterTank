//
// Created by lmartinignacio@gmail.com on 3/11/2026.
//

#pragma once

#include <ArduinoJson.h>

namespace CE::Domain
{
    struct WeatherSample
    {
        float temperatureC  = .0f;
        float humidity      = .0f;
        float heatIndexC    = .0f;

        void toJson(const JsonVariant dst, const char* name = "weather") const
        {
            ESP_LOGD("Wather", "toJson");
            dst[name]["temperatureC"]  = temperatureC;
            dst[name]["humidity"]      = humidity;
            dst[name]["heatIndexC"]    = heatIndexC;
        }

        void fromJson(const JsonObject obj, const char* name = "weather")
        {
            ESP_LOGD("Wather", "fromJson");
            temperatureC  = obj[name]["temperatureC"]  | temperatureC;
            humidity      = obj[name]["humidity"]      | humidity;
            heatIndexC    = obj[name]["heatIndexC"]    | heatIndexC;
        }
    };
}   // namespace CE::Domain