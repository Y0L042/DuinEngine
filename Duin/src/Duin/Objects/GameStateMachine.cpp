#include "dnpch.h"
#include "GameStateMachine.h"

#define VALIDATE_SM_STACK (!stateStack.empty() && stateStack.top())

duin::GameState::GameState(GameStateMachine &owner) : owner(owner)
{
    Enable(false);
}

void duin::GameState::StateEnter()
{
    Enter();
    OnStateEnter.Emit();
}

void duin::GameState::StateOnEvent(Event e)
{
    ObjectOnEvent(e);
}

void duin::GameState::StateUpdate(double delta)
{
    ObjectUpdate(delta);
}

void duin::GameState::StatePhysicsUpdate(double delta)
{
    ObjectPhysicsUpdate(delta);
}

void duin::GameState::StateDraw()
{
    ObjectDraw();
}

void duin::GameState::StateDrawUI()
{
    ObjectDrawUI();
}

void duin::GameState::StateExit()
{
    Exit();
    OnStateExit.Emit();
}

void duin::GameState::StateSetPause()
{
    Enable(false);
    SetPause();
}

void duin::GameState::StateSetUnpause()
{
    Enable(true);
    SetUnpause();
}

void duin::GameState::PopState()
{
    owner.PopState();
}

void duin::GameState::FlushStack()
{
    owner.FlushStack();
}

// Signal connection functions
duin::UUID duin::GameState::ConnectOnStateEnter(std::function<void()> callback)
{
    return OnStateEnter.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStateExit(std::function<void()> callback)
{
    return OnStateExit.Connect(callback);
}

// Signal disconnection functions
void duin::GameState::DisconnectOnStateEnter(UUID uuid)
{
    OnStateEnter.Disconnect(uuid);
}

void duin::GameState::DisconnectOnStateExit(UUID uuid)
{
    OnStateExit.Disconnect(uuid);
}

duin::SignalConnections duin::GameState::ConnectAllSignals(std::function<void()> onEnter,
                                                           std::function<void()> onExit)
{
    SignalConnections connections;
    connections.onEnter = ConnectOnStateEnter(onEnter);
    connections.onExit = ConnectOnStateExit(onExit);
    return connections;
}

void duin::GameState::DisconnectAllSignals(const SignalConnections &connections)
{
    DisconnectOnStateEnter(connections.onEnter);
    DisconnectOnStateExit(connections.onExit);
}

bool duin::GameState::IsEqualTo(GameState *other)
{
    return (this->uuid == other->uuid);
}

duin::UUID duin::GameState::GetUUID()
{
    return uuid;
}

duin::GameStateMachine &duin::GameState::GetOwner()
{
    return owner;
}

const std::string duin::GameState::GetName()
{
    return stateName;
}

duin::GameStateMachine::GameStateMachine()
{
}

duin::GameStateMachine::~GameStateMachine()
{
}

duin::UUID duin::GameStateMachine::GetUUID()
{
    return GameObject::GetUUID();
}

void duin::GameStateMachine::PopState()
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StateExit();
    std::shared_ptr<GameState> poppedState = stateStack.top();
    RemoveChildObject(poppedState);
    stateStack.pop();
}

void duin::GameStateMachine::FlushStack()
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    while (!stateStack.empty())
    {
        PopState();
    }
}

std::stack<std::shared_ptr<duin::GameState>> &duin::GameStateMachine::GetStateStack()
{
    return stateStack;
}

void duin::GameStateMachine::Init()
{
}

void duin::GameStateMachine::Ready()
{
}

void duin::GameStateMachine::OnEvent(Event e)
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StateOnEvent(e);
}

void duin::GameStateMachine::Update(double delta)
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StateUpdate(delta);
}

void duin::GameStateMachine::PhysicsUpdate(double delta)
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StatePhysicsUpdate(delta);
}

void duin::GameStateMachine::Draw()
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StateDraw();
}

void duin::GameStateMachine::DrawUI()
{
    if (!VALIDATE_SM_STACK)
    {
        return;
    }

    if (stateStack.empty())
    {
        return;
    }
    stateStack.top()->StateDrawUI();
}

void duin::GameStateMachine::Debug()
{
}
