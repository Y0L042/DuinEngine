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
duin::TOMLValue projectTOML;

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

void Editor::SaveTOMLConfig(duin::TOMLValue tomlValue)
{
    std::ofstream ofs(activeProject.GetPathAsString());
    if (!ofs) {
        DN_FATAL("Unable to save tabs!");
    }
    toml::value& projectValue = projectTOML.GetValue();
    toml::value& value = tomlValue.GetValue();
    projectValue["TabBrowserConfigs"] = value;

    ofs << projectValue;  // This writes the TOML data to the file
    ofs.close();
}

duin::TOMLValue Editor::LoadTOMLConfig()
{
    Project& project = GetActiveProject();
    toml::value& projectValue = projectTOML.GetValue();
    projectValue = toml::parse(project.GetPathAsString());

    return projectTOML;
}

Project& GetActiveProject()
{
    return activeProject;
}

void SetActiveProject(Project project)
{
    activeProject = project;
}
