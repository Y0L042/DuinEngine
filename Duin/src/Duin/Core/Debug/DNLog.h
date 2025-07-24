#ifndef DN_LOG_H
#define DN_LOG_H

#include "Duin/Core/Core.h"
#include <spdlog/spdlog.h>
#include <vector>

namespace duin
{
	class DAPI Log
	{
	public:
		static void Init();

		inline static std::shared_ptr<spdlog::logger>& GetCoreLogger() 
        { 
            return s_CoreLogger; 
        }

		inline static std::shared_ptr<spdlog::logger>& GetClientLogger() 
        { 
            return s_ClientLogger; 
        }

	private:
		static std::shared_ptr<spdlog::logger> s_CoreLogger;
		static std::shared_ptr<spdlog::logger> s_ClientLogger;
	};
}


// Core log macros
#define DN_CORE_FATAL(...) SPDLOG_LOGGER_CRITICAL(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_ERROR(...) SPDLOG_LOGGER_ERROR(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_WARN(...)  SPDLOG_LOGGER_WARN(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_INFO(...)  SPDLOG_LOGGER_INFO(::duin::Log::GetCoreLogger(), __VA_ARGS__)
#define DN_CORE_TRACE(...) SPDLOG_LOGGER_TRACE(::duin::Log::GetCoreLogger(), __VA_ARGS__)

// Client log macros
#define DN_FATAL(...)      SPDLOG_LOGGER_CRITICAL(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_ERROR(...)      SPDLOG_LOGGER_ERROR(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_WARN(...)       SPDLOG_LOGGER_WARN(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_INFO(...)       SPDLOG_LOGGER_INFO(::duin::Log::GetClientLogger(), __VA_ARGS__)
#define DN_TRACE(...)      SPDLOG_LOGGER_TRACE(::duin::Log::GetClientLogger(), __VA_ARGS__)

#endif /* DN_LOG_H */
