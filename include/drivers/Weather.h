//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <DHT.h>
#include <domain/Weather.hpp>

namespace CE::Drivers
{
    class Weather
    {
    public:
        static const char* TAG;
        Weather(const int pin, const int type) : dht_(pin, type) {}

        void Setup()
        {
            ESP_LOGV(TAG, "Setup");
            dht_.begin();
        }

        // Returns true when read is valid.
        bool Read(Domain::WeatherSample& out);

    private:
        DHT dht_;
    };

} // namespace CE::Drivers