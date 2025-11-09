#include "dnpch.h"
#include "GameObject.h"

#include "ObjectManager.h"
#include "Duin/Core/Utils/StateMachine.h"

namespace duin {
    GameObject::GameObject()
    {

    }

    GameObject::~GameObject()
    {
    }

    void GameObject::AddChildObject(std::shared_ptr<GameObject> child)
    {
        if (child)
        {
            // Check if the child is already present by comparing UUIDs
            auto it = std::find_if(
                children.begin(), 
                children.end(),
                [&child](const std::shared_ptr<GameObject>& existingChild) 
                {
                    return existingChild->GetUUID() == child->GetUUID();
                }
            );

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

    void GameObject::RemoveChildObject(std::shared_ptr<GameObject> child)
    {
        if (child)
        {
            children.erase
            (
                std::remove(children.begin(), children.end(), child),
                children.end()
            );

            child->ResetParent();
        }
    }

    void GameObject::SetParent(std::shared_ptr<GameObject> parent)
    {
        this->parent = parent;
    }

    std::shared_ptr<GameObject> GameObject::GetParent()
    {
        return parent;
    }

    void GameObject::ResetParent()
    {
        this->parent = nullptr;
    }

    void GameObject::Init()
    {

    }

    void GameObject::Ready()
    {
    }

    void GameObject::OnEvent(Event event)
    {
    }

    void GameObject::Update(double delta)
    {
    }

    void GameObject::PhysicsUpdate(double delta)
    {
    }

    void GameObject::Draw()
    {
    }

    void GameObject::DrawUI()
    {
    }

    void GameObject::Debug()
    {
    }

    UUID GameObject::GetUUID()
    { 
        return uuid; 
    }

	bool GameObject::operator==(GameObject& other)
	{
		return this->GetUUID() == other.GetUUID();
	}

    void GameObject::ObjectReady()
	{
        for (auto& child : children) {
            if (child) child->ObjectReady();
        }
        Ready();
	}

    void GameObject::ObjectOnEvent(Event event)
	{
        for (auto& child : children) {
            if (child) child->ObjectOnEvent(event);
        }
        OnEvent(event);
	}

    void GameObject::ObjectUpdate(double delta)
	{
        for (auto& child : children) {
            if (child) child->ObjectUpdate(delta);
        }
        Update(delta);
	}

    void GameObject::ObjectPhysicsUpdate(double delta)
	{
        for (auto& child : children) {
            if (child) child->ObjectPhysicsUpdate(delta);
        }
        PhysicsUpdate(delta);
	}

    void GameObject::ObjectDraw()
	{
        for (auto& child : children) {
            if (child) child->ObjectDraw();
        }
        Draw();
	}

    void GameObject::ObjectDrawUI()
	{
        for (auto& child : children) {
            if (child) child->ObjectDrawUI();
        }
        DrawUI();
	}

    void GameObject::ObjectDebug()
	{
        for (auto& child : children) {
            if (child) child->ObjectDebug();
        }
        Debug();
	}

}
