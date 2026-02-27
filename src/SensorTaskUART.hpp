#include <Defines.h>

void SensorTaskUART(void *pvParameters)
{
    Serial1.begin(9600, SERIAL_8N1, RX_ECHO_PIN, TX_TRIGGER_PIN);
    uint8_t buffer[4];

    while (true)
    {
        // Request measurement (SR04M-2 UART command)
        Serial1.write(0x55);

        vTaskDelay(pdMS_TO_TICKS(100));

        // Make sure we have at least one full frame
        while (Serial1.available() >= 4)
        {
            // Sync to frame header 0xFF
            if (Serial1.read() == 0xFF)
            {
                buffer[0] = 0xFF;

                // Wait until remaining 3 bytes arrive
                while (Serial1.available() < 3)
                {
                    vTaskDelay(pdMS_TO_TICKS(1));
                }

                buffer[1] = Serial1.read();
                buffer[2] = Serial1.read();
                buffer[3] = Serial1.read();

                // Validate checksum
                uint8_t checksum = (buffer[0] + buffer[1] + buffer[2]) & 0xFF;

                if (checksum == buffer[3])
                {
                    uint16_t distance = (buffer[1] << 8) | buffer[2];

                    xQueueSend(rawDistanceQueue, &distance, 0);
                }
            }
        }

        vTaskDelay(pdMS_TO_TICKS(10));
    }
}