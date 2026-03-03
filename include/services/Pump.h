//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once

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

        static void SwitchOn();
        static void SwitchOff();

    private:
        [[noreturn]] static void Task(void* pvParameters);

        static void MonitorTimeOn();
        static void MonitorCooldown();

        static Domain::States::Pump state_;
        static OS::PersistentState<Domain::States::Pump> persistent_;

        static Drivers::Relay* driver_;
        static bool isOn_;

    };

}   // namespace CE::Services