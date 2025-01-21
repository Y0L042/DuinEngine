#include "dnpch.h"
#include "GameStateMachine.h"

#define VALIDATE_SM_STACK ( !stateStack.empty() && stateStack.top() )

namespace duin {

GameState::GameState(GameStateMachine& owner)
    : owner(owner)
{}

const std::string GameState::GetName()
{
    return stateName;
}

}

namespace duin {
GameStateMachine::GameStateMachine()
{}

GameStateMachine::~GameStateMachine()
{}


void GameStateMachine::PopState()
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->Exit) {
        stateStack.top()->Exit();
    }
    stateStack.pop();
}

void GameStateMachine::FlushStack()
{
    if (!VALIDATE_SM_STACK) { return; } 

    while (!stateStack.empty()) {
        PopState();
    }
}

void GameStateMachine::ExecuteHandleInput()
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->HandleInput) {
        stateStack.top()->HandleInput();
    }
}

void GameStateMachine::ExecuteUpdate(double delta)
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->Update) {
        stateStack.top()->Update(delta);
    }
}

void GameStateMachine::ExecutePhysicsUpdate(double delta)
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->PhysicsUpdate) {
        stateStack.top()->PhysicsUpdate(delta);
    }
}

void GameStateMachine::ExecuteDraw()
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->Draw) {
        stateStack.top()->Draw();
    }
}

void GameStateMachine::ExecuteDrawUI()
{
    if (!VALIDATE_SM_STACK) { return; } 

    if (stateStack.empty()) { return; }
    if (stateStack.top()->DrawUI) {
        stateStack.top()->DrawUI();
    }
}

}
