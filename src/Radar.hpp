#pragma once
#include <Arduino.h>
#include <Settings.hpp>
#include <Globals.h>

namespace CE
{
    namespace Radar
    {
        static const char *TAG = "Radar";

        #define RX_ECHO_PIN 2
        #define TX_TRIGGER_PIN 4

        QueueHandle_t rawDistanceQueue;

        [[noreturn]] inline void Task(void *pvParameters)
        {
            ESP_LOGV(TAG, "Task");

            while (true)
            {
                digitalWrite(TX_TRIGGER_PIN, LOW);
                esp_rom_delay_us(10);
                digitalWrite(TX_TRIGGER_PIN, HIGH);
                esp_rom_delay_us(250);
                digitalWrite(TX_TRIGGER_PIN, LOW);

                float distance_cm = static_cast<float>(pulseIn(RX_ECHO_PIN, HIGH)) * Globals::SPEED_CM_US / 2.0f;
                ESP_LOGD(TAG, "distance_cm: %ucm.", distance_cm);
                xQueueOverwrite(rawDistanceQueue, &distance_cm);

                vTaskDelay(pdMS_TO_TICKS(CE::Settings::data.radarDelay));
                ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
            }
        }

        inline bool Init()
        {
            ESP_LOGV(TAG, "Init");

            pinMode(TX_TRIGGER_PIN, OUTPUT);
            pinMode(RX_ECHO_PIN, INPUT);
            digitalWrite(TX_TRIGGER_PIN, HIGH);
            delay(1000);
            rawDistanceQueue = xQueueCreate(1, sizeof(float));
            xTaskCreate(Task, "RadarTask",4096,nullptr, 2, nullptr); //384~568
            return true;
        }
    }
}