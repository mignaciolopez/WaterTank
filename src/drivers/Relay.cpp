//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include <drivers/Relay.h>

namespace CE::Drivers
{
    const char* Relay::TAG = "Pump-Driver";

    Relay::Relay(const byte switchPin, const byte ledPin, const byte trigPin)
        : switchPin_(switchPin), led_(ledPin), trig_(trigPin), state(false)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    bool Relay::Setup() const
    {
        ESP_LOGV(TAG, "Setup");

        pinMode(switchPin_, OUTPUT);
        pinMode(led_,       OUTPUT);
        pinMode(trig_,      INPUT_PULLUP);

        digitalWrite(switchPin_,   LOW);
        digitalWrite(led_,      LOW);

        return true;
    }

    void Relay::SwitchOn()
    {
        ESP_LOGV(TAG, "SwitchOn");
        digitalWrite(led_, HIGH);
        digitalWrite(switchPin_, HIGH);
        state = true;
    }

    void Relay::SwitchOff()
    {
        ESP_LOGV(TAG, "SwitchOff");
        digitalWrite(switchPin_, LOW);
        digitalWrite(led_, LOW);
        state = false;
    }

    bool Relay::IsOn() const
    {
        return state;
    }
} // CE::Drivers