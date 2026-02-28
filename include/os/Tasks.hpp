//
// Created by lmartinignacio@outlook.com on 2/27/2026.
//

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace CE::OS::Tasks
{

    inline bool Start(TaskFunction_t fn,
                      const char* name,
                      uint32_t stackBytes,
                      void* param,
                      UBaseType_t prio,
                      TaskHandle_t* outHandle = nullptr)
    {
        return xTaskCreate(fn, name, stackBytes, param, prio, outHandle) == pdPASS;
    }

} // namespace CE::OS::Tasks