#include "dnpch.h"
#include "DNLog.h"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/pattern_formatter.h>


#define LOG_TO_FILE 1

#ifdef LOG_TO_FILE
#include <format>
#include <iostream>
#include <chrono>
#endif

namespace duin {

    // External frame count function
    extern size_t GetPhysicsFrameCount();

    // Custom formatter for frame count
    class FrameCountFormatter : public spdlog::custom_flag_formatter {
    public:
        void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override {
            size_t frameCount = GetPhysicsFrameCount();
            fmt::format_to(std::back_inserter(dest), "{:0{}}", frameCount, 7);
        }

        std::unique_ptr<spdlog::custom_flag_formatter> clone() const override {
            return std::make_unique<FrameCountFormatter>();
        }
    };

    std::vector<spdlog::sink_ptr> sinks;
    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        // Create sinks to log to console and file
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
#ifdef LOG_TO_FILE
        const auto now = std::chrono::system_clock::now();
        std::string fileName = std::format("{:%Y-%m-%d_%H_%M_%OS}", now) + "_dnlogfile.txt";
        sinks.push_back(std::make_shared<spdlog::sinks::basic_file_sink_mt>("./logs/" + fileName));
#endif

        // Create a custom formatter with the frame count flag
        auto coreFormatter = std::make_unique<spdlog::pattern_formatter>();
        coreFormatter->add_flag<FrameCountFormatter>('F');
        coreFormatter->set_pattern("%^[ %T | %F ]\t%n\t(%s:%#): %v%$");

        //s_CoreLogger = spdlog::stdout_color_mt("DUIN");
        s_CoreLogger = std::make_shared<spdlog::logger>("CORE", begin(sinks), end(sinks));
        s_CoreLogger->set_formatter(std::move(coreFormatter));
        s_CoreLogger->set_level(spdlog::level::trace);

        // Duplicate formatter for the client logger
        auto clientFormatter = std::make_unique<spdlog::pattern_formatter>();
        clientFormatter->add_flag<FrameCountFormatter>('F');
        //clientFormatter->set_pattern("%^[%T] %n [%F] (%s:%#): %v%$");
        clientFormatter->set_pattern("%^[ %T | %F ]\t%n\t(%s:%#): %v%$");

        //s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger = std::make_shared<spdlog::logger>("APP", begin(sinks), end(sinks));
        s_ClientLogger->set_formatter(std::move(clientFormatter));
        s_ClientLogger->set_level(spdlog::level::trace);

        


    }
}




#if 0
#include "dnpch.h"
#include "DNLog.h"

#include <spdlog/sinks/stdout_color_sinks.h>

//#include "Duin/Core/Application.h"

namespace duin
{
    extern size_t GetPhysicsFrameCount();

    // Custom flag formatter to include the frame count
    class FrameCountFormatter : public spdlog::custom_flag_formatter {
    public:
        void format(const spdlog::details::log_msg& msg, const std::tm&, spdlog::memory_buf_t& dest) override {
            size_t frameCount = GetPhysicsFrameCount();
            // Correctly format the frame count into the buffer
            fmt::format_to(std::back_inserter(dest), "Frame: {}", frameCount);
        }

        std::unique_ptr<spdlog::custom_flag_formatter> clone() const override {
            return std::make_unique<FrameCountFormatter>();
        }
    };

    std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
    std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

    void Log::Init() {
        auto formatter = std::make_unique<spdlog::pattern_formatter>();
        formatter->add_flag<FrameCountFormatter>('F'); // Use 'F' as the custom flag
        formatter->set_pattern("%^[%T] %n [%F] (%s:%#): %v%$"); // Include '%F' in the pattern

        s_CoreLogger = spdlog::stdout_color_mt("DUIN");
        s_CoreLogger->set_formatter(std::move(formatter));
        s_CoreLogger->set_level(spdlog::level::trace);

        s_ClientLogger = spdlog::stdout_color_mt("APP");
        s_ClientLogger->set_formatter(std::move(formatter));
        s_ClientLogger->set_level(spdlog::level::trace);
    }

	//std::shared_ptr<spdlog::logger> Log::s_CoreLogger;
	//std::shared_ptr<spdlog::logger> Log::s_ClientLogger;

	//void Log::Init()
	//{
	//	//spdlog::set_pattern("%^[%T] %n: %v%$");
	//	//s_CoreLogger = spdlog::stdout_color_mt("DUIN");
	//	//s_CoreLogger->set_level(spdlog::level::trace);

	//	//s_ClientLogger = spdlog::stdout_color_mt("APP");
	//	//s_ClientLogger->set_level(spdlog::level::trace);

	//	spdlog::set_pattern("%^[%T] %n (%s:%#): %v%$");
	//	s_CoreLogger = spdlog::stdout_color_mt("DUIN");
	//	s_CoreLogger->set_level(spdlog::level::trace);

	//	s_ClientLogger = spdlog::stdout_color_mt("APP");
	//	s_ClientLogger->set_level(spdlog::level::trace);
	//}
}
#endif