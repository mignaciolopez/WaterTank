//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include <Arduino.h>
#include <os/Settings.h>
#include <services/WaterLevel.h>

#include "services/Watchdog.h"

using namespace CE::OS;

namespace CE::Services::WaterLevel
{
    WaterLevel GetWaterLevelState(const unsigned short distance)
    {
        const auto s = Settings::Get();
        if (distance > s.height_cm)
        {
            ESP_LOGE(TAG, "Distance %ucm is higher than configured water tank height: %ucm", distance, s.height_cm);
            char msg[128];
            snprintf(msg, sizeof(msg), "Measured distance %ucm is higher than configured water tank height: %ucm", distance, s.height_cm);
            Watchdog::ReportError(Domain::ErrorSeverity::Critical, Domain::ErrorType::ConfigError, TAG, msg);
            return Invalid;
        }

        if (distance <= s.height_cm - s.maxLevel_cm)
        {
            return Full;
        }
        if (distance > s.height_cm - s.maxLevel_cm && distance <= s.height_cm - s.minLevel_cm)
        {
            return Normal;
        }
        if (distance > s.height_cm - s.minLevel_cm && distance <= s.height_cm - s.criticalLevel_cm)
        {
            return Low;
        }
        if (distance > s.height_cm - s.criticalLevel_cm)
        {
            return Critical;
        }

        return Invalid;
    }

    const char* GetWaterLevelString(const unsigned short distance)
    {
        return StatusToString(GetWaterLevelState(distance));
    }

    const char* StatusToString(const WaterLevel status)
    {
        switch(status)
        {
            case Invalid: return "Invalid";
            case Full: return "Full";
            case Normal: return "Normal";
            case Low: return "Low";
            case Critical: return "Critical";
            default: return "Unknown";
        }
    }
}
