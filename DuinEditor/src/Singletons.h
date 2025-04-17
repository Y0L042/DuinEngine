#pragma once

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/UtilsModule.h>

#include "FileManager.h"
#include "Project.h"

extern FileManager fileManager;
extern duin::DebugConsole debugConsole;
extern duin::DebugWatchlist debugWatchlist;
extern duin::StateMachine mainStateMachine;

void SetActiveProject(Project project);
Project& GetActiveProject();
