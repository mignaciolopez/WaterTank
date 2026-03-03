//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <cmath>
#include <drivers/DHTDevice.h>

namespace CE::Drivers
{
    const char* DHTDevice::TAG = "DHTDevice-Driver";

    DHTDevice::DHTDevice(const byte pin, const byte type) : dht_(pin, type), pin_(pin), type_(type)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    bool DHTDevice::Setup()
    {
        ESP_LOGV(TAG, "Setup");
        pinMode(pin_, INPUT);
        dht_.begin();

        return true;
    }

    bool DHTDevice::Read(Domain::WeatherSample& out)
    {
        ESP_LOGV(TAG, "Reading DHT%d on PIN %d", type_, pin_);

        out.humidity = dht_.readHumidity(true);
        out.tempC = dht_.readTemperature(false, true);
        out.heatIndexC = dht_.computeHeatIndex(out.tempC, out.humidity, false);

        ESP_LOGD(TAG, "h=%.2f t=%.2f hic=%.2f", out.humidity, out.tempC, out.heatIndexC);

        if (std::isnan(out.humidity) || out.humidity < 0 || std::isnan(out.tempC))
        {
            return false;
        }

        return true;
    }

}   // namespace CE::Drivers