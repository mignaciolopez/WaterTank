//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <drivers/Radar.h>
#include <Services/Settings.h>
#include "domain/Global.hpp"

namespace CE::Drivers
{
    const char* Radar::TAG = "RadarDriver";

    Radar::Radar(byte trigPin, byte echoPin)
      : trig_(trigPin), echo_(echoPin)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    void Radar::Setup() const
    {
        ESP_LOGV(TAG, "Setup");

        pinMode(trig_, OUTPUT);
        pinMode(echo_, INPUT);
        digitalWrite(trig_, HIGH);
    }

    bool Radar::ReadDistanceCm(unsigned short& out_cm) const
    {
        ESP_LOGV(TAG, "ReadDistanceCm");

        // Timeout prevents blocking forever
        const auto timeout = static_cast<unsigned long>(static_cast<float>(Services::Settings::Get().height_cm)  * 2 / Domain::g_speed_cm_per_us);
        ESP_LOGD(TAG, "timeout_us=%u", timeout);

        unsigned long duration_us = 0;

        { // Do not add code while waiting for echo on pulseIn function call
            // Trigger pulse
            digitalWrite(trig_, LOW);
            delayMicroseconds(10);
            digitalWrite(trig_, HIGH);
            delayMicroseconds(250);
            digitalWrite(trig_, LOW);
            duration_us = pulseIn(echo_, HIGH, timeout);
        }

        if (duration_us == 0)
        {
            return false;
        }

        out_cm = static_cast<unsigned short>(static_cast<float>(duration_us) * Domain::g_speed_cm_per_us / 2.0f * 100u);
        return true;
    }

} // namespace CE::Drivers