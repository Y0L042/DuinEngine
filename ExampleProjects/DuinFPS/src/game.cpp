#include "Game.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include "Singletons.h"

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
}

void Game::HandleInputs(duin::InputEvent event)
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
    debugConsole.Draw("Console");
}

