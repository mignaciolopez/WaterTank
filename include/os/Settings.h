//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <domain/Settings.hpp>

namespace CE::OS
{
    class Settings
    {
    public:
        static const char* TAG;

        Settings();
        [[nodiscard]] static bool Setup();

        [[nodiscard]] static Domain::Settings& Get();

        [[nodiscard]] static bool Load();
        [[nodiscard]] static bool Save();

        static void Print();

        static Domain::Settings* settings;
    };

}   // namespace CE::Services