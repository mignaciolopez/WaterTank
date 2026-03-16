//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include <drivers/Relay.h>

namespace CE::Drivers
{
    const char* Relay::TAG = "Pump-Driver";

    Relay::Relay(const byte switchPin)
        : switchPin_(switchPin), state(false)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    bool Relay::Setup() const
    {
        ESP_LOGV(TAG, "Setup");

        pinMode(switchPin_, OUTPUT);

        digitalWrite(switchPin_,   LOW);

        return true;
    }

    void Relay::SwitchOn()
    {
        ESP_LOGV(TAG, "SwitchOn");
        digitalWrite(switchPin_, HIGH);
        state = true;
    }

    void Relay::SwitchOff()
    {
        ESP_LOGV(TAG, "SwitchOff");
        digitalWrite(switchPin_, LOW);
        state = false;
    }

    bool Relay::IsOn() const
    {
        return state;
    }
} // CE::Drivers