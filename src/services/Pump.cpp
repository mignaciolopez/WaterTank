//
// Created by lmart on 2/28/2026.
//

#include "config/BuildConfig.hpp"
#include "config/Pins.hpp"
#include "drivers/Pump.h"
#include "os/Tasks.hpp"
#include "os/Time.hpp"
#include "services/Filter.h"
#include <services/Pump.h>

namespace CE::Services::Pump
{
    const Drivers::Pump driver(Config::Pins::kPumpSwitchPin, Config::Pins::kPumpLedPin, Config::Pins::kPumpTrigPin);

    [[noreturn]] static void Task(void*)
    {
        ESP_LOGV(TAG, "Task");
        while (true)
        {
            unsigned short filtered = 0;
            if (Filter::TryGetLatestFilteredCm(filtered))
            {
                ESP_LOGI(TAG, "Distance: %.1fcm.", filtered / 100.0f);
                if (static_cast<float>(filtered) / 100.0f < 30.0f)
                {
                    driver.SwitchOff();
                }
                else if (static_cast<float>(filtered) / 100.0f > 100.0f)
                {
                    driver.SwitchOn();
                }
            }
            OS::Time::SleepMs(2000);
        }
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");

        if (!driver.Setup())
            return false;

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackPumpTask, nullptr, Config::Build::kPrioPump, nullptr);
    }
}
