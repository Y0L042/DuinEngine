#include "Editor.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "Project.h"
#include "States.h"

#define DEBUG

duin::Application* duin::CreateApplication() { return new Editor(); }

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;

duin::StateMachine mainStateMachine;

FileManager fileManager;
Project activeProject;

void Editor::Initialize()
{
    duin::SetFramerate(244);

    SetWindowStartupSize(1600, 900);

}

void Editor::Ready()
{
    duin::SetWindowResizable(true);
    duin::MaximizeWindow();

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

Project& GetActiveProject()
{
    return activeProject;
}

void SetActiveProject(Project project)
{
    activeProject = project;
}
