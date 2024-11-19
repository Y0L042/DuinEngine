#include "DuinEditor.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"

#include "GameStates/States.h"


duin::Application* duin::CreateApplication() { return new DuinEditor(); }

duin::GameStateMachine gameSM;
duin::DebugConsole debugConsole;
DuinEditor *application;



void DuinEditor::Initialize()
{
    application = this;

    SetWindowName("Duin Editor");
    SetFramerate(60);
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


