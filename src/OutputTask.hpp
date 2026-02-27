#include <Defines.h>

void OutputTask(void *pvParameters)
{
    uint16_t distance;
    DHTData dhtData;

    while (true)
    {
        ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
        if (xQueueReceive(filteredDistanceQueue, &distance, settings.delay/*portMAX_DELAY*/) == pdTRUE)
        {
            ESP_LOGI(TAG, "Distance: %dcm.", distance);
        }

        if (xQueueReceive(DHTQueue, &dhtData, settings.delay) == pdTRUE)
        {
            ESP_LOGI(TAG, "Humidity: %.2f%% Temperature: %.2f°C Heat Index: %.2f°C", dhtData.h, dhtData.t, dhtData.hic);
        }
        vTaskDelay(pdMS_TO_TICKS(settings.delay));
    }
}