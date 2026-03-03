//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

#include "os/Queues.hpp"

namespace CE::Services
{
    class Filter
    {
    public:
        static const char* TAG;
        static QueueHandle_t gMedianDistanceQueue;

        Filter();
        [[nodiscard]] static bool Setup();

        static bool TryGetLatestFilteredCm(unsigned short& out_cm);

    private:
        [[noreturn]] static void Task(void* pvParameters);
        static unsigned short MedianInPlace(unsigned short* buf, std::size_t n);
    };

}   // namespace CE::Services