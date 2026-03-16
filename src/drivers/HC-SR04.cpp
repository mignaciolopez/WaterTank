//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include "domain/Global.hpp"
#include <drivers/HC-SR04.h>
#include <os/Settings.h>

#include "os/Log.hpp"

namespace CE::Drivers
{
    const char* HC_SR04::TAG = "HC_SR04-Driver";

    HC_SR04::HC_SR04(const byte triggerPin, const byte echoPin)
      : _triggerPin(triggerPin), _echoPin(echoPin)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    void HC_SR04::Setup() const
    {
        ESP_LOGV(TAG, "Setup");

        pinMode(_triggerPin, OUTPUT);
        pinMode(_echoPin, INPUT);
        digitalWrite(_triggerPin, HIGH);
        delay(1000);
    }

    bool HC_SR04::ReadDistanceCm(unsigned short& out_cm) const
    {
        ESP_LOGV(TAG, "ReadDistanceCm");

        // Timeout prevents blocking forever
        const auto timeout = static_cast<unsigned long>(static_cast<float>(OS::Settings::Get().heightCm) * 2.0f / Domain::g_speed_cm_per_us);
        ESP_LOGD(TAG, "timeout_us=%u", timeout);

        unsigned long duration_us = 0;

        { // Do not add code while waiting for echo on pulseIn function call
            // Trigger pulse
            digitalWrite(_triggerPin, LOW);
            delayMicroseconds(2);
            digitalWrite(_triggerPin, HIGH);
            delayMicroseconds(10);
            digitalWrite(_triggerPin, LOW);
            duration_us = pulseIn(_echoPin, HIGH, timeout);
        }

        if (duration_us == 0)
        {
            return false;
        }

        out_cm = static_cast<unsigned short>(static_cast<float>(duration_us) * Domain::g_speed_cm_per_us / 2.0f * 100u);
        return true;
    }

}   // namespace CE::Drivers