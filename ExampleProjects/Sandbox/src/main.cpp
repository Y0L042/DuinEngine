#include "Main.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

duin::Application* duin::CreateApplication() { return new Game(); }


void Game::Initialize()
{
    SetWindowName("Sandbox");
}

void Game::Ready()
{
}

void Game::HandleInputs()
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

