/**
 * @file DNLog.h
 * @brief Logging system and macros.
 * @ingroup Core_Debug
 *
 * Provides logging functionality via spdlog. Two loggers are available:
 * - Core logger (DN_CORE_*) for engine internals
 * - Client logger (DN_*) for application code
 *
 * Log levels from most to least severe: FATAL, ERROR, WARN, INFO, TRACE
 *
 * Define DN_DISABLE_ALL_LOGGING before including to disable logging.
 *
 * @code
 * DN_INFO("Player {} scored {} points", playerName, score);
 * DN_CORE_ERROR("Failed to load resource: {}", path);
 * @endcode
 */

#pragma once

#include "Duin/Core/Core.h"
#include <spdlog/spdlog.h>
#include <vector>

namespace duin
{

/**
 * @class Log
 * @brief Logging system manager.
 * @ingroup Core_Debug
 *
 * Initializes and provides access to spdlog loggers. The engine calls
 * Init() automatically; user code should use the DN_* macros.
 */
class DAPI Log
{
  public:
    /** @brief Initializes the logging system. Called automatically. */
    static void Init();

    /** @brief Returns the engine/core logger. */
    inline static std::shared_ptr<spdlog::logger> &GetCoreLogger()
    {
        if (s_CoreLogger == nullptr)
        {
            std::printf("Duin: DNLog was not initialised, running Init() now.\n");
            Init();
        }
        return s_CoreLogger;
    }

    /** @brief Returns the client/application logger. */
    inline static std::shared_ptr<spdlog::logger> &GetClientLogger()
    {
        if (s_ClientLogger == nullptr)
        {
            std::printf("Duin: DNLog was not initialised, running Init() now.\n");
            Init();
        }
        return s_ClientLogger;
    }

  private:
    static std::shared_ptr<spdlog::logger> s_CoreLogger;
    static std::shared_ptr<spdlog::logger> s_ClientLogger;
};

} // namespace duin

#ifdef DN_DISABLE_ALL_LOGGING

/**
 * @name Core Log Macros (Disabled)
 * @{
 */
#define DN_CORE_FATAL(...)
#define DN_CORE_ERROR(...)
#define DN_CORE_WARN(...)
#define DN_CORE_INFO(...)
#define DN_CORE_TRACE(...)
/** @} */

/**
 * @name Client Log Macros (Disabled)
 * @{
 */
#define DN_FATAL(...)
#define DN_ERROR(...)
#define DN_WARN(...)
#define DN_INFO(...)
#define DN_TRACE(...)
/** @} */

#else

/**
 * @name Core Log Macros
 * For engine/core code logging.
 * @{
 */
#define DN_CORE_FATAL(...) SPDLOG_LOGGER_CRITICAL(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_ERROR(...) SPDLOG_LOGGER_ERROR(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_WARN(...) SPDLOG_LOGGER_WARN(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_INFO(...) SPDLOG_LOGGER_INFO(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_TRACE(...) SPDLOG_LOGGER_TRACE(::duin::Log::GetCoreLogger(), __VA_ARGS__)
/** @} */

/**
 * @name Client Log Macros
 * For application code logging.
 * @{
 */
#define DN_FATAL(...) SPDLOG_LOGGER_CRITICAL(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_ERROR(...) SPDLOG_LOGGER_ERROR(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_WARN(...) SPDLOG_LOGGER_WARN(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_INFO(...) SPDLOG_LOGGER_INFO(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_TRACE(...) SPDLOG_LOGGER_TRACE(::duin::Log::GetClientLogger(), __VA_ARGS__)
/** @} */

#endif
