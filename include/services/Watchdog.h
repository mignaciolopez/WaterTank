//
// Created by lmartinignacio@gmail.com on 3/3/2026.
//

#pragma once

#include <map>

#include "config/BuildConfig.hpp"
#include "domain/ErrorReport.hpp"
#include "os/Queues.hpp"

namespace CE::Services
{
    class Watchdog
    {
    public:
        static const char* TAG;
        static const char* REPORTS_FILENAME;

        Watchdog();
        [[nodiscard]] static bool Setup();

        struct MonitoredTask
        {
            float timeout_s;
            time_t timestamp;
        };

        static bool RegisterTask(const char* name, unsigned short timeoutMs);
        static void NotifyTaskAlive(const char* name);

        // Report an error from any service
        static void ReportError(Domain::ErrorSeverity severity,
                              Domain::ErrorType type,
                              const char* serviceName,
                              const char* message);

        // Get error report count
        static unsigned short GetErrorCount();

        // Clear all error reports
        static void ClearErrors();

    private:
        [[noreturn]] static void Task(void* pvParameters);

        static void CheckTaskHealth();

        static std::map<const char*, MonitoredTask>  tasks_;

        // Health check functions
        static void CheckMemoryHealth();
        static void CheckWiFiHealth();

        // Recovery actions
        static void AttemptWiFiReconnect();
        static void TriggerESPRestart(const char* reason);
        static void ProgramESPRestart(const char* reason);

        // Error report storage
        static bool LoadReports();
        static bool SaveReports();
        static bool Troubleshoot(const Domain::ErrorReport& report);

        static void Notify(const Domain::ErrorReport &report);

        // Internal state
        static QueueHandle_t errorQueue_;
        static unsigned short errorCount_;
        static uint8_t restartCount_;
        static uint8_t restartVotes_;
    };

}   // namespace CE::Services
