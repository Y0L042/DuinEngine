#include "Editor.h"
#include "ExternalDependencies.h"

#include <DuinMeta.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "Project.h"
#include "States.h"
#include "gui/GuiMeta.h"
#include "EditorMeta.h"



int PROJECT_EDITOR_VERSION;
int PROJECT_ENGINE_VERSION;


#define DEBUG

duin::Application* duin::CreateApplication() { return new Editor(); }

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;

duin::StateMachine mainStateMachine;

FileManager fileManager;
Project activeProject;
std::shared_ptr<duin::JSONValue> PROJECT_DATA;

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

void Editor::SaveProjectEditorConfig(duin::JSONValue value)
{
    std::ofstream ofs(activeProject.GetPathAsString());
    if (!ofs) {
        DN_FATAL("Unable to save tabs!");
    }

    if (value.IsReadValid() && !value.IsNull()) {
        PROJECT_DATA->AddMember(guitag::EDITORVERSION, EDITOR_VERSION);
        PROJECT_DATA->AddMember(guitag::ENGINEVERSION, ENGINE_VERSION);
        PROJECT_DATA->AddMember(guitag::EDITOR_CONFIG, value);
    }

    ofs << duin::JSONValue::Write((*PROJECT_DATA), true);
    ofs.close();
}

duin::JSONValue Editor::LoadProjectEditorConfig()
{
    Project& project = GetActiveProject();
    PROJECT_DATA = std::make_shared<duin::JSONValue>(duin::JSONValue::Parse(project.GetPathAsString()));
    
    if (PROJECT_DATA->HasMember(guitag::EDITORVERSION)) {
        PROJECT_EDITOR_VERSION = (*PROJECT_DATA)[guitag::EDITORVERSION].GetInt();
    } else { PROJECT_EDITOR_VERSION = -1; }

    if (PROJECT_DATA->HasMember(guitag::ENGINEVERSION)) {
        PROJECT_EDITOR_VERSION = (*PROJECT_DATA)[guitag::ENGINEVERSION].GetInt();
    } else { PROJECT_ENGINE_VERSION = -1; }

    return (*PROJECT_DATA);
}

Project& GetActiveProject()
{
    return activeProject;
}

void SetActiveProject(Project project)
{
    activeProject = project;
}
