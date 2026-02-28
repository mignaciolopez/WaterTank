//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class Radar
    {
    public:
        static const char* TAG;
        Radar(byte trigPin, byte echoPin, unsigned long timeoutUs);

        void Setup() const;
        bool ReadDistanceCm(float speed_cm_per_us, float& out_cm);

    private:
        byte trig_;
        byte echo_;
        unsigned long timeoutUs_;
    };

} // namespace CE::Drivers