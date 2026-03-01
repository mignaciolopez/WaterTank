//
// Created by lmart on 2/28/2026.
//

#pragma once

#include "config/Pins.hpp"
#include "drivers/Pump.h"

namespace CE::Services::Pump
{
    static auto TAG = "PumpService";

    static Drivers::Pump driver(Config::Pins::kPumpSwitchPin, Config::Pins::kPumpLedPin, Config::Pins::kPumpTrigPin);

    [[nodiscard]] bool Setup();

} // namespace CE::Services::Filter