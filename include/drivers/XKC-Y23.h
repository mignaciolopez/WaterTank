//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <Arduino.h>

namespace CE::Drivers
{
    class XKC_Y23
    {
    public:
        static const char* TAG;

        explicit XKC_Y23(byte pullupPin);

        void Setup() const;
        [[nodiscard]] bool IsDetectingWater() const;

    private:
        byte pullupPin_;
    };

}   // namespace CE::Drivers