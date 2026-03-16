//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class HC_SR04
    {
    public:
        static const char* TAG;
        explicit HC_SR04(byte triggerPin, byte echoPin);

        void Setup() const;
        bool ReadDistanceCm(unsigned short& out_cm) const;

    private:
        byte _triggerPin;
        byte _echoPin;
    };

}   // namespace CE::Drivers