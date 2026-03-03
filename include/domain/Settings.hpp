//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <cstdint>

namespace CE::Domain
{
    struct Settings
    {
                                                                //Water levels measured from bottom to top
        unsigned short height_cm            = 140u;             //Water tank height, max distance to measure
        unsigned short criticalLevel_cm     = 20u;              //Critical water level to START pumping even during Nighttime.
        unsigned short minLevel_cm          = 50u;              //Minimum water level to START pumping
        unsigned short maxLevel_cm          = height_cm - 30u;  //Maximum water level to STOP pumping
        unsigned short radarDelay_ms        = 5000u;            //Delay between distance measures
        unsigned short medianWindow         = 5u;               //Number of measures to median
        unsigned short weatherDelay_ms      = 10000u;           //Delay between weather updates

        // Watchdog settings
        unsigned short watchdogCheckInterval_ms = 15000u;   //Delay between health checks
        unsigned short wifiReconnectTimeout_s   = 60u;      //WiFi reconnect attempt timeout
        unsigned short maxServiceRestarts       = 10u;      //Max service restart attempts before ESP restart
        unsigned short minFreeHeapThreshold_kb  = 20u;      //Minimum free heap (KB) before warning
        unsigned short maxErrorReports          = 50u;      //Max error reports to store in SPIFFS

        // Security Settings
        unsigned short pumpMaxTimeOn_m          = 1u;      //Max time in minutes Pump can be On
        unsigned short pumpCooldownTime_m       = 1u;      //Time in minutes Pump needs to cool down after a max Time On triggered.
    };

}   // namespace CE::Domain