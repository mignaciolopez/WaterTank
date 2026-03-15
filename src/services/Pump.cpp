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
#include "services/WSServer.h"

using namespace CE::OS;

namespace CE::Services
{
    const char* Pump::TAG = "Pump-Service";
    Domain::States::Pump Pump::_state{};
    PersistentState<Domain::States::Pump> Pump::persistent_(_state);
    Drivers::Relay* Pump::driver_ = nullptr;

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
        const bool watchdogResult = Watchdog::RegisterTask(TAG, Settings::Get().radarDelayS * 1000);

        return taskResult && watchdogResult;
    }

    void Pump::Switch(const bool on)
    {
        if (on) SwitchOn();
        else SwitchOff();
    }

    void Pump::SwitchOn()
    {
        if (_state.waterLevel == WaterLevel::Full || _state.waterLevel == WaterLevel::Invalid || _state.waterLevel == WaterLevel::Unknown)
        {
            ESP_LOGI(TAG, "Cannot turn pump on when water level is full or still reading");
            Watchdog::ReportError(Domain::ErrorSeverity::Info, Domain::ErrorType::WaterLevel, TAG, "Cannot turn pump on when water level is full or still reading");
            return;
        }

        if (!_state.isOn && !_state.isOnCooldown)
        {
            if (Time::Get() - _state.timeStampOff < 10)
            {
                ESP_LOGW(TAG, "Can not switch pump to fast!");
                return;
            }

            Settings::Get().radarDelayS = 1u;
            _state.timeStampOn = Time::Get();
            ESP_LOGI(TAG, "Pump turned ON at: %s", OS::Time::GetFormattedTime());
            driver_->SwitchOn();
            _state.isOn = true;
            persistent_.Save();
        }
    }

    void Pump::SwitchOff()
    {
        if (_state.isOn)
        {
            if (Time::Get() - _state.timeStampOn < 10)
            {
                ESP_LOGW(TAG, "Can not switch pump to fast!");
                return;
            }

            if (!Settings::Load()) Settings::Get().radarDelayS = 30u;
            _state.timeStampOff = Time::Get();
            ESP_LOGI(TAG, "Pump turned OFF at: %s", OS::Time::GetFormattedTime());
            driver_->SwitchOff();
            _state.isOn = false;
            persistent_.Save();
        }
    }

    void Pump::Task(void*)
    {
        ESP_LOGV(TAG, "Task");

        while (true)
        {
            Domain::RadarSample sample = {};
            if (Filter::TryGetLatestFilteredCm(sample))
            {
                sample.distanceCm /= 100.0f;
                ESP_LOGI(TAG, "Median Distance: %.1fcm.", sample.distanceCm);
                ESP_LOGI(TAG, "Water Level State: %s", WaterLevel::GetWaterLevelString(sample.distanceCm));
                _state.waterLevel = WaterLevel::GetWaterLevelState(static_cast<unsigned short>(sample.distanceCm));
                switch (_state.waterLevel)
                {
                    case WaterLevel::Invalid:
                        ESP_LOGE(TAG, "Invalid water level state");
                        Watchdog::ReportError(Domain::ErrorSeverity::Critical, Domain::ErrorType::WaterLevel, TAG, "Invalid water level state");
                        break;
                    case WaterLevel::Full:
                        SwitchOff();
                        break;
                    case WaterLevel::Normal:
                        if (Time::IsNightTime())
                        {
                            SwitchOff();
                        }
                        break;
                    case WaterLevel::Low:
                        if (!Time::IsNightTime())
                        {
                            SwitchOn();
                        }
                        break;
                    case WaterLevel::Critical:
                        SwitchOn();
                        break;
                    default:
                        ESP_LOGE(TAG, "Water State not handled %d", _state.waterLevel);
                        Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::WaterLevel, TAG, "Water State not handled");
                        break;
                }

                WSServer::Broadcast("distance", sample);
                WSServer::Broadcast("pumpStatus", Pump::GetState());
            }

            MonitorCooldown();
            MonitorTimeOn();
            Watchdog::NotifyTaskAlive(TAG);
            Time::SleepMs(Settings::Get().radarDelayS * 1000);
        }
    }

    void Pump::MonitorTimeOn()
    {
        if (_state.isOn)
        {
            const auto& s = Settings::Get();
            ESP_LOGI(TAG, "MonitorTimeOn: On at: %d should turn of in: %d seconds.", _state.timeStampOn, s.pumpMaxTimeOnM * 60 - (Time::Get() - _state.timeStampOn));
            if (Time::Get() - _state.timeStampOn > s.pumpMaxTimeOnM * 60)
            {
                SwitchOff();
                _state.isOnCooldown = true;
                _state.timeStampCooldown = Time::Get();
                Watchdog::ReportError(Domain::ErrorSeverity::Error, Domain::ErrorType::PumpTimeout, TAG, "Pump timeout");
                persistent_.Save();
            }
        }
    }

    void Pump::MonitorCooldown()
    {
        if (_state.isOnCooldown)
        {
            const auto& s = Settings::Get();
            ESP_LOGI(TAG, "MonitorCooldown at: %d should release pump in: %d seconds.", _state.timeStampCooldown, s.pumpCooldownTimeM * 60 - (Time::Get() - _state.timeStampCooldown));
            if (Time::Get() - _state.timeStampCooldown > s.pumpCooldownTimeM * 60)
            {
                _state.isOnCooldown = false;
                persistent_.Save();
            }
        }
    }
}
