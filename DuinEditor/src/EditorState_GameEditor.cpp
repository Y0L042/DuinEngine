#include "EditorState_GameEditor.h"

#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Events/EventsModule.h>
#include <external/imgui.h>

#include "Singletons.h"
#include "States.h"
#include "gui/EditorWindow.h"
#include "gui/ViewportPanel.h"
#include "gui/GuiMeta.h"
#include "Editor.h"

EditorWindow tabBrowser;
bool isGameEditorValid = false;
duin::Signal<double> onPhysicsUpdateSignal;
duin::Signal<duin::Event> onEventSignal;
duin::Signal<> onDrawSignal;
duin::Signal<> onDrawUISignal;

void SaveProjectConfig()
{
    duin::DataValue dataValue = tabBrowser.Serialise();
    Editor::SaveProjectEditorConfig(dataValue);
}


EditorState_GameEditor::EditorState_GameEditor(duin::StateMachine& owner)
    : State(owner)
{
    stateName = "EditorState_SelectProject";
    isGameEditorValid = true;
}

EditorState_GameEditor::~EditorState_GameEditor() 
{
    isGameEditorValid = false;
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
        DN_WARN("Project has no EDITOR_CONFIG, creating new one...");
        tabBrowser.Init();
    }

    static std::shared_ptr<duin::InputDevice_Keyboard> keyboardInput = std::make_shared<duin::InputDevice_Keyboard>();
    static std::shared_ptr<duin::InputDevice_Mouse> mouseInput = std::make_shared<duin::InputDevice_Mouse>();
    duin::AddInputActionBinding("OnEditorCameraOrbit", mouseInput, DN_BUTTON_RIGHT, duin::Input::KeyEvent::HELD);
    duin::AddInputActionBinding("OnEditorCameraOrbit", keyboardInput, DN_KEY_LALT, duin::Input::KeyEvent::HELD);
}

void EditorState_GameEditor::OnEvent(duin::Event e)
{
    onEventSignal.Emit(e);
}

void EditorState_GameEditor::Update(double delta)
{
}

void EditorState_GameEditor::PhysicsUpdate(double delta)
{
    onPhysicsUpdateSignal.Emit(delta);
}

void EditorState_GameEditor::Draw()
{
    onDrawSignal.Emit();
}

void EditorState_GameEditor::DrawUI()
{
    onDrawUISignal.Emit();
    tabBrowser.Render();
}

void EditorState_GameEditor::Exit()
{
}

