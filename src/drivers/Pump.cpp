//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include <drivers/Pump.h>

namespace CE::Drivers
{
    const char* Pump::TAG = "PumpDriver";

    Pump::Pump(const byte switchPin, const byte ledPin, const byte trigPin)
        : switchPin_(switchPin), led_(ledPin), trig_(trigPin)
    {
    }

    bool Pump::Setup() const
    {
        pinMode(switchPin_, OUTPUT);
        pinMode(led_,       OUTPUT);
        pinMode(trig_,      INPUT_PULLUP);

        digitalWrite(switchPin_,   LOW);
        digitalWrite(led_,      LOW);

        return true;
    }

} // CE::Drivers