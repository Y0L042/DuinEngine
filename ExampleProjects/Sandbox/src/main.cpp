#include "Main.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <memory>
#include <Duin/Objects/GameStateMachine.h>
#include "Sandboxes/Scenes/SandboxScene.h"
#include "Sandboxes/GUI/ImGuiSandbox.h"

duin::Application *duin::CreateApplication()
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
    // sm->SwitchState<SandboxScene>();
    sm->SwitchState<ImGuiSandbox>();
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
