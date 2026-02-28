//
// Created by lmartinignacio@outlook.com on 2/27/2026.
//

#pragma once
#include <cstdint>

namespace CE::Domain
{
    struct Settings
    {
        unsigned short height_cm = 250u;
        unsigned short radarDelay_ms = 1000u;
        unsigned short medianWindow = 10u;
        unsigned short weatherDelay_ms = 10000u;
        unsigned short reloadInterval_ms = 60000u;
    };

} // namespace CE::Domain