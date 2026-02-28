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
#include <Services/Settings.h>

namespace CE::Services::Pump
{
    Drivers::Pump driver(Config::Pins::kPumpSwitchPin, Config::Pins::kPumpLedPin, Config::Pins::kPumpTrigPin);

    [[noreturn]] static void Task(void*)
    {
        ESP_LOGV(TAG, "Task");
        while (true)
        {
            unsigned short filteredDistance = 0;
            if (Filter::TryGetLatestFilteredCm(filteredDistance))
            {
                ESP_LOGI(TAG, "Distance: %.1fcm.", filteredDistance / 100.0f);
                UpdateState(filteredDistance / 100u);
                ESP_LOGI(TAG, "Water Level State: %s", StatusToString());
                switch (status)
                {
                    case Invalid:
                        ESP_LOGE(TAG, "Invalid water level state");
                        break;
                    case Critical:
                        driver.SwitchOn();
                        break;
                    case Low:
                        if (OS::Time::IsSleepTime())
                            driver.SwitchOff();
                        else
                            driver.SwitchOn();
                        break;
                    case Full:
                        driver.SwitchOff();
                        break;
                    case Normal:
                        //Do nothing
                        break;
                    default:
                        ESP_LOGE(TAG, "Water State not handled %d", status);
                        break;
                }
            }
            OS::Time::SleepMs(1000);
        }
    }

    bool Setup()
    {
        ESP_LOGV(TAG, "Setup");

        if (!driver.Setup())
            return false;

        return OS::Tasks::Start(Task, TAG, Config::Build::kStackPumpTask, nullptr, Config::Build::kPrioPump, nullptr);
    }

    void UpdateState(const unsigned short distance)
    {
        const auto s = Settings::Get();
        if (distance > s.height_cm)
        {
            ESP_LOGE(TAG, "Distance %ucm is higher than configured water tank height: %ucm", distance, s.height_cm);
            OS::Time::SleepMs(10000);
            status = Invalid;
        }

        if (distance <= s.height_cm - s.maxLevel_cm)
        {
            status = Full;
        }
        if (distance > s.height_cm - s.maxLevel_cm && distance <= s.height_cm - s.minLevel_cm)
        {
            status = Normal;
        }
        if (distance > s.height_cm - s.minLevel_cm && distance <= s.height_cm - s.criticalLevel_cm)
        {
            status = Low;
        }
        if (distance > s.height_cm - s.criticalLevel_cm)
        {
            status = Critical;
        }
    }

    const char* StatusToString()
    {
        switch(status)
        {
            case Invalid: return "Invalid";
            case Full: return "Full";
            case Normal: return "Normal";
            case Low: return "Low";
            case Critical: return "Critical";
            default: return "Unknown";
        }
    }
}
