//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <cstdint>

namespace CE::Domain
{
    struct Settings
    {
        unsigned short height_cm            = 450u;     //Water tank height, max distance to measure
        unsigned short radarDelay_ms        = 1000u;    //Delay between distance measures
        unsigned short medianWindow         = 10u;      //Number of measures to average
        unsigned short weatherDelay_ms      = 10000u;   //Delay between weather updates
        unsigned short reloadInterval_ms    = 60000u;   //Settings reload interval
    };

} // namespace CE::Domain