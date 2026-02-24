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
    //if (!onEventEnabled)
    //    return;

    OnEvent(e);
    OnStateOnEvent.Emit(e);
}

void duin::GameState::StateUpdate(double delta)
{
    //if (!updateEnabled)
    //    return;

    Update(delta);
    OnStateUpdate.Emit(delta);
}

void duin::GameState::StatePhysicsUpdate(double delta)
{
    //if (!physicsUpdateEnabled)
    //    return;

    PhysicsUpdate(delta);
    OnStatePhysicsUpdate.Emit(delta);
}

void duin::GameState::StateDraw()
{
    //if (!drawEnabled)
    //    return;

    Draw();
    OnStateDraw.Emit();
}

void duin::GameState::StateDrawUI()
{
    //if (!drawUIEnabled)
    //    return;

    DrawUI();
    OnStateDrawUI.Emit();
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

duin::UUID duin::GameState::ConnectOnStateOnEvent(std::function<void(Event)> callback)
{
    return OnStateOnEvent.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStateUpdate(std::function<void(double)> callback)
{
    return OnStateUpdate.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStatePhysicsUpdate(std::function<void(double)> callback)
{
    return OnStatePhysicsUpdate.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStateDraw(std::function<void()> callback)
{
    return OnStateDraw.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStateDrawUI(std::function<void()> callback)
{
    return OnStateDrawUI.Connect(callback);
}

duin::UUID duin::GameState::ConnectOnStateExit(std::function<void()> callback)
{
    return OnStateExit.Connect(callback);
}

// Signal disconnection functions
bool duin::GameState::DisconnectOnStateEnter(UUID uuid)
{
    return OnStateEnter.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStateOnEvent(UUID uuid)
{
    return OnStateOnEvent.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStateUpdate(UUID uuid)
{
    return OnStateUpdate.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStatePhysicsUpdate(UUID uuid)
{
    return OnStatePhysicsUpdate.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStateDraw(UUID uuid)
{
    return OnStateDraw.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStateDrawUI(UUID uuid)
{
    return OnStateDrawUI.Disconnect(uuid);
}

bool duin::GameState::DisconnectOnStateExit(UUID uuid)
{
    return OnStateExit.Disconnect(uuid);
}

duin::SignalConnections duin::GameState::ConnectAllSignals(std::function<void()> onEnter,
                                                           std::function<void(Event)> onEvent,
                                                           std::function<void(double)> onUpdate,
                                                           std::function<void(double)> onPhysicsUpdate,
                                                           std::function<void()> onDraw, std::function<void()> onDrawUI,
                                                           std::function<void()> onExit)
{
    SignalConnections connections;
    connections.onEnter = ConnectOnStateEnter(onEnter);
    connections.onEvent = ConnectOnStateOnEvent(onEvent);
    connections.onUpdate = ConnectOnStateUpdate(onUpdate);
    connections.onPhysicsUpdate = ConnectOnStatePhysicsUpdate(onPhysicsUpdate);
    connections.onDraw = ConnectOnStateDraw(onDraw);
    connections.onDrawUI = ConnectOnStateDrawUI(onDrawUI);
    connections.onExit = ConnectOnStateExit(onExit);
    return connections;
}

void duin::GameState::DisconnectAllSignals(const SignalConnections &connections)
{
    DisconnectOnStateEnter(connections.onEnter);
    DisconnectOnStateOnEvent(connections.onEvent);
    DisconnectOnStateUpdate(connections.onUpdate);
    DisconnectOnStatePhysicsUpdate(connections.onPhysicsUpdate);
    DisconnectOnStateDraw(connections.onDraw);
    DisconnectOnStateDrawUI(connections.onDrawUI);
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
    return uuid;
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
    poppedState->GetParent()->RemoveChildObject(poppedState);
    stateStack.pop();

    if (!stateStack.empty())
    {
        stateStack.top()->StateSetUnpause();
    }
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
