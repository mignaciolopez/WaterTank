//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <domain/Weather.hpp>

namespace CE::Services::Weather
{
    static auto TAG = "WeatherService";

    [[nodiscard]] bool Setup();
    bool TryGetLatest(Domain::WeatherSample& out);

} // namespace CE::Services::Weather