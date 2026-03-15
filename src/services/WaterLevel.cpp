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
        if (distance > s.heightCm)
        {
            ESP_LOGE(TAG, "Distance %ucm is higher than configured water tank height: %ucm", distance, s.heightCm);
            char msg[128];
            snprintf(msg, sizeof(msg), "Measured distance %ucm is higher than configured water tank height: %ucm", distance, s.heightCm);
            Watchdog::ReportError(Domain::ErrorSeverity::Critical, Domain::ErrorType::ConfigError, TAG, msg);
            return Invalid;
        }

        if (distance == 0)
        {
            return Unknown;
        }

        if (distance <= s.sensorRangeMinCm)
        {
            return Full;
        }
        if (distance > s.sensorRangeMinCm && distance <= s.maxLevelCm - s.sensorRangeMinCm - s.minLevelCm)
        {
            return Normal;
        }
        if (distance > s.maxLevelCm - s.sensorRangeMinCm - s.minLevelCm && distance <= s.maxLevelCm - s.sensorRangeMinCm - s.criticalLevelCm)
        {
            return Low;
        }
        if (distance > s.maxLevelCm - s.sensorRangeMinCm - s.criticalLevelCm)
        {
            return Critical;
        }

        return Unknown;
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
