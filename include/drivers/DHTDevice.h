//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <DHT.h>
#include <Domain/Samples.hpp>

namespace CE::Drivers
{
    class DHTDevice
    {
    public:
        static const char* TAG;
        DHTDevice(byte pin, byte type);

        bool Setup();

        // Returns true when read is valid.
        bool Read(Domain::WeatherSample& out);

    private:
        DHT dht_;
        byte pin_;
        byte type_;
    };

}   // namespace CE::Drivers