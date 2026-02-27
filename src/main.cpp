#include <Defines.h>
#include <DHTTask.hpp>
#include <FilterTask.hpp>
#include <OutputTask.hpp>
#include <PingTask.hpp>
#include <SensorTask.hpp>
#include <SensorTaskUART.hpp>
#include <Settings.hpp>

void setupPinModes()
{
  pinMode(BLUE_LED, OUTPUT);
  pinMode(RX_ECHO_PIN, INPUT);
  pinMode(TX_TRIGGER_PIN, OUTPUT);
}

void setupPinStates()
{
  digitalWrite(BLUE_LED, BLUE_LED_ON);
  digitalWrite(TX_TRIGGER_PIN, HIGH);
}

void setupQueues()
{
  rawDistanceQueue = xQueueCreate(1, sizeof(uint16_t));
  filteredDistanceQueue = xQueueCreate(1, sizeof(uint16_t));
  DHTQueue = xQueueCreate(1, sizeof(DHTData));
}

bool setupSettings()
{
  if (!SPIFFS.begin(true))
  {
      ESP_LOGE(TAG, "SPIFFS failed");
      return false;
  }

  return true;
}

void setupTasks()
{
  xTaskCreate(SettingsTask, "SettingsTask", 4096, NULL, 3, NULL); //1236~1220
  xTaskCreate(DHTTask,      "DHTTask",      4096, NULL, 2, NULL); //832~712
  xTaskCreate(SensorTask,   "SensorTask",   4096, NULL, 2, NULL); //384~568
  xTaskCreate(FilterTask,   "FilterTask",   4096,  NULL, 1, NULL); //320~172
  xTaskCreate(OutputTask,   "OutputTask",   4096,  NULL, 1, NULL); //288
}

void setupEnd()
{
  delay(1000);
  digitalWrite(TX_TRIGGER_PIN, LOW);
  digitalWrite(BLUE_LED, BLUE_LED_OFF);
}

void setup()
{
  setupPinModes();
  setupPinStates();

  Serial.begin(SERIAL_SPEED);
  esp_log_level_set("*", ESP_LOG_DEBUG);

  if (!setupSettings())
    return;

  setupQueues();
  setupTasks();  
  setupEnd();
}

void loop()
{
  //ESP_LOGV("MEM", "Free heap: %u bytes.", ESP.getFreeHeap());
}