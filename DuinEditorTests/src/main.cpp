//#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#include <iostream>
#include <fstream>
#include <ctime>
#include <sstream>

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
    doctest::Context ctx;
    ctx.setOption("abort-after", 50);
    ctx.setOption("no-breaks", true);

    // Set output file to timestamped log
    std::string logFile = getTimestampedFilename();
    //ctx.setOption("out", logFile.c_str());

    int res = ctx.run();

    if (ctx.shouldExit()) 
    {
       
    }

    return res;
}
