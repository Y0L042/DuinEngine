#include "MultiWindowSandbox.h"
#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Window/WindowManager.h>

MultiWindowSandbox::MultiWindowSandbox(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

MultiWindowSandbox::~MultiWindowSandbox()
{
}

void MultiWindowSandbox::Enter()
{
    Tests();
}

void MultiWindowSandbox::OnEvent(duin::Event e)
{
}

void MultiWindowSandbox::Update(double delta)
{
}

void MultiWindowSandbox::PhysicsUpdate(double delta)
{
}

void MultiWindowSandbox::Draw()
{
}

void MultiWindowSandbox::DrawUI()
{
}

void MultiWindowSandbox::Exit()
{
}

void MultiWindowSandbox::SetPause()
{
}

void MultiWindowSandbox::SetUnpause()
{
}

void MultiWindowSandbox::Tests()
{
    DN_INFO("Hello!");
    std::shared_ptr<duin::WindowCtx> win = duin::WindowManager::CreateNewWindow("Sandbox #01", 640, 480);
}
