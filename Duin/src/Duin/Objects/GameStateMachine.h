/**
 * @file GameStateMachine.h
 * @brief Stack-based state machine for application states.
 * @ingroup Objects_StateMachine
 *
 * Manages game states (menu, gameplay, pause, etc.) with push/pop/switch
 * operations. Each state has its own lifecycle and GameObject hierarchy.
 */

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

class GameStateMachine;
class GameState;

/**
 * @class GameStateMachine
 * @brief Stack-based state manager.
 * @ingroup Objects_StateMachine
 *
 * Manages a stack of GameState objects. The top state receives lifecycle
 * callbacks. States can be pushed (pausing current), popped, or switched.
 *
 * @code
 * stateMachine.PushState<MenuState>();
 * // Later...
 * stateMachine.SwitchState<GameplayState>();
 * @endcode
 */
class GameStateMachine : public GameObject
{
  public:
    GameStateMachine();
    ~GameStateMachine();

    UUID GetUUID();

    template <typename T>
    std::shared_ptr<T> CreateState()
    {
        auto newState = CreateChildObject<T>(*this);
        newState->Enable(false);

        return newState;
    }

    /**
     * @brief Returns the active state cast to type T.
     * @tparam T Expected state type.
     * @return Pointer to state, or nullptr if wrong type or empty.
     */
    template <typename T>
    T *GetActiveState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty())
        {
            if (dynamic_cast<T *>(stateStack.top().get()) == nullptr)
            {
                return nullptr;
            }
            return static_cast<T *>(stateStack.top().get());
        }
        return nullptr;
    }

    /**
     * @brief Exits current state and enters a new one.
     * @tparam T New state type.
     * @return Pointer to the new state.
     */
    template <typename T>
    T *SwitchState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty())
        {
            if (!stateStack.empty())
            {
                PopState();
            }
        }

        std::shared_ptr<T> newState = CreateChildObject<T>(*this);
        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    template <typename T>
    T *SwitchState(std::shared_ptr<T> newState)
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty())
        {
            if (!stateStack.empty())
            {
                PopState();
            }
        }

        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    /**
     * @brief Exits all states and enters a new one.
     * @tparam T New state type.
     * @return Pointer to the new state.
     */
    template <typename T>
    T *FlushAndSwitchState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        while (!stateStack.empty())
        {
            if (!stateStack.empty())
            {
                PopState();
            }
        }

        std::shared_ptr<T> newState = CreateChildObject<T>(*this);
        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    template <typename T>
    T *FlushAndSwitchState(std::shared_ptr<T> newState)
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        while (!stateStack.empty())
        {
            if (!stateStack.empty())
            {
                PopState();
            }
        }

        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    /**
     * @brief Pauses current state and pushes a new one.
     * @tparam T New state type.
     * @return Pointer to the new state.
     */
    template <typename T>
    T *PushState()
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty())
        {
            stateStack.top()->StateSetPause();
        }

        std::shared_ptr<T> newState = CreateChildObject<T>(*this);
        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    template <typename T>
    T *PushState(std::shared_ptr<T> newState)
    {
        static_assert(std::is_base_of<GameState, T>::value, "T must derive from GameState");
        if (!stateStack.empty())
        {
            stateStack.top()->StateSetPause();
        }

        newState->Enable(false);
        T *ptr = newState.get();
        stateStack.push(newState);
        stateStack.top()->StateEnter();

        return ptr;
    }

    /** @brief Pops the current state and resumes the previous one. */
    void PopState();

    /** @brief Exits and removes all states from the stack. */
    void FlushStack();
    /** @brief Returns reference to the internal state stack. */
    std::stack<std::shared_ptr<GameState>> &GetStateStack();

    virtual void Init() override;
    virtual void Ready() override;
    virtual void OnEvent(Event e) override;
    virtual void Update(double delta) override;
    virtual void PhysicsUpdate(double delta) override;
    virtual void Draw() override;
    virtual void DrawUI() override;
    virtual void Debug() override;

  private:
    std::stack<std::shared_ptr<GameState>> stateStack;
};

/**
 * @class GameState
 * @brief Base class for application states.
 * @ingroup Objects_StateMachine
 *
 * Represents a single state in the state machine (e.g., menu, gameplay).
 * Override lifecycle methods to implement state behavior. Each state has
 * its own GameObject hierarchy via stateGameObject.
 *
 * @code
 * class MenuState : public duin::GameState {
 * public:
 *     MenuState(GameStateMachine& owner) : GameState(owner) {}
 *     void Enter() override { // Setup menu UI }
 *     void Update(double delta) override { // Handle menu input }
 *     void Exit() override { // Cleanup }
 * };
 * @endcode
 */
class GameState : public GameObject
{
  public:
    GameState(GameStateMachine &owner);
    virtual ~GameState() = default;

    /** @name Internal State Methods (called by GameStateMachine)
     * @{
     */
    void StateEnter();
    void StateOnEvent(Event e);
    void StateUpdate(double delta);
    void StatePhysicsUpdate(double delta);
    void StateDraw();
    void StateDrawUI();
    void StateExit();
    void StateSetPause();
    void StateSetUnpause();
    /** @} */

    /**
     * @name Lifecycle Callbacks
     * Override these in derived classes.
     * @{
     */
    virtual void Enter() {};
    virtual void OnEvent(Event e) override {};
    virtual void Update(double delta) override {};
    virtual void PhysicsUpdate(double delta) override {};
    virtual void Draw() override {};
    virtual void DrawUI() override {};
    virtual void Exit() {};
    virtual void SetPause() {};
    virtual void SetUnpause() {};

    void Init() override final {};
    void Ready() override final {};
    void Debug() override final {};

    /** @} */

    template <typename T>
    T *SwitchState()
    {
        return owner.template SwitchState<T>();
    }

    template <typename T>
    T *FlushAndSwitchState()
    {
        return owner.template FlushAndSwitchState<T>();
    }

    template <typename T>
    T *PushState()
    {
        return owner.template PushState<T>();
    }

    void PopState();
    void FlushStack();

    bool IsEqualTo(GameState *other);

    // Signal connection functions
    UUID ConnectOnStateEnter(std::function<void()> callback);
    UUID ConnectOnStateOnEvent(std::function<void(Event)> callback);
    UUID ConnectOnStateUpdate(std::function<void(double)> callback);
    UUID ConnectOnStatePhysicsUpdate(std::function<void(double)> callback);
    UUID ConnectOnStateDraw(std::function<void()> callback);
    UUID ConnectOnStateDrawUI(std::function<void()> callback);
    UUID ConnectOnStateExit(std::function<void()> callback);

    // Signal disconnection functions
    bool DisconnectOnStateEnter(UUID uuid);
    bool DisconnectOnStateOnEvent(UUID uuid);
    bool DisconnectOnStateUpdate(UUID uuid);
    bool DisconnectOnStatePhysicsUpdate(UUID uuid);
    bool DisconnectOnStateDraw(UUID uuid);
    bool DisconnectOnStateDrawUI(UUID uuid);
    bool DisconnectOnStateExit(UUID uuid);

    SignalConnections ConnectAllSignals(std::function<void()> onEnter, std::function<void(Event)> onEvent,
                                        std::function<void(double)> onUpdate,
                                        std::function<void(double)> onPhysicsUpdate, std::function<void()> onDraw,
                                        std::function<void()> onDrawUI, std::function<void()> onExit);

    void DisconnectAllSignals(const SignalConnections &connections);

    void Enable(bool enable);
    void EnableOnEvent(bool enable);
    void EnableUpdate(bool enable);
    void EnablePhysicsUpdate(bool enable);
    void EnableDraw(bool enable);
    void EnableDrawUI(bool enable);

    UUID GetUUID();
    GameStateMachine &GetOwner();

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
    UUID uuid;
    std::string stateName;
    GameStateMachine &owner;

    // Signals emit after main functions called
    Signal<> OnStateEnter;
    Signal<Event> OnStateOnEvent;
    Signal<double> OnStateUpdate;
    Signal<double> OnStatePhysicsUpdate;
    Signal<> OnStateDraw;
    Signal<> OnStateDrawUI;
    Signal<> OnStateExit;

  private:
    bool onEventEnabled = true;
    bool updateEnabled = true;
    bool physicsUpdateEnabled = true;
    bool drawEnabled = true;
    bool drawUIEnabled = true;
};

} // namespace duin
