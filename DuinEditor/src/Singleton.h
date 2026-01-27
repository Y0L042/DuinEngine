#pragma once

#include "Project.h"

class Singleton
{
  public:
    static Singleton &Get()
    {
        static Singleton instance;
        return instance;
    }

    static void SetActiveProject(Project project);
    static Project &GetActiveProject();

  private:
    Project activeProject;

    void SetActiveProject_(Project project);
    Project &GetActiveProject_();
};