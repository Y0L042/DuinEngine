#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "GameStates/States.h"

Duin::Application* Duin::CreateApplication() { return new Game(); }

Duin::DebugConsole dbgConsole;


Duin::GameStateMachine gameSM;




void Game::Initialize()
{
}

void Game::Ready()
{
    SetWindowTitle("DuinFPS");
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
    dbgConsole.Draw("Console");
}

