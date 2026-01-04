#pragma once

#include <stack>
#include <string>
#include <memory>
#include <functional>

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Events/Event.h"
#include "Duin/Objects/GameObject.h"

#include <assert.h>

namespace duin
{
class GameObject;
class StateMachine;
class State;

class StateMachine
{
  public:
    StateMachine();
    ~StateMachine();

    template <typename T>
    T *GetActiveState()
    {
        static_assert(std::is_base_of<State, T>::value, "T must derive from State");
        if (!stateStack.empty())
        {
            if (dynamic_cast<T *>(stateStack.top().get()) == nullptr) { return nullptr; }; // Ensure top state is of type T
            return static_cast<T *>(stateStack.top().get());
        }
        return nullptr;
    }

    template <typename T>
    void SwitchState()
    {
        static_assert(std::is_base_of<State, T>::value, "T must derive from State");
        if (!stateStack.empty())
        {
            stateStack.top()->StateExit();
            if (!stateStack.empty())
            {
                stateStack.pop();
            }
        }
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        stateStack.top()->StateEnter();
    }

    template <typename T>
    void FlushAndSwitchState()
    {
        static_assert(std::is_base_of<State, T>::value, "T must derive from State");
        while (!stateStack.empty())
        {
            stateStack.top()->StateExit();
            if (!stateStack.empty())
            {
                stateStack.pop();
            }
        }
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        stateStack.top()->StateEnter();
    }

    template <typename T>
    T* PushState()
    {
        static_assert(std::is_base_of<State, T>::value, "T must derive from State");
        auto newState = std::make_unique<T>(*this);
        stateStack.emplace(std::move(newState));
        stateStack.top()->StateEnter();

        return newState.get();
    }

    UUID GetUUID();

    void PopState();
    void FlushStack();

    void ExecuteOnEvent(Event e);
    void ExecuteUpdate(double delta);
    void ExecutePhysicsUpdate(double delta);
    void ExecuteDraw();
    void ExecuteDrawUI();

  private:
    UUID uuid;
    std::stack<std::unique_ptr<State>> stateStack;
};

class State
{
  public:
    State(StateMachine &owner);
    virtual ~State() = default;

    void StateEnter();
    void StateOnEvent(Event e);
    void StateUpdate(double delta);
    void StatePhysicsUpdate(double delta);
    void StateDraw();
    void StateDrawUI();
    void StateExit();

    virtual void Enter() {};
    virtual void OnEvent(Event e) {};
    virtual void Update(double delta) {};
    virtual void PhysicsUpdate(double delta) {};
    virtual void Draw() {};
    virtual void DrawUI() {};
    virtual void Exit() {};

    template <typename T>
    void SwitchState()
    {
        owner.template SwitchState<T>();
    }

    template <typename T>
    void FlushAndSwitchState()
    {
        owner.template FlushAndSwitchState<T>();
    }

    template <typename T>
    void PushState()
    {
        owner.template PushState<T>();
    }

    void PopState();
    void FlushStack();

    template <typename T, typename... Args>
    std::shared_ptr<T> CreateChildObject(Args... args)
    {
        return stateGameObject->template CreateChildObject<T>(std::forward<Args>(args)...);
    }
    void AddChildObject(std::shared_ptr<GameObject> child);
    void RemoveChildObject(std::shared_ptr<GameObject> child);

    UUID GetUUID();
    StateMachine &GetOwner();

    const std::string GetName();

    bool operator==(const State &state) const
    {
        return this->uuid == state.uuid;
    }

    bool operator!=(const State &state) const
    {
        return this->uuid != state.uuid;
    }

  protected:
    UUID uuid;
    std::string stateName;
    StateMachine &owner;
    std::shared_ptr<GameObject> stateGameObject;
};

} // namespace duin
