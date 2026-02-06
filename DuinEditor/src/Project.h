#pragma once

#include <string>
#include <filesystem>
namespace fs = std::filesystem;

class Project
{
  public:
    int valid = 0;
    fs::path projectCfg;
    fs::path projectDir;
    std::string projectTitle;

    Project() = default;
    Project(std::string projectCfg);
    Project(fs::path projectCfg);

    std::string GetPathAsString();

  private:
};
