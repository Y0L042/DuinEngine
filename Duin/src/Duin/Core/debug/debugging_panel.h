#ifndef DEBUGGING_PANEL_H
#define DEBUGGING_PANEL_H

#include "dnpch.h"

#include <vector>
#include <string>
#include <mutex>

namespace Duin {

    enum class LogLevel {
        Fatal,
        Error,
        Warn,
        Info,
        Trace
    };

    class DebugConsole {
    public:
        DebugConsole();
        ~DebugConsole();

        void Clear();

        void Log(const char* format, ...);

        void Draw(const char* title);

        void SetScrollToBottom();

        void LogEx(LogLevel level, const char* format, ...); 



    private:
        bool scrollToBottom_ = true;

        std::vector<std::string> logMessages_;
        std::mutex mutex_;
        std::chrono::steady_clock::time_point startTime;
        int newMessageAdded_ = 0;
    };
}



#endif /* DEBUGGING_PANEL_H */
