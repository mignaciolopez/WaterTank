//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <ArduinoJson.h>

#include "drivers/XKC-Y23.h"

namespace CE::Domain
{
    struct Settings
    {
        //Tank Water Distance levels measured from bottom to top
        unsigned short heightCm                    = 130u;             //Water tank height, max distance to measure
        unsigned short criticalLevelCm             = 20u;              //Critical water level to START pumping even during Nighttime.
        unsigned short minLevelCm                  = 50u;              //Minimum water level to START pumping
        unsigned short maxLevelCm                  = heightCm - 25u;   //Maximum water level to STOP pumping

        //Measure delays
        unsigned short RadarDelayS                 = 5u;               //Delay between distance measures
        unsigned short WeatherDelayS               = 30u;              //Delay between weather updates
        unsigned short MedianWindow                = 5u;               //Number of measures to median

        // Watchdog settings
        unsigned short WatchdogCheckIntervalS      = 60u;              //Delay between health checks
        unsigned short WifiReconnectTimeoutS       = 30u;              //WiFi reconnect attempt timeout
        unsigned short MaxServiceRestarts          = 10u;              //Max service restart attempts before ESP restart
        unsigned short MinFreeHeapThresholdKb      = 20u;              //Minimum free heap (KB) before warning
        unsigned short MaxErrorReports             = 50u;              //Max error reports to store in SPIFFS

        // Security Settings
        unsigned short PumpMaxTimeOnM              = 20u;              //Max time in minutes Pump can be On
        unsigned short PumpCooldownTimeM           = 60u;              //Time in minutes Pump needs to cool down after a max Time On triggered.

        // Distance
        unsigned short FilteredDistanceOffsetCm    = 0u;               //Distance offset in cm

        void toJson(const JsonVariant dst, const char* name = "settings") const
        {
            ESP_LOGD("Settings", "toJson");

            dst[name]["heightCm"]                      = heightCm;
            dst[name]["criticalLevelCm"]               = criticalLevelCm;
            dst[name]["minLevelCm"]                    = minLevelCm;
            dst[name]["maxLevelCm"]                    = maxLevelCm;

            dst[name]["RadarDelayS"]                   = RadarDelayS;
            dst[name]["WeatherDelayS"]                 = WeatherDelayS;
            dst[name]["MedianWindow"]                  = MedianWindow;

            dst[name]["WatchdogCheckIntervalS"]        = WatchdogCheckIntervalS;
            dst[name]["WifiReconnectTimeoutS"]         = WifiReconnectTimeoutS;
            dst[name]["MaxServiceRestarts"]            = MaxServiceRestarts;
            dst[name]["MinFreeHeapThresholdKb"]        = MinFreeHeapThresholdKb;
            dst[name]["MaxErrorReports"]               = MaxErrorReports;

            dst[name]["PumpMaxTimeOnM"]                = PumpMaxTimeOnM;
            dst[name]["PumpCooldownTimeM"]             = PumpCooldownTimeM;

            dst[name]["FilteredDistanceOffsetCm"]      = FilteredDistanceOffsetCm;
        }

        void fromJson(const JsonObject obj, const char* name = "settings")
        {
            ESP_LOGD("Settings", "fromJson");
            heightCm                    = obj[name]["heightCm"]                 | heightCm;
            criticalLevelCm             = obj[name]["criticalLevelCm"]          | criticalLevelCm;
            minLevelCm                  = obj[name]["minLevelCm"]               | minLevelCm;
            maxLevelCm                  = obj[name]["maxLevelCm"]               | maxLevelCm;

            RadarDelayS                 = obj[name]["RadarDelayS"]              | RadarDelayS;
            WeatherDelayS               = obj[name]["WeatherDelayS"]            | WeatherDelayS;
            MedianWindow                = obj[name]["MedianWindow"]             | MedianWindow;

            WatchdogCheckIntervalS      = obj[name]["WatchdogCheckIntervalS"]   | WatchdogCheckIntervalS;
            WifiReconnectTimeoutS       = obj[name]["WifiReconnectTimeoutS"]    | WifiReconnectTimeoutS;
            MaxServiceRestarts          = obj[name]["MaxServiceRestarts"]       | MaxServiceRestarts;
            MinFreeHeapThresholdKb      = obj[name]["MinFreeHeapThresholdKb"]   | MinFreeHeapThresholdKb;
            MaxErrorReports             = obj[name]["MaxErrorReports"]          | MaxErrorReports;

            PumpMaxTimeOnM              = obj[name]["PumpMaxTimeOnM"]           | PumpMaxTimeOnM;
            PumpCooldownTimeM           = obj[name]["PumpCooldownTimeM"]        | PumpCooldownTimeM;

            FilteredDistanceOffsetCm    = obj[name]["FilteredDistanceOffsetCm"] | FilteredDistanceOffsetCm;
        }
    };

}   // namespace CE::Domain