//
// Created by lmart on 2/28/2026.
//

#include "config/BuildConfig.hpp"
#include "os/Tasks.hpp"
#include "os/Time.hpp"
#include "services/Filter.h"
#include <services/Pump.h>
#include "services/WaterLevel.h"

namespace CE::Services::Pump
{
    [[noreturn]] static void Task(void*)
    {
        ESP_LOGV(TAG, "Task");
        while (true)
        {
            unsigned short filteredDistance = 0;
            if (Filter::TryGetLatestFilteredCm(filteredDistance))
            {
                ESP_LOGI(TAG, "Median Distance: %.1fcm.", filteredDistance / 100.0f);
                ESP_LOGI(TAG, "Water Level State: %s", WaterLevel::GetWaterLevelString(filteredDistance / 100u));
                const auto status = WaterLevel::GetWaterLevelState(filteredDistance / 100u);
                switch (status)
                {
                    case WaterLevel::Invalid:
                        ESP_LOGE(TAG, "Invalid water level state");
                        break;
                    case WaterLevel::Critical:
                        driver.SwitchOn();
                        break;
                    case WaterLevel::Low:
                        if (OS::Time::IsSleepTime())
                            driver.SwitchOff();
                        else
                            driver.SwitchOn();
                        break;
                    case WaterLevel::Full:
                        driver.SwitchOff();
                        break;
                    case WaterLevel::Normal:
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
}
