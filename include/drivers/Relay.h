//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class Relay
    {
    public:
        static const char* TAG;
        Relay(byte switchPin, byte ledPin, byte trigPin);

        [[nodiscard]] bool Setup() const;

        void SwitchOn();
        void SwitchOff();
        [[nodiscard]] bool IsOn() const;

    private:
        byte switchPin_;
        byte led_;
        byte trig_;
        bool state;
    };
} // CE::Drivers