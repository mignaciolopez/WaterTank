//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include "ArduinoJson/Variant/VariantRefBase.hpp"

namespace CE::Config::Build
{
#if CONFIG_IDF_TARGET_ESP32C3
    static constexpr byte kLedOn = LOW;
    static constexpr byte kLedOff = HIGH;
#elif CONFIG_IDF_TARGET_ESP32
    static constexpr byte kLedOn = HIGH;
    static constexpr byte kLedOff = LOW;
#else
    static constexpr byte kLedOn = HIGH;
    static constexpr byte kLedOff = LOW;
#endif

    // Logging
    static constexpr bool kLogStackHighWatermark = false; // set true only for debugging

    // Filter limits (avoid dynamic memory and VLAs)
    static constexpr byte kMedianMaxWindow = 10U;

    // Task stacks (tune as needed)
    static constexpr unsigned short kStackWeatherTask   = 12288;
    static constexpr unsigned short kStackRadarTask     = 12288;
    static constexpr unsigned short kStackFilterTask    = 12288;
    static constexpr unsigned short kStackPumpTask      = 12288;
    static constexpr unsigned short kStackWatchdogTask  = 12288;

    // Task priorities
    static constexpr byte kPrioWeather   = 2;
    static constexpr byte kPrioRadar     = 2;
    static constexpr byte kPrioFilter    = 1;
    static constexpr byte kPrioPump      = 3;
    static constexpr byte kPrioWatchdog  = 1;

    static constexpr byte kWatchdogMaxMissedNotifies = 5;

}   // namespace CE::config::Build