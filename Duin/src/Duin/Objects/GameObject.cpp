#include "dnpch.h"
#include "GameObject.h"
#include "ObjectManager.h"
#include <Duin/Core/Debug/DebugModule.h>

duin::GameObject::GameObject()
{
}

duin::GameObject::~GameObject()
{
}

void duin::GameObject::AddChildObject(std::shared_ptr<GameObject> child)
{
    if (child)
    {
        // Guard: Ensure this object is managed by a shared_ptr before calling shared_from_this()
        // This prevents std::bad_weak_ptr when AddChildObject/CreateChildObject is called from a constructor
        DN_CORE_DEBUG_ASSERT(!weak_from_this().expired(),
                             "Cannot add child object: parent is not yet managed by a shared_ptr. Move "
                             "CreateChildObject() calls from the constructor to Init() or Ready()");

        // Check if the child is already present by comparing UUIDs
        auto it =
            std::find_if(children.begin(), children.end(), [&child](const std::shared_ptr<GameObject> &existingChild) {
                return existingChild->GetUUID() == child->GetUUID();
            });

        // Only add the child if it is not already a child
        if (it == children.end())
        {
            children.push_back(child);
            std::shared_ptr<GameObject> selfPtr = GetSharedPointer<GameObject>();
            child->SetParent(selfPtr);
            child->Init();
        }
    }
}

void duin::GameObject::RemoveChildObject(std::shared_ptr<GameObject> child)
{
    if (child)
    {
        children.erase(std::remove(children.begin(), children.end(), child), children.end());

        child->ResetParent();
    }
}

void duin::GameObject::TransferChildObject(std::shared_ptr<GameObject> child, std::shared_ptr<GameObject> newParent)
{
    RemoveChildObject(child);
    newParent->AddChildObject(child);
}

void duin::GameObject::SetParent(std::shared_ptr<GameObject> parent)
{
    this->parent = parent;
}

std::shared_ptr<duin::GameObject> duin::GameObject::GetParent()
{
    return parent;
}

void duin::GameObject::ResetParent()
{
    this->parent = nullptr;
}

// Signal connection implementations
duin::UUID duin::GameObject::ConnectOnObjectReady(std::function<void()> callback)
{
    return OnObjectReady.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectOnEvent(std::function<void(Event)> callback)
{
    return OnObjectOnEvent.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectUpdate(std::function<void(double)> callback)
{
    return OnObjectUpdate.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectPhysicsUpdate(std::function<void(double)> callback)
{
    return OnObjectPhysicsUpdate.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectDraw(std::function<void()> callback)
{
    return OnObjectDraw.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectDrawUI(std::function<void()> callback)
{
    return OnObjectDrawUI.Connect(callback);
}

duin::UUID duin::GameObject::ConnectOnObjectDebug(std::function<void()> callback)
{
    return OnObjectDebug.Connect(callback);
}

// Signal disconnection implementations
bool duin::GameObject::DisconnectOnObjectReady(UUID uuid)
{
    return OnObjectReady.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectOnEvent(UUID uuid)
{
    return OnObjectOnEvent.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectUpdate(UUID uuid)
{
    return OnObjectUpdate.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectPhysicsUpdate(UUID uuid)
{
    return OnObjectPhysicsUpdate.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectDraw(UUID uuid)
{
    return OnObjectDraw.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectDrawUI(UUID uuid)
{
    return OnObjectDrawUI.Disconnect(uuid);
}

bool duin::GameObject::DisconnectOnObjectDebug(UUID uuid)
{
    return OnObjectDebug.Disconnect(uuid);
}

// Connect all signals at once
duin::SignalConnections duin::GameObject::ConnectAllSignals(
    std::function<void()> onReady, std::function<void(Event)> onEvent, std::function<void(double)> onUpdate,
    std::function<void(double)> onPhysicsUpdate, std::function<void()> onDraw, std::function<void()> onDrawUI,
    std::function<void()> onDebug)
{
    SignalConnections connections;
    connections.onReady = OnObjectReady.Connect(onReady);
    connections.onEvent = OnObjectOnEvent.Connect(onEvent);
    connections.onUpdate = OnObjectUpdate.Connect(onUpdate);
    connections.onPhysicsUpdate = OnObjectPhysicsUpdate.Connect(onPhysicsUpdate);
    connections.onDraw = OnObjectDraw.Connect(onDraw);
    connections.onDrawUI = OnObjectDrawUI.Connect(onDrawUI);
    connections.onDebug = OnObjectDebug.Connect(onDebug);
    return connections;
}

// Disconnect all signals at once
void duin::GameObject::DisconnectAllSignals(const SignalConnections &connections)
{
    OnObjectReady.Disconnect(connections.onReady);
    OnObjectOnEvent.Disconnect(connections.onEvent);
    OnObjectUpdate.Disconnect(connections.onUpdate);
    OnObjectPhysicsUpdate.Disconnect(connections.onPhysicsUpdate);
    OnObjectDraw.Disconnect(connections.onDraw);
    OnObjectDrawUI.Disconnect(connections.onDrawUI);
    OnObjectDebug.Disconnect(connections.onDebug);
}

void duin::GameObject::Init()
{
}

void duin::GameObject::Ready()
{
}

void duin::GameObject::OnEvent(Event event)
{
}

void duin::GameObject::Update(double delta)
{
}

void duin::GameObject::PhysicsUpdate(double delta)
{
}

void duin::GameObject::Draw()
{
}

void duin::GameObject::DrawUI()
{
}

void duin::GameObject::Debug()
{
}

void duin::GameObject::Enable(bool enable)
{
    EnableOnEvent(enable);
    EnableUpdate(enable);
    EnablePhysicsUpdate(enable);
    EnableDraw(enable);
    EnableDrawUI(enable);
    EnableDebug(enable);
}

void duin::GameObject::EnableOnEvent(bool enable)
{
    onEventEnabled = enable;
}

void duin::GameObject::EnableUpdate(bool enable)
{
    updateEnabled = enable;
}

void duin::GameObject::EnablePhysicsUpdate(bool enable)
{
    physicsUpdateEnabled = enable;
}

void duin::GameObject::EnableDraw(bool enable)
{
    drawEnabled = enable;
}

void duin::GameObject::EnableDrawUI(bool enable)
{
    drawUIEnabled = enable;
}

void duin::GameObject::EnableDebug(bool enable)
{
    debugEnabled = enable;
}

duin::UUID duin::GameObject::GetUUID() const
{
    return uuid;
}

bool duin::GameObject::operator==(const GameObject &other) const
{
    return this->GetUUID() == other.GetUUID();
}

bool duin::GameObject::operator!=(const GameObject &other) const
{
    return this->GetUUID() != other.GetUUID();
}

void duin::GameObject::ObjectReady()
{
    for (auto &child : children)
    {
        if (child)
            child->ObjectReady();
    }
    Ready();
    OnObjectReady.Emit();
}

void duin::GameObject::ObjectOnEvent(Event event)
{
    if (!onEventEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectOnEvent(event);
    }
    OnEvent(event);
    OnObjectOnEvent.Emit(event);
}

void duin::GameObject::ObjectUpdate(double delta)
{
    if (!updateEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectUpdate(delta);
    }
    Update(delta);
    OnObjectUpdate.Emit(delta);
}

void duin::GameObject::ObjectPhysicsUpdate(double delta)
{
    if (!physicsUpdateEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectPhysicsUpdate(delta);
    }
    PhysicsUpdate(delta);
    OnObjectPhysicsUpdate.Emit(delta);
}

void duin::GameObject::ObjectDraw()
{
    if (!drawEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectDraw();
    }
    Draw();
    OnObjectDraw.Emit();
}

void duin::GameObject::ObjectDrawUI()
{
    if (!drawUIEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectDrawUI();
    }
    DrawUI();
    OnObjectDrawUI.Emit();
}

void duin::GameObject::ObjectDebug()
{
    if (!debugEnabled)
        return;

    for (auto &child : children)
    {
        if (child)
            child->ObjectDebug();
    }
    Debug();
    OnObjectDebug.Emit();
}
