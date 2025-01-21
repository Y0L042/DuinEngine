#pragma once

#include <stack>
#include <string>
#include <memory>
#include <functional>

#include "Duin/Core/Utils/UUID.h"



namespace duin {
class GameStateMachine;
class GameState
{
public:
    UUID uuid;
    GameStateMachine& owner;
    std::function<void()> Enter;
    std::function<void()> HandleInput;
    std::function<void(double)> Update;
    std::function<void(double)> PhysicsUpdate;
    std::function<void()> Draw;
    std::function<void()> DrawUI;
    std::function<void()> Exit;



    GameState(GameStateMachine& owner);
    virtual ~GameState() = default;

    const std::string GetName();

    bool operator==(const GameState &state) const
    {
        return this->uuid == state.uuid;
    }

    bool operator!=(const GameState &state) const
    {
        return this->uuid != state.uuid;
    }

protected:
    std::string stateName;

};



class GameStateMachine
{
public:
    UUID uuid;



    GameStateMachine();
    ~GameStateMachine();

    template<typename T>
    void SwitchState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty()) {
            if (stateStack.top()->Exit) {
                stateStack.top()->Exit();
            }
            if (!stateStack.empty()) {
                stateStack.pop();
            }
        }
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        if (stateStack.top()->Enter) {
            stateStack.top()->Enter();
        }
    }

    template<typename T>
    void FlushAndSwitchState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        while (!stateStack.empty()) {
            if (stateStack.top()->Exit) {
                stateStack.top()->Exit();
            }
            if (!stateStack.empty()) {
                stateStack.pop();
            }
        }
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        if (stateStack.top()->Enter) {
            stateStack.top()->Enter();
        }
    }

    template<typename T>
    void PushState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        if (stateStack.top()->Enter) {
            stateStack.top()->Enter();
        }
    }

    void PopState();
    void FlushStack();
    
    void ExecuteHandleInput();
    void ExecuteUpdate(double delta);
    void ExecutePhysicsUpdate(double delta);
    void ExecuteDraw();
    void ExecuteDrawUI();

private:
    std::stack<std::unique_ptr<GameState>> stateStack;

};



}
