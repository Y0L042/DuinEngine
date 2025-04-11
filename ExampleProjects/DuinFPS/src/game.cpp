#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "GameStates.h"

duin::Application* duin::CreateApplication() { return new Game(); }

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;

duin::StateMachine mainStateMachine;


void Game::Initialize()
{
    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
}

void Game::Ready()
{
	debugConsole.Log("Game is ready!");
    mainStateMachine.SwitchState<GameState_Gameplay>();
}

void Game::OnEvent(duin::Event event)
{
    mainStateMachine.ExecuteOnEvent(event);
}

void Game::Update(double rDelta)
{
    mainStateMachine.ExecuteUpdate(rDelta);
}

void Game::PhysicsUpdate(double pDelta)
{
    mainStateMachine.ExecutePhysicsUpdate(pDelta);
}

void Game::Draw()
{
    mainStateMachine.ExecuteDraw();
}

void Game::DrawUI()
{
    mainStateMachine.ExecuteDrawUI();
    debugConsole.Draw("Console");
}

void Game::Debug()
{
}

