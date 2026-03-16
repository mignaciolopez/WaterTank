//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
//#include <drivers/JSN-SR04M-2.h>
#include <drivers/HC-SR04.h>

namespace CE::Services
{
    class Radar
    {
    public:
        static const char* TAG;
        static QueueHandle_t gRadarQueue;

        Radar();
        [[nodiscard]] static bool Setup();

        static bool TryGetLatestRawCm(unsigned short& out_cm);

    private:
        [[noreturn]] static void Task(void* pvParameters);

        static Drivers::HC_SR04* driver_;
    };

}   // namespace CE::Services