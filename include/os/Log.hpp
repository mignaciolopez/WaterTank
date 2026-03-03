//
// Created by lmartinignacio@gmail.com on 2/27/2026.
//

#pragma once
#include <esp_log.h>

namespace CE::OS::Log
{
    static void set(const char* TAG, esp_log_level_t level)
    {
        esp_log_level_set(TAG, level);
    }
}   // namespace CE::OS::Log