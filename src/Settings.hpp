#include <Defines.h>

void loadSettings()
{
    File file = SPIFFS.open("/settings.json", "r");
    if (!file)
    {
        ESP_LOGE(TAG, "Failed to open settings file");      
        return;
    }

    StaticJsonDocument<16U> doc;
    DeserializationError error = deserializeJson(doc, file);

    if (error)
    {
        ESP_LOGE(TAG, "JSON Error: %s", error.c_str());
        return;
    }

    settings.height         = doc["height"]         | 250;
    settings.delay          = doc["delay"]          | 1000;
    settings.median         = doc["median"]         | 10;
    settings.dhtDelay       = doc["dhtDelay"]       | 10;
    settings.reloadInterval = doc["reloadInterval"] | 3000;

    file.close();
    ESP_LOGD(TAG, "Settings reloaded");
}

void SettingsTask(void* pvParameters)
{
    while(true)
    {
        loadSettings();
        vTaskDelay(pdMS_TO_TICKS(settings.reloadInterval));
        ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
    }
}