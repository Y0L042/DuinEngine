#include "main.h"

#include <DuinMeta.h>
#include <Duin/EntryPoint.h>
#include <Duin/Objects/GameStateMachine.h>

#include "EditorStates/States.h"

#include <memory>

#ifndef DEBUG
#define DEBUG
#endif

Editor *Editor::instance = nullptr;
std::shared_ptr<duin::GameStateMachine> sm;

duin::Application *duin::CreateApplication()
{
    return new Editor();
}

Editor &Editor::Get()
{
    return *instance;
}

void Editor::Initialize()
{
    Editor::instance = this;

    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
    SetWindowName("DuinEditor");
    duin::SetAllowDockingInMain(true);
}

void Editor::Ready()
{
    duin::SetWindowResizable(true);

    sm = CreateChildObject<duin::GameStateMachine>();
    sm->SwitchState<State_SelectProject>();
}

void Editor::OnEvent(duin::Event event)
{
}

void Editor::Update(double rDelta)
{
}

void Editor::PhysicsUpdate(double pDelta)
{
}

void Editor::Draw()
{
}

void Editor::DrawUI()
{
}

void Editor::Debug()
{
}
