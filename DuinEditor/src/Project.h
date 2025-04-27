#pragma once

#include <string>
#include <filesystem>
namespace fs = std::filesystem;


class Project
{
    public:
        int valid = 0;
        std::string projectTitle;
        fs::path projectDir;

        Project() = default;
        Project(std::string projectDir);
        Project(fs::path projectDir);

        std::string GetPathAsString();

    private:

};