#include "dnpch.h"
#include "StateMachine.h"

#define VALIDATE_SM_STACK ( !stateStack.empty() && stateStack.top() )

namespace duin {

    State::State(StateMachine& owner)
        : owner(owner)
    {
        stateGameObject = std::make_shared<GameObject>();
    }

    void State::StateEnter()
    {
        Enter();
        stateGameObject->ObjectReady();
    }

    void State::StateOnEvent(Event e)
    {
        stateGameObject->ObjectOnEvent(e);
        OnEvent(e);
    }

    void State::StateUpdate(double delta)
    {
        stateGameObject->ObjectUpdate(delta);
        Update(delta);
    }

    void State::StatePhysicsUpdate(double delta)
    {
		stateGameObject->ObjectPhysicsUpdate(delta);
        PhysicsUpdate(delta);
    }

    void State::StateDraw()
    {
		stateGameObject->ObjectDraw();
        Draw();
    }

    void State::StateDrawUI()
    {
		stateGameObject->ObjectDrawUI();
        DrawUI();
    }

    void State::StateExit()
    {
        Exit();
    }

    void State::PopState()
    {
		owner.PopState();
    }

    void State::FlushStack()
    {
        owner.FlushStack();
    }

    void State::AddChild(std::shared_ptr<GameObject> child)
    {
        stateGameObject->AddChild(child);
    }

    void State::RemoveChild(std::shared_ptr<GameObject> child)
    {
        stateGameObject->RemoveChild(child);
    }

    UUID State::GetUUID()
    {
        return uuid;
    }

    StateMachine& State::GetOwner()
    {
        return owner;
    }

    const std::string State::GetName()
    {
        return stateName;
    }

}

namespace duin {
    StateMachine::StateMachine()
    {}

    StateMachine::~StateMachine()
    {}

    UUID StateMachine::GetUUID()
    {
        return uuid;
    }

    void StateMachine::PopState()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StateExit();
        stateStack.pop();
    }

    void StateMachine::FlushStack()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        while (!stateStack.empty()) {
            PopState();
        }
    }

    void StateMachine::ExecuteOnEvent(Event e)
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StateOnEvent(e);
    }

    void StateMachine::ExecuteUpdate(double delta)
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StateUpdate(delta);
    }

    void StateMachine::ExecutePhysicsUpdate(double delta)
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StatePhysicsUpdate(delta);
    }

    void StateMachine::ExecuteDraw()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StateDraw();
    }

    void StateMachine::ExecuteDrawUI()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        stateStack.top()->StateDrawUI();
    }

}
