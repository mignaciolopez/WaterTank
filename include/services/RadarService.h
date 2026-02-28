//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

namespace CE::Services::Radar
{
    bool Init();
    bool TryGetLatestRawCm(float& out_cm);

} // namespace CE::Services::Radar