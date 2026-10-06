#include "MultiWindowSandbox.h"
#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DNLog.h>

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
    duin::CreateNewWindow(640, 480, "Sandbox #01");
}
