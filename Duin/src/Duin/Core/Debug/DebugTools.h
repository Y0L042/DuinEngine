#ifndef DEBUGGING_PANEL_H
#define DEBUGGING_PANEL_H

#include <unordered_map>
#include <vector>
#include <string>
#include <mutex>

namespace duin
{

enum class LogLevel
{
    Fatal,
    Error,
    Warn,
    Info,
    Trace
};

class DebugConsole
{
  public:
    int enableSilent = 0;

    DebugConsole();
    ~DebugConsole();

    void Clear();

    void Log(const char *format, ...);

    void Draw(const char *title);

    void SetScrollToBottom();

    void LogEx(LogLevel level, const char *format, ...);

    void ToggleEditing();

  private:
    bool scrollToBottom_ = true;
    int enableEditing = 0;

    std::vector<std::string> logMessages_;
    std::mutex mutex_;
    std::chrono::steady_clock::time_point startTime;
    int newMessageAdded = 0;
};
} // namespace duin

namespace duin
{

class DebugWatchlist
{
  public:
    int enableSilent = 0;

    DebugWatchlist();
    ~DebugWatchlist();

    void Clear();

    void Post(const std::string description, const std::string format, ...);
    void Draw(const char *title);
    void ToggleEditing();

  private:
    int enableEditing = 0;

    std::unordered_map<std::string, std::string> watchlist;
    std::vector<std::string> order;
    std::mutex mutex_;
};
} // namespace duin

#endif /* DEBUGGING_PANEL_H */
