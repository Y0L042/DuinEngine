#include "Main.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <memory>
#include <Duin/Objects/GameStateMachine.h>
#include "Sandboxes/MultiWindow/MultiWindowSandbox.h"

duin::Application *duin::CreateApplication(int argc, char **argv)
{
    return new Game();
}

std::shared_ptr<duin::GameStateMachine> sm;

void Game::Initialize()
{
    SetWindowName("Sandbox");
}

void Game::Ready()
{
    sm = CreateChildObject<duin::GameStateMachine>();
    //sm->SwitchState<ImGuiSandbox>();
    sm->SwitchState<MultiWindowSandbox>();
}

void Game::OnEvent(duin::Event e)
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
