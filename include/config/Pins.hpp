//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

namespace CE::Config::Pins
{
    // LED
#if CONFIG_IDF_TARGET_ESP32C3
    static constexpr int kBlueLed = 8;
#elif CONFIG_IDF_TARGET_ESP32
    static constexpr int kBlueLed = 2;
#else
    static constexpr int kBlueLed = 2;
#endif

    // DHT
    static constexpr int kDhtPin = 7;

    // Ultrasonic
    static constexpr int kTrigPin = 4;
    static constexpr int kEchoPin = 2;
}