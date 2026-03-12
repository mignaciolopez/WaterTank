//
// Created by lmartinignacio@gmail.com on 2/28/2026.
//

#pragma once

namespace CE::Services::WaterLevel
{
    static auto TAG = "WaterLevelService";

    enum WaterLevel
    {
        Invalid = -1,
        Unknown = 0,
        Full,
        Normal,
        Low,
        Critical
    };

    WaterLevel GetWaterLevelState(unsigned short distance);
    const char* GetWaterLevelString(unsigned short distance);

    const char* StatusToString(WaterLevel status);

}   // namespace CE::Services::WaterLevel