//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT
#include <doctest.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cstring>
#include <Duin/Core/Debug/DNLog.h>

// Forward declaration for process-isolated test execution
// Defined in TestApplication.cpp
extern int RunIsolatedTest(const char* testName);

const char testLogDir[] = "./TestLogs";

std::string getTimestampedFilename() {
    std::time_t now = std::time(nullptr);
    std::tm* local = std::localtime(&now);
    std::ostringstream oss;
    oss << testLogDir << "/test_log_";
    oss << (1900 + local->tm_year)
        << "-"
        << (1 + local->tm_mon)
        << "-"
        << local->tm_mday
        << "_"
        << local->tm_hour
        << "-"
        << local->tm_min
        << "-"
        << local->tm_sec
        << ".log";
    return oss.str();
}

int main(int argc, char** argv)
{
    // Check if we're running an isolated test
    // Usage: DuinTests.exe --run-test-isolated "Test Name"
    if (argc == 3 && strcmp(argv[1], "--run-test-isolated") == 0)
    {
        const char* testName = argv[2];

        // Initialize logging for isolated test
        duin::Log::Init();

        // Run the isolated test and return its exit code
        int exitCode = RunIsolatedTest(testName);

        return exitCode;
    }

    // Normal doctest execution
    duin::Log::Init();

    doctest::Context ctx;
    ctx.setOption("abort-after", 50);
    ctx.setOption("no-breaks", true);

    // Pass command line arguments to doctest
    ctx.applyCommandLine(argc, argv);

    // Set output file to timestamped log
    std::string logFile = getTimestampedFilename();
    //ctx.setOption("out", logFile.c_str());

    int res = ctx.run();

    if (ctx.shouldExit())
    {

    }

    return res;
}