//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <domain/Settings.hpp>

namespace CE::Services::Settings
{
    static auto TAG = "SettingsService";

    [[nodiscard]] bool Setup();
    const Domain::Settings& Get(); // always returns current settings snapshot

} // namespace CE::Services::Settings