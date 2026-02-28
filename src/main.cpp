#include <Arduino.h>
#include <app/App.hpp>
#include "config/BuildConfig.hpp"
#include <config/Pins.hpp>
#include <esp_log.h>

using namespace CE;

static const char* TAG = "Main";

bool initState = false;

void setup()
{
    pinMode(Config::Pins::kBlueLed, OUTPUT);
    digitalWrite(Config::Pins::kBlueLed, Config::Build::kLedOn);

    Serial.begin(921600);
    esp_log_level_set("*", ESP_LOG_VERBOSE);

    delay(500);
    ESP_LOGI(TAG, "Boot");

    while (!App::Setup())
    {
        for (int i = 0; i < 30; ++i)
        {
            digitalWrite(Config::Pins::kBlueLed, Config::Build::kLedOff);
            delay(500);
            digitalWrite(Config::Pins::kBlueLed, Config::Build::kLedOn);
            delay(500);
        }
        esp_restart();
    }

    digitalWrite(Config::Pins::kBlueLed, Config::Build::kLedOff);
}

void loop()
{
    //App::Loop();
}