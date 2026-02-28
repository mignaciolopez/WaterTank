//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <cstdint>

namespace CE::Domain
{
    struct Settings
    {
        unsigned short height_cm            = 110u;     //Water tank height, max distance to measure
        unsigned short criticalLevel_cm     = 30u;      //Critical water level to START pumping even during Night time.
        unsigned short minLevel_cm          = 50u;      //Minimum water level to START pumping
        unsigned short maxLevel_cm          = 70u;      //Maximum water level to STOP pumping
        unsigned short radarDelay_ms        = 1000u;    //Delay between distance measures
        unsigned short medianWindow         = 10u;      //Number of measures to average
        unsigned short weatherDelay_ms      = 10000u;   //Delay between weather updates
    };

} // namespace CE::Domain