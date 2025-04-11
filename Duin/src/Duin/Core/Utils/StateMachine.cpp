#include "dnpch.h"
#include "StateMachine.h"

#define VALIDATE_SM_STACK ( !stateStack.empty() && stateStack.top() )

namespace duin {

    State::State(StateMachine& owner)
        : owner(owner)
    {}

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
        if (stateStack.top()->Exit) {
            stateStack.top()->Exit();
        }
        stateStack.pop();
    }

    void StateMachine::FlushStack()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        while (!stateStack.empty()) {
            PopState();
        }
    }

    void StateMachine::ExecuteHandleInput()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        if (stateStack.top()->HandleInput) {
            stateStack.top()->HandleInput();
        }
    }

    void StateMachine::ExecuteUpdate(double delta)
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        if (stateStack.top()->Update) {
            stateStack.top()->Update(delta);
        }
    }

    void StateMachine::ExecutePhysicsUpdate(double delta)
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        if (stateStack.top()->PhysicsUpdate) {
            stateStack.top()->PhysicsUpdate(delta);
        }
    }

    void StateMachine::ExecuteDraw()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        if (stateStack.top()->Draw) {
            stateStack.top()->Draw();
        }
    }

    void StateMachine::ExecuteDrawUI()
    {
        if (!VALIDATE_SM_STACK) { return; } 

        if (stateStack.empty()) { return; }
        if (stateStack.top()->DrawUI) {
            stateStack.top()->DrawUI();
        }
    }

}
