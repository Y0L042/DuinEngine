#include "dnpch.h"

#include "Node.h"

namespace Duin
{
    Node::Node()
    {
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

    void Node::AddChild(std::shared_ptr<Node> child)
    {
        if (child)
        {
            children.push_back(child);
            std::shared_ptr<Node> selfPtr = ObjectManager::GetNodePtr(GetUUID());
            child->SetParent(selfPtr);
            child->RequestSignalConnections();
            child->ProcessInitialize();
        }
    }

    void Node::RemoveChild(std::shared_ptr<Node> child)
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

    void Node::SetSignalMenu()
    {
        signalMenu["READY"] = new SignalWrapper<>(onReady);
        signalMenu["HANDLEINPUT"] = new SignalWrapper<InputEvent>(onHandleInput);
        signalMenu["UPDATE"] = new SignalWrapper<double>(onUpdate);
        signalMenu["PHYSICSUPDATE"] = new SignalWrapper<double>(onPhysicsUpdate);
        signalMenu["DRAW"] = new SignalWrapper<>(onDraw);
    }

    void Node::RequestSignalConnections()
    {
        if (auto parentPtr = std::dynamic_pointer_cast<Node>(GetParent()))
        {
            std::map<std::string, std::any> callbacks = {
                {"READY", std::function<void()>([this]() { this->ProcessOnReady(); })},
                {"HANDLEINPUT", std::function<void(InputEvent)>([this](InputEvent e) { this->ProcessOnHandleInput(e); })},
                {"UPDATE", std::function<void(double)>([this](double rDelta) { this->ProcessOnUpdate(rDelta); })},
                {"PHYSICSUPDATE", std::function<void(double)>([this](double pDelta) { this->ProcessOnPhysicsUpdate(pDelta); })},
                {"DRAW", std::function<void()>([this]() { this->ProcessOnDraw(); })},
            };
            parentPtr->ConnectSignalsToCallbacks(parentPtr->GetSignalMenu(), callbacks);
        }
    }

    std::map<std::string, SignalBase*>& Node::GetSignalMenu()
    {
        return signalMenu;
    }

    void Node::ConnectSignalsToCallbacks(
        const std::map<std::string, SignalBase*>& signalMenu,
        const std::map<std::string, std::any>& callbacks
    )
    {
        for (const auto& callbackPair : callbacks)
        {
            const std::string& signalName = callbackPair.first;
            auto it = signalMenu.find(signalName);
            if (it != signalMenu.end())
            {
                it->second->ConnectCallback(callbackPair.second);
                std::cout << signalName << " has been connected. " << GetUUID() << "\n";
            }
            else
            {
                std::cerr << "Warning: Parent does not have signal: " << signalName << std::endl;
            }
        }
    }

    void Node::DisconnectSignalsFromCallbacks(
        const std::map<std::string, SignalBase*>& signalMenu,
        const std::vector<std::string>& signalNames
    )
    {
        for (const auto& signalName : signalNames)
        {
            auto it = signalMenu.find(signalName);
            if (it != signalMenu.end())
            {
                it->second->DisconnectAll();
            }
            else
            {
                std::cerr << "Warning: Parent does not have signal: " << signalName << std::endl;
            }
        }
    }

    void Node::ProcessInitialize()
    {
        SetSignalMenu();
        Initialize();
    }

    void Node::ProcessOnReady()
    {
        onReady.Emit();
        for (auto& child : children)
        {
            child->ProcessOnReady();
        }
        Ready();
    }

    void Node::ProcessOnHandleInput(InputEvent e)
    {
        onHandleInput.Emit(e);
        for (auto& child : children)
        {
            child->ProcessOnHandleInput(e);
        }
        HandleInput(e);
    }

    void Node::ProcessOnUpdate(double rDelta)
    {
        onUpdate.Emit(rDelta);
        for (auto& child : children)
        {
            child->ProcessOnUpdate(rDelta);
        }
        Update(rDelta);
    }

    void Node::ProcessOnPhysicsUpdate(double pDelta)
    {
        onPhysicsUpdate.Emit(pDelta);
        for (auto& child : children)
        {
            child->ProcessOnPhysicsUpdate(pDelta);
        }
        PhysicsUpdate(pDelta);
    }

    void Node::ProcessOnDraw()
    {
        onDraw.Emit();
        for (auto& child : children)
        {
            child->ProcessOnDraw();
        }
        Draw();
    }
}