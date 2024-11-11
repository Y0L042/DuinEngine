#include "dnpch.h"
#include "GameStateMachine.h"

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
    if (stateStack.empty()) { return; }
    if (stateStack.top()->Exit) {
        stateStack.top()->Exit();
    }
    stateStack.pop();
}

void GameStateMachine::ExecuteHandleInput()
{
    if (stateStack.empty()) { return; }
    if (stateStack.top()->HandleInput) {
        stateStack.top()->HandleInput();
    }
}

void GameStateMachine::ExecuteUpdate(double delta)
{
    if (stateStack.empty()) { return; }
    if (stateStack.top()->Update) {
        stateStack.top()->Update(delta);
    }
}

void GameStateMachine::ExecutePhysicsUpdate(double delta)
{
    if (stateStack.empty()) { return; }
    if (stateStack.top()->PhysicsUpdate) {
        stateStack.top()->PhysicsUpdate(delta);
    }
}

void GameStateMachine::ExecuteDraw()
{
    if (stateStack.empty()) { return; }
    if (stateStack.top()->Draw) {
        stateStack.top()->Draw();
    }
}

void GameStateMachine::ExecuteDrawUI()
{
    if (stateStack.empty()) { return; }
    if (stateStack.top()->DrawUI) {
        stateStack.top()->DrawUI();
    }
}



}
