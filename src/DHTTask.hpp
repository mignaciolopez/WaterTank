#include <Defines.h>

void DHTTask(void *pvParameters)
{
    DHT dht(DHT_PIN, DHT_TYPE);
    DHTData dhtData;
    dht.begin();

    vTaskDelay(pdMS_TO_TICKS(settings.dhtDelay));

    while (true)
    {
        dhtData.h = dht.readHumidity(true);
        dhtData.t = dht.readTemperature(false, true);
        dhtData.hic = dht.computeHeatIndex(dhtData.t, dhtData.h, false);

        if (!isnan(dhtData.h) && !isnan(dhtData.t) && dhtData.h > 0)
        {
            SPEED_CM_US = (331.3 + (0.606 * dhtData.hic)) / 10000.0;
            xQueueOverwrite(DHTQueue, &dhtData);
        }

        ESP_LOGD(TAG, "Humidity: %.2f%% Temperature: %.2f°C ", dhtData.h, dhtData.t);

        vTaskDelay(pdMS_TO_TICKS(settings.dhtDelay));
        ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
    }
}