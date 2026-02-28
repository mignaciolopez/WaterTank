//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <DHT.h>
#include <domain/WeatherModel.hpp>

namespace CE::Drivers
{
    class DhtDriver
    {
    public:
        DhtDriver(const int pin, const int type) : dht_(pin, type) {}

        void Begin() { dht_.begin(); }

        // Returns true when read is valid.
        bool Read(Domain::WeatherSample& out);

    private:
        DHT dht_;
    };

} // namespace CE::Drivers