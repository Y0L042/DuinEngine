#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"
#include "GameStates.h"

#define DEBUG

duin::Application *duin::CreateApplication()
{
    return new Game();
}

duin::DebugConsole debugConsole;
duin::DebugWatchlist debugWatchlist;


std::shared_ptr<duin::GameStateMachine> mainStateMachine;

void Game::Initialize()
{
    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
}

void Game::Ready()
{
    debugConsole.Log("Game is ready!");
    mainStateMachine = CreateChildObject<duin::GameStateMachine>();
    mainStateMachine->SwitchState<GameState_Gameplay>();
}

void Game::OnEvent(duin::Event event)
{
}

void Game::Update(double rDelta)
{
}

void Game::PhysicsUpdate(double pDelta)
{
}

void Game::Draw()
{
}

void Game::DrawUI()
{
    debugWatchlist.Draw("Watchlist");
    debugConsole.Draw("Console");
}

void Game::Debug()
{
}
