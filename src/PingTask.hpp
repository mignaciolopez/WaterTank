#include <Defines.h>

NewPing sonar(TX_TRIGGER_PIN, RX_ECHO_PIN, settings.height);

void PingTask(void* pvParameters)
{
  while (true)
  {
    //unsigned int uS = sonar.ping_median(50);
    //unsigned int distance_cm = uS / US_ROUNDTRIP_CM;
    //Serial.println(String(distance_cm) + " cm");

    unsigned int distance_cm = sonar.ping_cm(settings.height);
    Serial.println(distance_cm);

    vTaskDelay(pdMS_TO_TICKS(settings.delay));
  }
}