//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class JSN_SR04M_2
    {
    public:
        static const char* TAG;
        explicit JSN_SR04M_2(byte triggerPin, byte echoPin);

        void Setup() const;
        bool ReadDistanceCm(unsigned short& out_cm) const;

    private:
        byte _triggerPin;
        byte _echoPin;
    };

}   // namespace CE::Drivers