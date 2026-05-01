#include "dnpch.h"
#include "GameObjectImpl.h"
#include "GameObject.h"
#include "Duin/Core/Debug/DNLog.h"

duin::GameObjectImpl::GameObjectImpl(GameObject *owner) : owner(owner), uuid(UUID())
{
    DN_CORE_INFO("Construct GameObjectImpl");
}

duin::GameObjectImpl::~GameObjectImpl()
{
}

void duin::GameObjectImpl::Initialize()
{
    isInitialized = true;
}

bool duin::GameObjectImpl::IsInitialized() const
{
    return isInitialized;
}

bool duin::GameObjectImpl::IsReady() const
{
    return isReady;
}

void duin::GameObjectImpl::AddChild(std::shared_ptr<GameObjectImpl> childImpl, std::shared_ptr<GameObject> childOwner)
{
    childImpls.push_back(std::move(childImpl));
    childOwners.push_back(std::move(childOwner));
}

void duin::GameObjectImpl::RemoveChild(const std::shared_ptr<GameObjectImpl> &childImpl)
{
    for (size_t i = 0; i < childImpls.size(); ++i)
    {
        if (childImpls[i] == childImpl)
        {
            childImpls.erase(childImpls.begin() + i);
            childOwners.erase(childOwners.begin() + i);
            childImpl->parentImpl = nullptr;
            return;
        }
    }
}

std::vector<std::weak_ptr<duin::GameObject>> duin::GameObjectImpl::GetChildOwners()
{
    std::vector<std::weak_ptr<GameObject>> result(childOwners.begin(), childOwners.end());
    return result;
}

size_t duin::GameObjectImpl::GetChildrenCount()
{
    return childImpls.size();
}

void duin::GameObjectImpl::DispatchReady()
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child)
            {
                child->DispatchReady();
            }
        }
    }

    OnObjectReady.Emit();
    if (owner)
        owner->Ready();

    isReady = true;
}

void duin::GameObjectImpl::DispatchOnEvent(Event event)
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->onEventEnabled)
            {
                child->DispatchOnEvent(event);
            }
        }
    }

    OnObjectOnEvent.Emit(event);
    if (owner)
        owner->OnEvent(event);
}

void duin::GameObjectImpl::DispatchUpdate(double delta)
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->updateEnabled)
            {
                child->DispatchUpdate(delta);
            }
        }
    }

    OnObjectUpdate.Emit(delta);
    if (owner)
        owner->Update(delta);
}

void duin::GameObjectImpl::DispatchPhysicsUpdate(double delta)
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->physicsUpdateEnabled)
            {
                child->DispatchPhysicsUpdate(delta);
            }
        }
    }

    OnObjectPhysicsUpdate.Emit(delta);
    if (owner)
        owner->PhysicsUpdate(delta);
}

void duin::GameObjectImpl::DispatchDraw()
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->drawEnabled)
            {
                child->DispatchDraw();
            }
        }
    }

    OnObjectDraw.Emit();
    if (owner)
        owner->Draw();
}

void duin::GameObjectImpl::DispatchDrawUI()
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->drawUIEnabled)
            {
                child->DispatchDrawUI();
            }
        }
    }

    OnObjectDrawUI.Emit();
    if (owner)
        owner->DrawUI();
}

void duin::GameObjectImpl::DispatchDebug()
{
    if (childrenEnabled)
    {
        for (auto &child : childImpls)
        {
            if (child && child->debugEnabled)
            {
                child->DispatchDebug();
            }
        }
    }

    OnObjectDebug.Emit();
    if (owner)
        owner->Debug();
}
