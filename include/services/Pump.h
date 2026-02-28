//
// Created by lmart on 2/28/2026.
//

#pragma once

namespace CE::Services::Pump
{
    static auto TAG = "PumpService";

    enum WaterLevel
    {
        Invalid,
        Full,
        Normal,
        Low,
        Critical
    };
    static auto status = WaterLevel::Normal;

    [[nodiscard]] bool Setup();

    void UpdateState(unsigned short distance);
    const char* StatusToString();

} // namespace CE::Services::Filter