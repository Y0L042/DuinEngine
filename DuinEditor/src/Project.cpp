#include "Project.h"
#include <Duin/Core/Debug/DebugModule.h>

#include <string>

Project::Project(fs::path projectDir)
{
    DN_INFO("Selected project path {}",projectDir.string());
    this->projectDir = projectDir;
    if (this->projectDir.has_filename()) {
        projectTitle = this->projectDir.parent_path().filename().string();
    }
    else {
        fs::path::iterator it = this->projectDir.end();
        --it; // skip trailing slash
        --it; // get last folder
        projectTitle = *it->c_str();
    }
}


Project::Project(std::string projectDir)
    : Project(fs::path(projectDir))
{

}

std::string Project::GetPathAsString()
{
    return std::string(projectDir.string());
}
;
