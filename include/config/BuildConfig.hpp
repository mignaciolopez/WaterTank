//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include <cstddef>
#include "ArduinoJson/Variant/VariantRefBase.hpp"

namespace CE::Config::Build
{
#if CONFIG_IDF_TARGET_ESP32C3
    static constexpr std::size_t kLedOn = LOW;
    static constexpr std::size_t kLedOff = HIGH;
#elif CONFIG_IDF_TARGET_ESP32
    static constexpr std::size_t kBlueLedOn = HIGH;
    static constexpr std::size_t kBlueLedOff = LOW;
#else
    static constexpr std::size_t kBlueLedOn = HIGH;
    static constexpr std::size_t kBlueLedOff = LOW;
#endif

    // Logging
    static constexpr bool kLogStackHighWatermark = false; // set true only for debugging

    // Filter limits (avoid dynamic memory and VLAs)
    static constexpr std::size_t kMedianMaxWindow = 10U;

    // Task stacks (tune as needed)
    static constexpr uint32_t kStackWeatherTask = 4096;
    static constexpr uint32_t kStackRadarTask   = 4096;
    static constexpr uint32_t kStackFilterTask  = 4096;
    static constexpr uint32_t kStackPumpTask    = 4096;

    // Task priorities
    static constexpr UBaseType_t kPrioWeather   = 2;
    static constexpr UBaseType_t kPrioRadar     = 2;
    static constexpr UBaseType_t kPrioFilter    = 2;
    static constexpr UBaseType_t kPrioPump      = 3;

} // namespace CE::config::Build