//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT 
#define DN_DISABLE_ALL_LOGGING
#include <doctest.h>
#include <Duin/Core/Debug/DNLog.h>
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>
#include <cstring>

#define __TESTING__

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
    //// Check if we're running an isolated test FIRST (before doctest processing)
    //if (argc == 3 && strcmp(argv[1], "--run-test-isolated") == 0)
    //{
    //    const char* testName = argv[2];
    //    int exitCode = RunIsolatedTest(testName);
    //    return exitCode;
    //}

    // Create doctest context
    doctest::Context ctx;

    // Only configure options for actual test execution
    ctx.setOption("abort-after", 50);
    ctx.setOption("no-breaks", true);
    
    // Apply command line arguments (this processes --list-tests and other flags)
    ctx.applyCommandLine(argc, argv);

    // Run the tests
    int res = ctx.run();

    // Check if we should exit early (e.g., --list-tests, --help, --version)
    // If doctest is in "list mode", it will have set shouldExit() to true
    if (ctx.shouldExit()) {
        // Just run doctest to output the list/help, then exit
        return res;
    }

    return res;
}
