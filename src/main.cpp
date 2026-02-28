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
    digitalWrite(Config::Pins::kBlueLed, Config::Build::kBlueLedOn);

    Serial.begin(921600);
    esp_log_level_set("*", ESP_LOG_INFO);

    delay(1000);
    ESP_LOGI(TAG, "Boot");
    delay(2000);

    while (!App::Setup())
    {
        for (int i = 0; i < 30; ++i)
        {
            digitalWrite(Config::Pins::kBlueLed, Config::Build::kBlueLedOff);
            delay(500);
            digitalWrite(Config::Pins::kBlueLed, Config::Build::kBlueLedOn);
            delay(500);
        }
        esp_restart();
    }

    digitalWrite(Config::Pins::kBlueLed, Config::Build::kBlueLedOff);
}

void loop()
{
    App::Loop();
    delay(1000); // keep the main loop light; tasks do the real work
}