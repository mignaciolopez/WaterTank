//
// Created by lmart on 3/8/2026.
//

#pragma once

#include <ArduinoJson.h>

namespace CE::Domain::States
{
    struct Pump
    {
        const char* filename = "/PumpState.json";
        bool isOnCooldown_ = false;
        time_t timeStampOn_ = 0;
        time_t timeStampCooldown_ = 0;

        void toJson(JsonDocument obj) const
        {
            obj["isOnCooldown_"] = isOnCooldown_;
            obj["timeStampOn_"] = timeStampOn_;
            obj["timeStampCooldown_"] = timeStampCooldown_;
        }

        void fromJson(const JsonObject obj)
        {
            isOnCooldown_ = obj["isOnCooldown_"] | false;
            timeStampOn_ = obj["timeStampOn_"] | 0;
            timeStampCooldown_ = obj["timeStampCooldown_"] | 0;
        }
    };
}
