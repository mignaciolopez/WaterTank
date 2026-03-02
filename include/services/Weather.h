//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include "config/Pins.hpp"
#include <domain/Weather.hpp>
#include <drivers/DHTDevice.h>


namespace CE::Services::Weather
{
    static auto TAG = "Weather-Service";

    Drivers::DHTDevice driver(Config::Pins::kDhtPin, DHT11);

    [[nodiscard]] bool Setup();
    bool TryGetLatest(Domain::WeatherSample& out);

} // namespace CE::Services::Weather