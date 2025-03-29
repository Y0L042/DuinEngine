#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "GameStates/States.h"

duin::Application* duin::CreateApplication() { return new Game(); }

duin::DebugConsole debugConsole;


duin::GameStateMachine gameSM;



void Game::Initialize()
{
    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
}

void Game::Ready()
{
    //SetWindowTitle("DuinFPS");
    gameSM.PushState<StateGameLoop>();
}

void Game::HandleInputs()
{
    gameSM.ExecuteHandleInput();
}

void Game::Update(double rDelta)
{
    gameSM.ExecuteUpdate(rDelta);
}

void Game::PhysicsUpdate(double pDelta)
{
    gameSM.ExecutePhysicsUpdate(pDelta);
}

void Game::Draw()
{
    gameSM.ExecuteDraw();
}

void Game::DrawUI()
{
    gameSM.ExecuteDrawUI();
    debugConsole.Draw("Console");
}

