//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <cmath>
#include <drivers/DhtDriver.h>

namespace CE::Drivers
{
    bool DhtDriver::Read(Domain::WeatherSample& out)
    {
        const float h = dht_.readHumidity(true);
        const float t = dht_.readTemperature(false, true);
        const float hic = dht_.computeHeatIndex(t, h, false);

        if (std::isnan(h) || std::isnan(t) || h <= 0)
        {
            return false;
        }

        out.humidity = h;
        out.tempC = t;
        out.heatIndexC = hic;

        return true;
    }

} // namespace CE::Drivers