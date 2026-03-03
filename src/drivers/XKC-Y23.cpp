//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#include <drivers/XKC-Y23.h>

namespace CE::Drivers
{
    const char* XKC_Y23::TAG = "XKC_Y23-Driver";

    XKC_Y23::XKC_Y23(byte pullupPin) : pullupPin_(pullupPin)
    {
        ESP_LOGV(TAG, "Constructor");
    }

    void XKC_Y23::Setup() const
    {
        ESP_LOGV(TAG, "Setup");

        pinMode(pullupPin_, INPUT_PULLUP);
    }

    bool XKC_Y23::IsDetectingWater() const
    {
        ESP_LOGV(TAG, "IsDetectingWater");
        return digitalRead(pullupPin_) == LOW;
    }

}   // namespace CE::Drivers