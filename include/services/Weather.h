//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>
#include <Domain/Samples.hpp>
#include <drivers/DHTDevice.h>

namespace CE::Services
{
    class Weather
    {
    public:
        static const char* TAG;
        static QueueHandle_t gWeatherQueue;

        Weather();
        [[nodiscard]] static bool Setup();

        static bool TryGetLatest(Domain::WeatherSample& out);
        static bool ReadLast(Domain::WeatherSample& out);

    private:
        [[noreturn]] static void Task(void* pvParameters);

        static Drivers::DHTDevice* driver_;
    };
}   // namespace CE::Services