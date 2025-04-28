#pragma once

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/UtilsModule.h>

#include "Project.h"
#include "gui/FileManager.h"
#include "gui/TabBrowser.h"

extern FileManager fileManager;
extern TabBrowser tabBrowser;

extern duin::DebugConsole debugConsole;
extern duin::DebugWatchlist debugWatchlist;
extern duin::StateMachine mainStateMachine;


void SetActiveProject(Project project);
Project& GetActiveProject();

