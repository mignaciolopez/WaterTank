#include <Filters.hpp>
#include <Globals.h>
#include <Radar.hpp>
#include <Settings.hpp>
#include <Weather.hpp>

void setupStart()
{
    CE::Globals::INIT = false;
    pinMode(BLUE_LED, OUTPUT);
    digitalWrite(BLUE_LED, BLUE_LED_ON);
    Serial.begin(SERIAL_SPEED);
    esp_log_level_set("*", ESP_LOG_INFO);
    delay(1000);
    ESP_LOGV("Setup", "Starting");
}

void setupEnd()
{
    CE::Globals::INIT = true;
    digitalWrite(TX_TRIGGER_PIN, LOW);
    digitalWrite(BLUE_LED, BLUE_LED_OFF);
    ESP_LOGV("Setup", "End");
}

void setup()
{
    setupStart();
    if (!CE::Settings::Init())
    {
        return;
    }
    CE::Weather::Init();
    CE::Radar::Init();
    CE::Filters::Init();
    setupEnd();
}

void loop()
{
    if (!CE::Globals::INIT)
    {
        digitalWrite(BLUE_LED, BLUE_LED_OFF);
        delay(500);
        digitalWrite(BLUE_LED, BLUE_LED_ON);
        ESP_LOGV("MEM", "Free heap: %u bytes.", ESP.getFreeHeap());
    }
}