//
// Created by lmart on 2/27/2026.
//

#pragma once
#include <domain/WeatherModel.hpp>

namespace CE::Services::Weather
{
    static float g_speed_cm_per_us = 331.3f + 0.606f * 20.0f;
    bool Init();
    bool TryGetLatest(Domain::WeatherSample& out);

} // namespace CE::Services::Weather