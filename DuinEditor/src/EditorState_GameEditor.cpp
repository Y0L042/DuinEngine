#include "EditorState_GameEditor.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <external/imgui.h>

#include "Singletons.h"
#include "States.h"
#include "gui/TabBrowser.h"
#include "gui/ViewportPanel.h"
#include "gui/GuiMeta.h"
#include "Editor.h"

TabBrowser tabBrowser;


void SaveProjectConfig()
{
    duin::DataValue dataValue = tabBrowser.Serialise();
    Editor::SaveProjectEditorConfig(dataValue);
}

void EditorState_GameEditor::Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING GameEditor");

    duin::DataValue data = Editor::LoadProjectEditorConfig();
    if (data.HasMember(guitag::EDITOR_CONFIG)) {
        duin::DataValue tabBrowserData = data[guitag::EDITOR_CONFIG];
        tabBrowser.Init(tabBrowserData);
    }
    else {
        tabBrowser.Init();
    }
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
    tabBrowser.Render();
}

void EditorState_GameEditor::Exit()
{
}
