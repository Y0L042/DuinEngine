#include "Editor.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "Project.h"
#include "States.h"
#include "gui/GuiMeta.h"


#define DEBUG

duin::Application* duin::CreateApplication() { return new Editor(); }

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;

duin::StateMachine mainStateMachine;

FileManager fileManager;
Project activeProject;
duin::DataValue projectData;

Editor* Editor::instance = nullptr;

Editor& Editor::Get()
{
    return *instance;
}

void Editor::Initialize()
{
    Editor::instance = this;

    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
}

void Editor::Ready()
{
    duin::SetWindowResizable(true);

	debugConsole.Log("Editor is ready!");
    mainStateMachine.SwitchState<EditorState_SelectProject>();
}

void Editor::OnEvent(duin::Event event)
{
    mainStateMachine.ExecuteOnEvent(event);
}

void Editor::Update(double rDelta)
{
    mainStateMachine.ExecuteUpdate(rDelta);
}

void Editor::PhysicsUpdate(double pDelta)
{
    mainStateMachine.ExecutePhysicsUpdate(pDelta);
}

void Editor::Draw()
{
    mainStateMachine.ExecuteDraw();
}

void Editor::DrawUI()
{
    mainStateMachine.ExecuteDrawUI();


}

void Editor::Debug()
{
}

void Editor::SaveProjectEditorConfig(duin::DataValue value)
{
    std::ofstream ofs(activeProject.GetPathAsString());
    if (!ofs) {
        DN_FATAL("Unable to save tabs!");
    }

    projectData.AddMember(guitag::EDITOR_CONFIG, value);

    ofs << duin::DataValue::Write(projectData, true);
    ofs.close();
}

duin::DataValue Editor::LoadProjectEditorConfig()
{
    Project& project = GetActiveProject();
    projectData = duin::DataValue::Parse(project.GetPathAsString());

    return projectData;
}

Project& GetActiveProject()
{
    return activeProject;
}

void SetActiveProject(Project project)
{
    activeProject = project;
}
