#pragma once

#include <string>
#include <filesystem>
namespace fs = std::filesystem;


class Project
{
    public:
        int valid = 0;
        fs::path projectDir;

        Project() = default;
        Project(std::string projectDir);

        std::string GetPathAsString();

    private:

};