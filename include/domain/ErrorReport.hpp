//
// Created by lmartinignacio@gmail.com on 3/3/2026.
//

#pragma once
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace CE::Domain
{
    enum class ErrorSeverity : uint8_t
    {
        Info = 0,
        Warning = 1,
        Error = 2,
        Critical = 3
    };

    enum class ErrorType : uint8_t
    {
        ServiceStuck = 0,
        ServiceCrash = 1,
        WiFiDisconnect = 2,
        LowMemory = 3,
        StackOverflow = 4,
        QueueEmpty = 5,
        QueueFull = 6,
        SensorFailure = 7,
        ConfigError = 8,
        TaskDeleted = 9,
        WaterLevel = 10,
        PumpTimeout = 11,
        Unknown = 255,
    };

    struct ErrorReport
    {
        time_t timestamp;                   // Unix timestamp
        ErrorSeverity severity;
        ErrorType type;
        char serviceName[32];               // Name of affected service
        char message[128];                  // Error description
        uint32_t freeHeap;                  // Free heap at error time
        uint32_t minFreeHeap;               // Minimum free heap ever seen
        unsigned short taskStackWatermark;        // Task stack high watermark (if applicable)
        uint8_t restartCount;               // Number of restarts triggered
    };

    inline const char* ErrorSeverityToString(ErrorSeverity severity)
    {
        switch (severity)
        {
            case ErrorSeverity::Info:     return "Info";
            case ErrorSeverity::Warning:  return "Warning";
            case ErrorSeverity::Error:    return "Error";
            case ErrorSeverity::Critical: return "Critical";
            default:                      return "Unknown";
        }
    }

    inline const char* ErrorTypeToString(ErrorType type)
    {
        switch (type)
        {
            case ErrorType::ServiceStuck:    return "ServiceStuck";
            case ErrorType::ServiceCrash:    return "ServiceCrash";
            case ErrorType::WiFiDisconnect:  return "WiFiDisconnect";
            case ErrorType::LowMemory:       return "LowMemory";
            case ErrorType::StackOverflow:   return "StackOverflow";
            case ErrorType::QueueEmpty:      return "QueueEmpty";
            case ErrorType::QueueFull:       return "QueueFull";
            case ErrorType::SensorFailure:   return "SensorFailure";
            case ErrorType::ConfigError:     return "ConfigError";
            case ErrorType::TaskDeleted:     return "TaskDeleted";
            case ErrorType::WaterLevel:      return "WaterLevel";
            case ErrorType::PumpTimeout:     return "PumpTimeout";
            case ErrorType::Unknown:
            default:                         return "Unknown";
        }
    }

}   // namespace CE::Domain
