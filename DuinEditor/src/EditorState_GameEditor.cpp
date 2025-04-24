#include "EditorState_GameEditor.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <external/imgui.h>

#include "Singletons.h"
#include "States.h"


void EditorState_GameEditor::Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING GameEditor");
}

void EditorState_GameEditor::OnEvent(duin::Event e)
{
}

void EditorState_GameEditor::Update(double delta)
{
}

void EditorState_GameEditor::PhysicsUpdate(double delta)
{
}

void EditorState_GameEditor::Draw()
{
}

void EditorState_GameEditor::DrawUI()
{
    debugWatchlist.Draw("Watchlist");
    debugConsole.Draw("Console");
}

void EditorState_GameEditor::Exit()
{
}
