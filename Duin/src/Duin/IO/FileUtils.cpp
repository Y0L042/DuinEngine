#include "dnpch.h"
#include "FileUtils.h"
#include <fstream>

// TODO Add error checking

bool duin::FileUtils::ReadFileIntoString(const std::string &filePath, std::string &string)
{
    std::ifstream t(filePath);
    std::stringstream buffer;
    buffer << t.rdbuf();
    string = buffer.str();

    return true;
}

bool duin::FileUtils::WriteStringIntoFile(const std::string &filePath, const std::string &string)
{
    std::ofstream out(filePath);
    out << string;
    out.close();

    return true;
}
