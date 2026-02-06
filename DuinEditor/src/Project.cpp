#include "Project.h"
#include <Duin/Core/Debug/DebugModule.h>

#include <string>

Project::Project(fs::path projectCfg)
{
    this->projectCfg = projectCfg;
    projectTitle = this->projectCfg.parent_path().filename().string();
    projectDir = this->projectCfg.parent_path();
    // if (this->projectCfg.has_filename())
    //{
    // }
    // else
    //{
    //     fs::path::iterator it = this->projectCfg.end();
    //     --it; // skip trailing slash
    //     --it; // get last folder
    //     projectTitle = *it->c_str();
    //     projectDir = it->parent_path();
    // }
}

Project::Project(std::string projectCfg) : Project(fs::path(projectCfg))
{
}

std::string Project::GetPathAsString()
{
    return std::string(projectDir.string());
};
