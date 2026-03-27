#include "dnpch.h"
#include "GameObject.h"
#include "GameObjectImpl.h"
#include "ObjectManager.h"
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Debug/DNAssert.h>

duin::GameObject::GameObject() : impl(std::make_shared<GameObjectImpl>(this))
{
    DN_CORE_INFO("Construct GameObject!");
}

duin::GameObject::~GameObject()
{
    if (impl)
    {
        impl->owner = nullptr;
    }
}

void duin::GameObject::AddChildObject(std::shared_ptr<GameObject> child)
{
    if (child && impl)
    {
        auto childImpl = child->GetImpl();
        if (!childImpl)
            return; 

        // Check if the child is already present by comparing UUIDs
        auto &ci = impl->childImpls;
        auto it = std::find_if(ci.begin(), ci.end(), [&](const std::shared_ptr<GameObjectImpl>& existing) {
            return existing->uuid == childImpl->uuid;
        });

        // Only add the child if it is not already a child
        if (it == ci.end())
        {
            impl->AddChild(childImpl, child);
            childImpl->parentImpl = impl.get();
            child->SetParent(this);
            child->Init();
        }
    }
}

void duin::GameObject::RemoveChildObject(std::shared_ptr<GameObject> child)
{
    if (child && impl)
    {
        if (auto childImpl = child->GetImpl())
        {
            impl->RemoveChild(childImpl);
            child->ResetParent();
        }
    }
}

void duin::GameObject::TransferChildObject(std::shared_ptr<GameObject> child, std::shared_ptr<GameObject> newParent)
{
    auto childKeeper = child;
    RemoveChildObject(child);
    newParent->AddChildObject(child);
}

std::vector<std::weak_ptr<duin::GameObject>> duin::GameObject::GetChildren()
{
    if (impl)
    {
        return impl->GetChildOwners();
    }
    return {};
}

size_t duin::GameObject::GetChildrenCount()
{
    if (impl)
    {
        return impl->GetChildrenCount();
    }
    return 0;
}

void duin::GameObject::SetParent(GameObject *parent)
{
    this->parent = parent;
}

duin::GameObject *duin::GameObject::GetParent()
{
    return parent;
}

void duin::GameObject::SetUUID(UUID newUUID)
{
    if (impl)
    {
        impl->uuid = newUUID;
    }
}

void duin::GameObject::ResetParent()
{
    this->parent = nullptr;
}

// Signal connection implementations
duin::UUID duin::GameObject::ConnectOnObjectReady(std::function<void()> callback)
{
    if (impl)
    {
        return impl->OnObjectReady.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectOnEvent(std::function<void(Event)> callback)
{
    if (impl)
    {
        return impl->OnObjectOnEvent.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectUpdate(std::function<void(double)> callback)
{
    if (impl)
    {
        return impl->OnObjectUpdate.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectPhysicsUpdate(std::function<void(double)> callback)
{
    if (impl)
    {
        return impl->OnObjectPhysicsUpdate.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectDraw(std::function<void()> callback)
{
    if (impl)
    {
        return impl->OnObjectDraw.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectDrawUI(std::function<void()> callback)
{
    if (impl)
    {
        return impl->OnObjectDrawUI.Connect(callback);
    }
    return UUID();
}

duin::UUID duin::GameObject::ConnectOnObjectDebug(std::function<void()> callback)
{
    if (impl)
    {
        return impl->OnObjectDebug.Connect(callback);
    }
    return UUID();
}

// Signal disconnection implementations
void duin::GameObject::DisconnectOnObjectReady(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectReady.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectOnEvent(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectOnEvent.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectUpdate(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectUpdate.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectPhysicsUpdate(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectPhysicsUpdate.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectDraw(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectDraw.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectDrawUI(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectDrawUI.Disconnect(uuid);
    }
}

void duin::GameObject::DisconnectOnObjectDebug(UUID uuid)
{
    if (impl)
    {
        impl->OnObjectDebug.Disconnect(uuid);
    }
}

// Connect all signals at once
duin::SignalConnections duin::GameObject::ConnectAllSignals(
    std::function<void()> onReady, std::function<void(Event)> onEvent, std::function<void(double)> onUpdate,
    std::function<void(double)> onPhysicsUpdate, std::function<void()> onDraw, std::function<void()> onDrawUI,
    std::function<void()> onDebug)
{
    SignalConnections connections;
    if (impl)
    {
        connections.onReady = impl->OnObjectReady.Connect(onReady);
        connections.onEvent = impl->OnObjectOnEvent.Connect(onEvent);
        connections.onUpdate = impl->OnObjectUpdate.Connect(onUpdate);
        connections.onPhysicsUpdate = impl->OnObjectPhysicsUpdate.Connect(onPhysicsUpdate);
        connections.onDraw = impl->OnObjectDraw.Connect(onDraw);
        connections.onDrawUI = impl->OnObjectDrawUI.Connect(onDrawUI);
        connections.onDebug = impl->OnObjectDebug.Connect(onDebug);
    }
    return connections;
}

// Disconnect all signals at once
void duin::GameObject::DisconnectAllSignals(const SignalConnections &connections)
{
    if (impl)
    {
        impl->OnObjectReady.Disconnect(connections.onReady);
        impl->OnObjectOnEvent.Disconnect(connections.onEvent);
        impl->OnObjectUpdate.Disconnect(connections.onUpdate);
        impl->OnObjectPhysicsUpdate.Disconnect(connections.onPhysicsUpdate);
        impl->OnObjectDraw.Disconnect(connections.onDraw);
        impl->OnObjectDrawUI.Disconnect(connections.onDrawUI);
        impl->OnObjectDebug.Disconnect(connections.onDebug);
    }
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
    if (impl)
    {
        impl->onEventEnabled = enable;
    }
}

void duin::GameObject::EnableUpdate(bool enable)
{
    if (impl)
    {
        impl->updateEnabled = enable;
    }
}

void duin::GameObject::EnablePhysicsUpdate(bool enable)
{
    if (impl)
    {
        impl->physicsUpdateEnabled = enable;
    }
}

void duin::GameObject::EnableDraw(bool enable)
{
    if (impl)
    {
        impl->drawEnabled = enable;
    }
}

void duin::GameObject::EnableDrawUI(bool enable)
{
    if (impl)
    {
        impl->drawUIEnabled = enable;
    }
}

void duin::GameObject::EnableDebug(bool enable)
{
    if (impl)
    {
        impl->debugEnabled = enable;
    }
}

void duin::GameObject::EnableChildren(bool enable)
{
    if (impl)
    {
        impl->childrenEnabled = enable;
    }
}

bool duin::GameObject::IsOnEventEnabled() const
{
    if (impl)
    {
        return impl->onEventEnabled;
    }
    return false;
}

bool duin::GameObject::IsUpdateEnabled() const
{
    if (impl)
    {
        return impl->updateEnabled;
    }
    return false;
}

bool duin::GameObject::IsPhysicsUpdateEnabled() const
{
    if (impl)
    {
        return impl->physicsUpdateEnabled;
    }
    return false;
}

bool duin::GameObject::IsDrawEnabled() const
{
    if (impl)
    {
        return impl->drawEnabled;
    }
    return false;
}

bool duin::GameObject::IsDrawUIEnabled() const
{
    if (impl)
    {
        return impl->drawUIEnabled;
    }
    return false;
}

bool duin::GameObject::IsDebugEnabled() const
{
    if (impl)
    {
        return impl->debugEnabled;
    }
    return false;
}

bool duin::GameObject::IsChildrenEnabled() const
{
    if (impl)
    {
        return impl->childrenEnabled;
    }
    return false;
}

duin::UUID duin::GameObject::GetUUID() const
{
    if (impl)
    {
        return impl->uuid;
    }
    return UUID();
}

std::shared_ptr<duin::GameObjectImpl> duin::GameObject::GetImpl() const
{
    return impl;
}

bool duin::GameObject::operator==(const GameObject &other) const
{
    return this->GetUUID() == other.GetUUID();
}

bool duin::GameObject::operator!=(const GameObject &other) const
{
    return this->GetUUID() != other.GetUUID();
}

bool duin::GameObject::IsValid()
{
    return impl != nullptr;
}

void duin::GameObject::ObjectReady()
{
    if (impl)
    {
        impl->DispatchReady();
    }
}

void duin::GameObject::ObjectOnEvent(Event event)
{
    if (impl)
    {
        impl->DispatchOnEvent(event);
    }
}

void duin::GameObject::ObjectUpdate(double delta)
{
    if (impl)
    {
        impl->DispatchUpdate(delta);
    }
}

void duin::GameObject::ObjectPhysicsUpdate(double delta)
{
    if (impl)
    {
        impl->DispatchPhysicsUpdate(delta);
    }
}

void duin::GameObject::ObjectDraw()
{
    if (impl)
    {
        impl->DispatchDraw();
    }
}

void duin::GameObject::ObjectDrawUI()
{
    if (impl)
    {
        impl->DispatchDrawUI();
    }
}

void duin::GameObject::ObjectDebug()
{
    if (impl)
    {
        impl->DispatchDebug();
    }
}

std::shared_ptr<duin::GameObjectImpl> duin::GameObject::EnsureImpl()
{
    if (!impl)
    {
        impl = std::make_shared<GameObjectImpl>(this);

        return impl;
    }
    return nullptr;
}
