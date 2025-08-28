#pragma once

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/UtilsModule.h>

#include "Project.h"
#include "gui/FileManager.h"
#include "gui/EditorWindow.h"

#include <memory>

extern int PROJECT_EDITOR_VERSION;
extern int PROJECT_ENGINE_VERSION;

extern FileManager fileManager;
extern std::shared_ptr<EditorWindow> editorWindow;

extern duin::DebugConsole debugConsole;
extern duin::DebugWatchlist debugWatchlist;
extern duin::StateMachine mainStateMachine;

extern std::shared_ptr<duin::JSONValue> PROJECT_DATA;

void SetActiveProject(Project project);
Project& GetActiveProject();

