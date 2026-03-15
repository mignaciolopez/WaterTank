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
        unsigned short heightCm                    = 140u;              //Water tank height, max distance to measure
        unsigned short criticalLevelCm             = 20u;               //Critical water level to START pumping even during Nighttime.
        unsigned short minLevelCm                  = 50u;               //Minimum water level to START pumping
        unsigned short maxLevelCm                  = 130u;              //Maximum water level to STOP pumping - Sensor position

        //Ultrasonic Sensor
        unsigned short sensorRangeMinCm            = 25u;               //25cm for JRT

        //Measure delays
        unsigned short radarDelayS                 = 5u;               //Delay between distance measures
        unsigned short weatherDelayS               = 30u;              //Delay between weather updates
        unsigned short medianWindow                = 5u;               //Number of measures to median
        unsigned short watchdogDelayS              = 60u;              //Delay between health checks

        // Watchdog settings
        unsigned short wifiReconnectTimeoutS       = 30u;              //WiFi reconnect attempt timeout
        unsigned short maxServiceRestarts          = 10u;              //Max service restart attempts before ESP restart
        unsigned short minFreeHeapThresholdKb      = 20u;              //Minimum free heap (KB) before warning
        unsigned short maxErrorReports             = 50u;              //Max error reports to store in SPIFFS

        // Pump Settings
        unsigned short pumpMaxTimeOnM              = 20u;              //Max time in minutes Pump can be On
        unsigned short pumpCooldownTimeM           = 60u;              //Time in minutes Pump needs to cool down after a max Time On triggered.

        // Distance
        unsigned short filteredDistanceOffsetCm    = 0u;               //Distance offset in cm

        void toJson(const JsonVariant dst, const char* name = "settings") const
        {
            ESP_LOGD("Settings", "toJson");

            dst[name]["heightCm"]                      = heightCm;
            dst[name]["criticalLevelCm"]               = criticalLevelCm;
            dst[name]["minLevelCm"]                    = minLevelCm;
            dst[name]["maxLevelCm"]                    = maxLevelCm;

            dst[name]["sensorRangeMinCm"]              = sensorRangeMinCm;

            dst[name]["radarDelayS"]                   = radarDelayS;
            dst[name]["weatherDelayS"]                 = weatherDelayS;
            dst[name]["medianWindow"]                  = medianWindow;
            dst[name]["watchdogDelayS"]                = watchdogDelayS;

            dst[name]["wifiReconnectTimeoutS"]         = wifiReconnectTimeoutS;
            dst[name]["maxServiceRestarts"]            = maxServiceRestarts;
            dst[name]["minFreeHeapThresholdKb"]        = minFreeHeapThresholdKb;
            dst[name]["maxErrorReports"]               = maxErrorReports;

            dst[name]["pumpMaxTimeOnM"]                = pumpMaxTimeOnM;
            dst[name]["pumpCooldownTimeM"]             = pumpCooldownTimeM;

            dst[name]["filteredDistanceOffsetCm"]      = filteredDistanceOffsetCm;
        }

        void fromJson(const JsonObject obj, const char* name = "settings")
        {
            ESP_LOGD("Settings", "fromJson");
            //ESP_LOGI("Settings", "fromJson: obj[%s][%d]", name, obj[name]["pumpMaxTimeOnM"].as<unsigned short>());
            heightCm                    = obj[name]["heightCm"]                 | heightCm;
            criticalLevelCm             = obj[name]["criticalLevelCm"]          | criticalLevelCm;
            minLevelCm                  = obj[name]["minLevelCm"]               | minLevelCm;
            maxLevelCm                  = obj[name]["maxLevelCm"]               | maxLevelCm;

            sensorRangeMinCm            = obj[name]["sensorRangeMinCm"]         | sensorRangeMinCm;

            radarDelayS                 = obj[name]["radarDelayS"]              | radarDelayS;
            weatherDelayS               = obj[name]["weatherDelayS"]            | weatherDelayS;
            medianWindow                = obj[name]["medianWindow"]             | medianWindow;
            watchdogDelayS              = obj[name]["watchdogDelayS"]           | watchdogDelayS;

            wifiReconnectTimeoutS       = obj[name]["wifiReconnectTimeoutS"]    | wifiReconnectTimeoutS;
            maxServiceRestarts          = obj[name]["maxServiceRestarts"]       | maxServiceRestarts;
            minFreeHeapThresholdKb      = obj[name]["minFreeHeapThresholdKb"]   | minFreeHeapThresholdKb;
            maxErrorReports             = obj[name]["maxErrorReports"]          | maxErrorReports;

            pumpMaxTimeOnM              = obj[name]["pumpMaxTimeOnM"]           | pumpMaxTimeOnM;
            pumpCooldownTimeM           = obj[name]["pumpCooldownTimeM"]        | pumpCooldownTimeM;

            filteredDistanceOffsetCm    = obj[name]["filteredDistanceOffsetCm"] | filteredDistanceOffsetCm;
        }
    };

}   // namespace CE::Domain