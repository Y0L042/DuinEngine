#include "dnpch.h"

#include "Node.h"

namespace Duin
{
    Node::Node()
    {
        nodeAlias = std::to_string(GetUUID());
    }

    Node::~Node()
    {
    }

    void Node::Initialize()
    {
    }

    void Node::Ready()
    {
    }

    void Node::HandleInput(InputEvent e)
    {
    }

    void Node::Update(double rDelta)
    {
    }

    void Node::PhysicsUpdate(double pDelta)
    {
    }

    void Node::Draw()
    {
    }

    bool Node::SceneManagerValidCheck()
    {
        if (sceneManager == nullptr)
        {
            DN_CORE_WARN("SceneManager is not valid!");
            return false;
        }
        return true;
    }

    void Node::SetNodeAlias(std::string alias)
    {
        nodeAlias = alias;
    }

    std::string Node::GetNodeAlias()
    {
        return nodeAlias;
    }

    void Node::AddChild(std::shared_ptr<Node> child)
    {
        if (child)
        {
            // Check if the child is already present by comparing UUIDs
            auto it = std::find_if(
                childNodesList.begin(), 
                childNodesList.end(),
                [&child](const std::shared_ptr<Node>& existingChild) 
                {
                    return existingChild->GetUUID() == child->GetUUID();
                }
            );

            // Only add the child if it is not already a child
            if (it == childNodesList.end())
            {
                childNodesList.push_back(child);
                std::shared_ptr<Node> selfPtr = sceneManager->GetNodePtr(GetUUID());
                child->SetParent(selfPtr);
                child->ProcessOnInitialize();
            }
        }
    }

    void Node::RemoveChild(std::shared_ptr<Node> child)
    {
        if (child)
        {
            childNodesList.erase
            (
                std::remove(childNodesList.begin(), childNodesList.end(), child),
                childNodesList.end()
            );

            child->ResetParent();
        }
    }

    void Node::SetSceneManager(SceneManager* manager)
    {
        sceneManager = manager;
    }

    void Node::ProcessOnInitialize()
    {
        Initialize();
    }

    void Node::ProcessOnReady()
    {
        for (auto& child : childNodesList)
        {
            child->ProcessOnReady();
        }
        Ready();
    }

    void Node::ProcessOnHandleInput(InputEvent e)
    {
        for (auto& child : childNodesList)
        {
            child->ProcessOnHandleInput(e);
        }
        HandleInput(e);
    }

    void Node::ProcessOnUpdate(double rDelta)
    {
        for (auto& child : childNodesList)
        {
            child->ProcessOnUpdate(rDelta);
        }
        Update(rDelta);
    }

    void Node::ProcessOnPhysicsUpdate(double pDelta)
    {
        for (auto& child : childNodesList)
        {
            child->ProcessOnPhysicsUpdate(pDelta);
        }
        PhysicsUpdate(pDelta);
    }

    void Node::ProcessOnDraw()
    {
        for (auto& child : childNodesList)
        {
            child->ProcessOnDraw();
        }
        Draw();
    }

}