#ifndef _DEFINES
#define _DEFINES

#define DEBUG 1

#include <Arduino.h>
#include <ArduinoJson.h>
#include <esp_log.h>
#include <NewPing.h>
#include <DHT.h>
#include <SPIFFS.h>

static const char *TAG = "";

struct Settings
{
    unsigned short height = 250;
    unsigned short delay = 1000;
    unsigned short median = 10;
    unsigned short dhtDelay = 10000;
    unsigned short reloadInterval = 3000;
} settings;

#define RX_ECHO_PIN 2
#define TX_TRIGGER_PIN 4
float SPEED_CM_US = 331.3 + (0.606 * 20);

#define DHT_PIN 7
#define DHT_TYPE DHT11
struct DHTData { float h, t, hic; };

#if CONFIG_IDF_TARGET_ESP32C3
    #define BLUE_LED 8
    #define BLUE_LED_ON LOW
    #define BLUE_LED_OFF HIGH
    #define SERIAL_SPEED 921600
#elif CONFIG_IDF_TARGET_ESP32
    #define BLUE_LED 2
    #define BLUE_LED_ON HIGH
    #define BLUE_LED_OFF LOW
    #define SERIAL_SPEED 115200
#else
    #define BLUE_LED 2
    #define BLUE_LED_ON HIGH
    #define BLUE_LED_OFF LOW
    #define SERIAL_SPEED 9600
#endif

QueueHandle_t rawDistanceQueue;
QueueHandle_t filteredDistanceQueue;
QueueHandle_t DHTQueue;

#endif //DEFINES