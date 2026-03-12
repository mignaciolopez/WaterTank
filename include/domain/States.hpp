//
// Created by lmartinignacio@gmail.com on 3/8/2026.
//

#pragma once

#include <ArduinoJson.h>
#include "services/WaterLevel.h"

namespace CE::Domain::States
{
    struct Pump
    {
        const char* filename = "/PumpState.json";
        bool isOnCooldown = false;
        bool isOn = false;
        time_t timeStampOn = 0;
        time_t timeStampOff = 0;
        time_t timeStampCooldown = 0;
        Services::WaterLevel::WaterLevel waterLevel = Services::WaterLevel::WaterLevel::Unknown;

        void toJson(const JsonVariant dst, const char* name = "pump") const
        {
            ESP_LOGD("Pump", "toJson");
            dst[name]["isOnCooldown"]         = isOnCooldown;
            dst[name]["isOn"]                 = isOn;
            dst[name]["timeStampOn"]          = timeStampOn;
            dst[name]["timeStampOff"]         = timeStampOff;
            dst[name]["timeStampCooldown"]    = timeStampCooldown;
        }

        void fromJson(const JsonObject obj, const char* name = "pump")
        {
            ESP_LOGD("Pump", "fromJson");
            isOnCooldown        = obj[name]["isOnCooldown"]       | false;
            isOn                = obj[name]["isOn"]               | false;
            timeStampOn         = obj[name]["timeStampOn"]        | 0;
            timeStampOff        = obj[name]["timeStampOff"]       | 0;
            timeStampCooldown   = obj[name]["timeStampCooldown"]  | 0;
        }
    };
}
