#include <Defines.h>

void SensorTask(void *pvParameters)
{
    uint8_t buffer[4];

    while (true)
    {
        digitalWrite(BLUE_LED, BLUE_LED_OFF);
        digitalWrite(TX_TRIGGER_PIN, LOW);
        esp_rom_delay_us(10);
        digitalWrite(BLUE_LED, BLUE_LED_ON);
        digitalWrite(TX_TRIGGER_PIN, HIGH);
        esp_rom_delay_us(250);
        digitalWrite(BLUE_LED, BLUE_LED_OFF);
        digitalWrite(TX_TRIGGER_PIN, LOW);
        
        unsigned long distance_cm = (pulseIn(RX_ECHO_PIN, HIGH) * SPEED_CM_US) / 2.0;
        ESP_LOGD(TAG, "distance_cm: %ucm.", distance_cm);
        xQueueOverwrite(rawDistanceQueue, &distance_cm);

        vTaskDelay(pdMS_TO_TICKS(settings.delay));
        ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
    }
}