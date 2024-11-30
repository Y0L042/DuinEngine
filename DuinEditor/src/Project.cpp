#include "Project.h"
#include <Duin.h>

#include <string>

Project::Project(std::string projectDir)
{
    if (fs::is_directory(projectDir)) {
        this->projectDir = fs::path(projectDir);
    } else {
        DN_WARN("Project directory {} is invalid!", projectDir);
    }
};
