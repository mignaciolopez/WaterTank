//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class Pump
    {
    public:
        static const char* TAG;
        Pump(byte switchPin, byte ledPin, byte trigPin);

        [[nodiscard]] bool Setup() const;

        void SwitchOn() const;
        void SwitchOff() const;

    private:
        byte switchPin_;
        byte led_;
        byte trig_;
    };
} // CE::Drivers