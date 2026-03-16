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
        Relay(byte switchPin);

        [[nodiscard]] bool Setup() const;

        void SwitchOn();
        void SwitchOff();
        [[nodiscard]] bool IsOn() const;

    private:
        byte switchPin_;
        bool state;
    };
} // CE::Drivers