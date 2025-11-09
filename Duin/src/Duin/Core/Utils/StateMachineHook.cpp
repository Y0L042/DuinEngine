#include "dnpch.h"
#include "StateMachineHook.h"

duin::StateMachineHook::StateMachineHook()
{
}

duin::StateMachineHook::~StateMachineHook()
{
}

void duin::StateMachineHook::HookToMachine(StateMachine* sm)
{
	//onEventSignalHandle = sm->onEventSignal.Connect([&](duin::Event e) { HandleEvents(e); });
	//onUpdateSignalHandle = sm->onUpdateSignal.Connect([&](double delta) { Update(delta); });
	//onPhysicsUpdateSignalHandle = EditorState_GameEditor::onPhysicsUpdateSignal.Connect([&](double delta) { PhysicsUpdate(delta); });
 //   onDrawSignalHandle = EditorState_GameEditor::onDrawSignal.Connect([&]() { Draw(); });
 //   onDrawUISignalHandle = EditorState_GameEditor::onDrawUISignal.Connect([&]() { DrawUI(); });
}

void duin::StateMachineHook::Unhook()
{
}

void duin::StateMachineHook::OnEvent(Event e)
{}

void duin::StateMachineHook::Update(double delta)
{}

void duin::StateMachineHook::PhysicsUpdate(double delta)
{}

void duin::StateMachineHook::Draw()
{}

void duin::StateMachineHook::DrawUI()
{}



void duin::StateMachineHook::ExecuteOnEvent(Event e)
{
    OnEvent(e);
}

void duin::StateMachineHook::ExecuteUpdate(double delta)
{
    Update(delta);
}

void duin::StateMachineHook::ExecutePhysicsUpdate(double delta)
{
    PhysicsUpdate(delta);
}

void duin::StateMachineHook::ExecuteDraw()
{
    Draw();
}

void duin::StateMachineHook::ExecuteDrawUI()
{
    DrawUI();
}

