#include "EditorState_GameEditor.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <external/imgui.h>

#include "Singletons.h"
#include "States.h"
#include "gui/TabBrowser.h"
#include "gui/ViewportPanel.h"
#include "Editor.h"

TabBrowser tabBrowser;


void SaveProjectConfig()
{
    duin::TOMLValue tomlValue = tabBrowser.Serialise();
    Editor::SaveTOMLConfig(tomlValue);
}

void EditorState_GameEditor::Enter()
{
    debugConsole.LogEx(duin::LogLevel::Info, "ENTERING GameEditor");

    duin::TOMLValue tomlValue = Editor::LoadTOMLConfig();
    toml::value& value = tomlValue.GetValue();
    if (value.contains("TabBrowserConfigs")) {
        toml::value tabBrowserValues = value["TabBrowserConfigs"].as_table();
        tabBrowser.Init(duin::TOMLValue(tabBrowserValues));
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
