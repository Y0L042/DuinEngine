#include "DuinEditor.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"

#include "GameStates/States.h"
#include "Project.h"
#include "FileManager.h"

#include <string>

duin::Application* duin::CreateApplication() { return new DuinEditor(); }

duin::GameStateMachine gameSM;
duin::DebugConsole debugConsole;
DuinEditor *application;

FileManager fileManager;
Project activeProject;



void DuinEditor::Initialize()
{
    application = this;

    SetWindowName("Duin Editor");
    duin::SetFramerate(96);
    SetWindowStartupSize(1600, 900);

}



void DuinEditor::Ready()
{
    gameSM.SwitchState<StateStartupMenu>();
}



void DuinEditor::HandleInputs()
{
    gameSM.ExecuteHandleInput();
}



void DuinEditor::Update(double rDelta)
{        
    gameSM.ExecuteUpdate(rDelta);
}



void DuinEditor::PhysicsUpdate(double pDelta)
{
    gameSM.ExecutePhysicsUpdate(pDelta);
}



void DuinEditor::Draw()
{
    gameSM.ExecuteDraw();
}



void DuinEditor::DrawUI()
{                
    gameSM.ExecuteDrawUI();
    debugConsole.Draw("Console");
}

Project& GetActiveProject()
{
    return activeProject;
}

void SetActiveProject(Project project)
{
    activeProject = project;
}
