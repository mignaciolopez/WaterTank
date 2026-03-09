//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#include "config/BuildConfig.hpp"
#include "config/Pins.hpp"
#include <domain/Settings.hpp>
#include "os/Tasks.hpp"
#include "os/Time.hpp"
#include "services/Filter.h"
#include <services/Pump.h>
#include <os/Settings.h>
#include "services/Watchdog.h"
#include "services/WaterLevel.h"

using namespace CE::OS;

namespace CE::Services
{
    const char* Pump::TAG = "Pump-Service";
    Domain::States::Pump Pump::state_{};
    PersistentState<Domain::States::Pump> Pump::persistent_(state_);
    Drivers::Relay* Pump::driver_ = nullptr;
    bool Pump::isOn_ = false;

    Pump::Pump() = default;

    bool Pump::Setup()
    {
        ESP_LOGV(TAG, "Setup");

        if (!driver_)
            driver_ = new Drivers::Relay(Config::Pins::kPumpSwitchPin, Config::Pins::kPumpLedPin, Config::Pins::kPumpTrigPin);

        if (!driver_->Setup())
            return false;

        persistent_.Read();

        const bool taskResult = Tasks::Start(Task, TAG, Config::Build::kStackPumpTask, nullptr, Config::Build::kPrioPump, nullptr);
        const bool watchdogResult = Watchdog::RegisterTask(TAG, Settings::Get().radarDelay_s * 1000);

        return taskResult && watchdogResult;
    }

    void Pump::SwitchOn()
    {
        if (!isOn_ && !state_.isOnCooldown_)
        {
            Settings::Get().radarDelay_s = 1u;
            state_.timeStampOn_ = Time::Get();
            ESP_LOGI(TAG, "Pump turned ON at: %s", OS::Time::GetFormattedTime());
            driver_->SwitchOn();
            isOn_ = true;
            persistent_.Save();
        }
    }

    void Pump::SwitchOff()
    {
        if (isOn_)
        {
            if (!Settings::Load()) Settings::Get().radarDelay_s = 30u;
            ESP_LOGI(TAG, "Pump turned OFF at: %s", OS::Time::GetFormattedTime());
            driver_->SwitchOff();
            isOn_ = false;
            persistent_.Save();
        }
    }

    void Pump::Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        while (true)
        {
            unsigned short medianDistance = 0;
            if (Filter::TryGetLatestFilteredCm(medianDistance))
            {
                ESP_LOGI(TAG, "Median Distance: %.1fcm.", medianDistance / 100.0f);
                ESP_LOGI(TAG, "Water Level State: %s", WaterLevel::GetWaterLevelString(medianDistance / 100u));
                const auto status = WaterLevel::GetWaterLevelState(medianDistance / 100u);
                switch (status)
                {
                    case WaterLevel::Invalid:
                        ESP_LOGE(TAG, "Invalid water level state");
                        Watchdog::ReportError(Domain::ErrorSeverity::Critical, Domain::ErrorType::WaterLevel, TAG, "Invalid water level state");
                        break;
                    case WaterLevel::Critical:
                        SwitchOn();
                        break;
                    case WaterLevel::Low:
                        if (OS::Time::IsNightTime())
                        {
                            SwitchOff();
                        }
                        else
                        {
                            SwitchOn();
                        }
                        break;
                    case WaterLevel::Full:
                        SwitchOff();
                        break;
                    case WaterLevel::Normal:
                        //Do nothing
                        break;
                    default:
                        ESP_LOGE(TAG, "Water State not handled %d", status);
                        Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::WaterLevel, TAG, "Water State not handled");
                        break;
                }
            }

            MonitorCooldown();
            MonitorTimeOn();
            Watchdog::NotifyTaskAlive(TAG);
            Time::SleepMs(Settings::Get().radarDelay_s * 1000);
        }
    }

    void Pump::MonitorTimeOn()
    {
        if (isOn_)
        {
            const auto& s = Settings::Get();
            if (Time::Get() - state_.timeStampOn_ > s.pumpMaxTimeOn_m * 60)
            {
                SwitchOff();
                state_.isOnCooldown_ = true;
                state_.timeStampCooldown_ = Time::Get();
                Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::PumpTimeout, TAG, "Pump timeout");
                persistent_.Save();
            }
        }
    }

    void Pump::MonitorCooldown()
    {
        if (state_.isOnCooldown_)
        {
            const auto& s = Settings::Get();
            if (Time::Get() - state_.timeStampCooldown_ > s.pumpCooldownTime_m * 60)
            {
                state_.isOnCooldown_ = false;
                persistent_.Save();
            }
        }
    }
}
