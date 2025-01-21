#pragma once

#include <Duin.h>
#include "DuinEditor.h"
#include "Project.h"
#include "FileManager.h"

#include <string>

extern duin::DebugConsole debugConsole;
extern DuinEditor *application;
extern FileManager fileManager;

void SetActiveProject(Project project);
Project& GetActiveProject();
