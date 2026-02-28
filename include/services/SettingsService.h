//
// Created by lmart on 2/27/2026.
//

#pragma once
#include <domain/SettingsModel.hpp>

namespace CE::Services::Settings
{
    bool Init();
    const Domain::Settings& Get(); // always returns current settings snapshot

} // namespace CE::Services::Settings