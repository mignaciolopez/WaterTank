//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once

namespace CE::Services::Filter
{
    static auto TAG = "FilterService";

    [[nodiscard]] bool Setup();
    bool TryGetLatestFilteredCm(unsigned short& out_cm);

} // namespace CE::Services::Filter