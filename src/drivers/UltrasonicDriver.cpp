//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <Arduino.h>
#include <drivers/UltrasonicDriver.h>

namespace CE::Drivers
{
    Ultrasonic::Ultrasonic(int trigPin, int echoPin, uint32_t timeoutUs)
      : trig_(trigPin), echo_(echoPin), timeoutUs_(timeoutUs) {}

    void Ultrasonic::Begin() const
    {
        pinMode(trig_, OUTPUT);
        pinMode(echo_, INPUT);
        digitalWrite(trig_, HIGH);
    }

    bool Ultrasonic::ReadDistanceCm(const float speed_cm_per_us, float& out_cm)
    {
        // Trigger pulse
        digitalWrite(trig_, LOW);
        delayMicroseconds(10);
        digitalWrite(trig_, HIGH);
        delayMicroseconds(250);
        digitalWrite(trig_, LOW);

        // Timeout prevents blocking forever
        const float duration_us = static_cast<float>(pulseIn(echo_, HIGH, timeoutUs_));
        if (duration_us == .0f)
        {
            return false;
        }

        out_cm = duration_us * speed_cm_per_us / 2.0f / 10000.0f;
        return true;
    }

} // namespace CE::Drivers