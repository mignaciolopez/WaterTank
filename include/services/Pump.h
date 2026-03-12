//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once

#include "WaterLevel.h"
#include "domain/States.hpp"
#include "drivers/Relay.h"
#include "os/PersistentState.hpp"

namespace CE::Services
{
    class Pump
    {
    public:
        static const char* TAG;

        Pump();
        [[nodiscard]] static bool Setup();

        static void Switch(bool on);
        static const Domain::States::Pump& GetState() { return _state; }

    private:
        [[noreturn]] static void Task(void* pvParameters);

        static void SwitchOn();
        static void SwitchOff();

        static void MonitorTimeOn();
        static void MonitorCooldown();

        static Domain::States::Pump _state;
        static OS::PersistentState<Domain::States::Pump> persistent_;

        static Drivers::Relay* driver_;
    };

}   // namespace CE::Services