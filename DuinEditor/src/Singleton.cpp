#include "Singleton.h"

void Singleton::SetActiveProject(Project project)
{
    Singleton::Get().SetActiveProject_(project);
}

Project &Singleton::GetActiveProject()
{
    return Singleton::Get().GetActiveProject_();
}

void Singleton::SetActiveProject_(Project project)
{
    activeProject = project;
}

Project &Singleton::GetActiveProject_()
{
    return activeProject;
}
