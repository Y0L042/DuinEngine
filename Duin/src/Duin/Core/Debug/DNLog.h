#ifndef DN_LOG_H
#define DN_LOG_H

#include "Duin/Core/Core.h"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>

namespace Duin
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
#define DN_CORE_FATAL(...) ::Duin::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define DN_CORE_ERROR(...) ::Duin::Log::GetCoreLogger()->error(__VA_ARGS__)
#define DN_CORE_WARN(...)  ::Duin::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define DN_CORE_INFO(...)  ::Duin::Log::GetCoreLogger()->info(__VA_ARGS__)
#define DN_CORE_TRACE(...) ::Duin::Log::GetCoreLogger()->trace(__VA_ARGS__)

// Client log macros
#define DN_FATAL(...)      ::Duin::Log::GetClientLogger()->critical(__VA_ARGS__)
#define DN_ERROR(...)      ::Duin::Log::GetClientLogger()->error(__VA_ARGS__)
#define DN_WARN(...)       ::Duin::Log::GetClientLogger()->warn(__VA_ARGS__)
#define DN_INFO(...)       ::Duin::Log::GetClientLogger()->info(__VA_ARGS__)
#define DN_TRACE(...)      ::Duin::Log::GetClientLogger()->trace(__VA_ARGS__)

#endif /* DN_LOG_H */
