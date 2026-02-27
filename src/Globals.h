#pragma once

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

#define DEBUG 1
namespace CE
{
    namespace Globals
    {
        float SPEED_CM_US = 331.3f + (0.606f * 20.0f);
        bool INIT = false;
    }
}





