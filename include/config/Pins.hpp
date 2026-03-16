//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>
#include "DHT.h"

namespace CE::Config::Pins
{
    // LED
#if CONFIG_IDF_TARGET_ESP32C3
    static constexpr byte kBlueLed = 8;

    // DHT
    static constexpr byte kDhtPin = 3;
    static constexpr byte kDhtType = DHT11;

    // Ultrasonic
    static constexpr byte kTrigPin = 4;
    static constexpr byte kEchoPin = 5;

    //Pump
    static constexpr byte kPumpSwitchPin = 6;

#elif CONFIG_IDF_TARGET_ESP32
    static constexpr byte kBlueLed = 2;

    // DHT
    static constexpr byte kDhtPin = 21;
    static constexpr byte kDhtType = DHT11;

    // Ultrasonic
    static constexpr byte kTrigPin = 22;
    static constexpr byte kEchoPin = 23;

    //Pump
    static constexpr byte kPumpSwitchPin = 18;

#else
    static constexpr byte kBlueLed = 2;
#endif


}
