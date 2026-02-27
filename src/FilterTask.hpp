#include <Defines.h>

static uint16_t median(uint16_t *arr)
{
    uint16_t temp[settings.median];

    for (int i = 0; i < settings.median; i++)
        temp[i] = arr[i];

    // Simple bubble sort (small array, very fast)
    for (int i = 0; i < settings.median - 1; i++)
    {
        for (int j = i + 1; j < settings.median; j++)
        {
            if (temp[j] < temp[i])
            {
                uint16_t t = temp[i];
                temp[i] = temp[j];
                temp[j] = t;
            }
        }
    }

    return temp[2]; // middle value
}

void FilterTask(void *pvParameters)
{
    uint16_t window[settings.median] = {0};
    uint16_t distance;
    int index = 0;
    bool filled = false;

    while (true)
    {
        ESP_LOGV(TAG, "Free stack: %u bytes.", uxTaskGetStackHighWaterMark(NULL));
        if (xQueueReceive(rawDistanceQueue, &distance, portMAX_DELAY) == pdTRUE)
        {
            if (distance > 0 && distance < settings.height)
            {
                window[index] = distance;
                index = (index + 1) % settings.median;

                if (index == 0)
                    filled = true;

                if (filled)
                {
                    uint16_t filtered = median(window);

                    xQueueOverwrite(filteredDistanceQueue, &filtered);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(settings.delay));
    }
}