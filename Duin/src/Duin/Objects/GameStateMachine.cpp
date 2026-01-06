#include "dnpch.h"
#include "GameStateMachine.h"

#define VALIDATE_SM_STACK (!stateStack.empty() && stateStack.top())

namespace duin
{

GameState::GameState(GameStateMachine &owner) : owner(owner)
{
    stateGameObject = std::make_shared<GameObject>();
}

void GameState::StateEnter()
{
    Enter();
    stateGameObject->ObjectReady();
    OnStateEnter.Emit();
}

void GameState::StateOnEvent(Event e)
{
    if (!onEventEnabled)
        return;

    stateGameObject->ObjectOnEvent(e);
    OnEvent(e);
    OnStateOnEvent.Emit(e);
}

void GameState::StateUpdate(double delta)
{
    if (!updateEnabled)
        return;

    stateGameObject->ObjectUpdate(delta);
    Update(delta);
    OnStateUpdate.Emit(delta);
}

void GameState::StatePhysicsUpdate(double delta)
{
    if (!physicsUpdateEnabled)
        return;

    stateGameObject->ObjectPhysicsUpdate(delta);
    PhysicsUpdate(delta);
    OnStatePhysicsUpdate.Emit(delta);
}

void GameState::StateDraw()
{
    if (!drawEnabled)
        return;

    stateGameObject->ObjectDraw();
    Draw();
    OnStateDraw.Emit();
}

void GameState::StateDrawUI()
{
    if (!drawUIEnabled)
        return;

    stateGameObject->ObjectDrawUI();
    DrawUI();
    OnStateDrawUI.Emit();
}

void GameState::StateExit()
{
    Exit();
    OnStateExit.Emit();
}

void GameState::StateSetPause()
{
    SetPause();
}

void GameState::StateSetUnpause()
{
    SetUnpause();
}

void GameState::PopState()
{
    owner.PopState();
}

void GameState::FlushStack()
{
    owner.FlushStack();
}

void GameState::AddChildObject(std::shared_ptr<GameObject> child)
{
    stateGameObject->AddChildObject(child);
}

void GameState::RemoveChildObject(std::shared_ptr<GameObject> child)
{
    stateGameObject->RemoveChildObject(child);
}

// Signal connection functions
UUID GameState::ConnectOnStateEnter(std::function<void()> callback)
{
    return OnStateEnter.Connect(callback);
}

UUID GameState::ConnectOnStateOnEvent(std::function<void(Event)> callback)
{
    return OnStateOnEvent.Connect(callback);
}

UUID GameState::ConnectOnStateUpdate(std::function<void(double)> callback)
{
    return OnStateUpdate.Connect(callback);
}

UUID GameState::ConnectOnStatePhysicsUpdate(std::function<void(double)> callback)
{
    return OnStatePhysicsUpdate.Connect(callback);
}

UUID GameState::ConnectOnStateDraw(std::function<void()> callback)
{
    return OnStateDraw.Connect(callback);
}

UUID GameState::ConnectOnStateDrawUI(std::function<void()> callback)
{
    return OnStateDrawUI.Connect(callback);
}

UUID GameState::ConnectOnStateExit(std::function<void()> callback)
{
    return OnStateExit.Connect(callback);
}

// Signal disconnection functions
bool GameState::DisconnectOnStateEnter(UUID uuid)
{
    return OnStateEnter.Disconnect(uuid);
}

bool GameState::DisconnectOnStateOnEvent(UUID uuid)
{
    return OnStateOnEvent.Disconnect(uuid);
}

bool GameState::DisconnectOnStateUpdate(UUID uuid)
{
    return OnStateUpdate.Disconnect(uuid);
}

bool GameState::DisconnectOnStatePhysicsUpdate(UUID uuid)
{
    return OnStatePhysicsUpdate.Disconnect(uuid);
}

bool GameState::DisconnectOnStateDraw(UUID uuid)
{
    return OnStateDraw.Disconnect(uuid);
}

bool GameState::DisconnectOnStateDrawUI(UUID uuid)
{
    return OnStateDrawUI.Disconnect(uuid);
}

bool GameState::DisconnectOnStateExit(UUID uuid)
{
    return OnStateExit.Disconnect(uuid);
}

SignalConnections GameState::ConnectAllSignals(std::function<void()> onEnter, std::function<void(Event)> onEvent,
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

void GameState::DisconnectAllSignals(const SignalConnections &connections)
{
    DisconnectOnStateEnter(connections.onEnter);
    DisconnectOnStateOnEvent(connections.onEvent);
    DisconnectOnStateUpdate(connections.onUpdate);
    DisconnectOnStatePhysicsUpdate(connections.onPhysicsUpdate);
    DisconnectOnStateDraw(connections.onDraw);
    DisconnectOnStateDrawUI(connections.onDrawUI);
    DisconnectOnStateExit(connections.onExit);
}

// Enable functions
void GameState::Enable(bool enable)
{
    onEventEnabled = enable;
    updateEnabled = enable;
    physicsUpdateEnabled = enable;
    drawEnabled = enable;
    drawUIEnabled = enable;
}

void GameState::EnableOnEvent(bool enable)
{
    onEventEnabled = enable;
}

void GameState::EnableUpdate(bool enable)
{
    updateEnabled = enable;
}

void GameState::EnablePhysicsUpdate(bool enable)
{
    physicsUpdateEnabled = enable;
}

void GameState::EnableDraw(bool enable)
{
    drawEnabled = enable;
}

void GameState::EnableDrawUI(bool enable)
{
    drawUIEnabled = enable;
}

bool GameState::IsEqualTo(GameState* other)
{
    return (this->uuid == other->uuid);
}

UUID GameState::GetUUID()
{
    return uuid;
}

GameStateMachine &GameState::GetOwner()
{
    return owner;
}

const std::string GameState::GetName()
{
    return stateName;
}

} // namespace duin

namespace duin
{
GameStateMachine::GameStateMachine()
{
}

GameStateMachine::~GameStateMachine()
{
}

UUID GameStateMachine::GetUUID()
{
    return uuid;
}

void GameStateMachine::PopState()
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
    stateStack.pop();

    if (!stateStack.empty())
    {
        stateStack.top()->StateSetUnpause();
    }
}

void GameStateMachine::FlushStack()
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

std::stack<std::unique_ptr<GameState>> &GameStateMachine::GetStateStack()
{
    return stateStack;
}

void GameStateMachine::Init()
{
}

void GameStateMachine::Ready()
{
}

void GameStateMachine::OnEvent(Event e)
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

void GameStateMachine::Update(double delta)
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

void GameStateMachine::PhysicsUpdate(double delta)
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

void GameStateMachine::Draw()
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

void GameStateMachine::DrawUI()
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

void GameStateMachine::Debug()
{
}

} // namespace duin
