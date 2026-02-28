//
// Created by lmart on 2/27/2026.
//

#pragma once
#include <cstdint>

namespace CE::Drivers
{
    class Ultrasonic
    {
    public:
        Ultrasonic(int trigPin, int echoPin, uint32_t timeoutUs);

        void Begin() const;
        bool ReadDistanceCm(float speed_cm_per_us, float& out_cm);

    private:
        int trig_;
        int echo_;
        uint32_t timeoutUs_;
    };

} // namespace CE::Drivers