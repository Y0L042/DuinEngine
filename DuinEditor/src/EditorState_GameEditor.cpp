#include "EditorState_GameEditor.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <external/imgui.h>

#include "Singletons.h"
#include "States.h"
#include "gui/TabBrowser.h"
#include "gui/ViewportPanel.h"

TabBrowser tabBrowser;
ViewportPanel viewportPanelTest("Test");


void EditorState_GameEditor::Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING GameEditor");

    tabBrowser.Init();
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

    tabBrowser.Render();
    viewportPanelTest.Draw();
}

void EditorState_GameEditor::Exit()
{
}
