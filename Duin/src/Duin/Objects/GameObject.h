/**
 * @file GameObject.h
 * @brief Scene graph node with lifecycle callbacks.
 * @ingroup Objects_GameObject
 */

#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Events/Event.h"
#include "Duin/Core/Signals/SignalsModule.h"

#include <vector>
#include <memory>
#include <utility>

#define __TESTING__

namespace duin
{

class GameStateMachine;
class ObjectManager;

/**
 * @class GameObject
 * @brief Base class for objects in the scene hierarchy.
 * @ingroup Objects_GameObject
 *
 * GameObjects form a parent-child tree structure. Each object receives
 * lifecycle callbacks (Init, Ready, Update, etc.) that propagate through
 * the hierarchy. Objects can connect to signals for decoupled communication.
 *
 * Lifecycle order:
 * 1. Init() - called once after construction
 * 2. Ready() - called after all children are ready
 * 3. Per-frame: OnEvent(), Update(), PhysicsUpdate(), Draw(), DrawUI(), Debug()
 *
 * Children are processed before parents for all callbacks.
 *
 * @code
 * class Player : public duin::GameObject {
 *     void Ready() override {
 *         // Setup code
 *     }
 *     void Update(double delta) override {
 *         // Per-frame logic
 *     }
 * };
 * @endcode
 */
class GameObject : public std::enable_shared_from_this<GameObject>
{
  public:
    GameObject();
    virtual ~GameObject();

    /**
     * @brief Creates and adds a child object.
     * @tparam T GameObject-derived type.
     * @tparam Args Constructor argument types.
     * @param args Arguments forwarded to T's constructor.
     * @return Shared pointer to the new child.
     */
    template <typename T, typename... Args>
    std::shared_ptr<T> CreateChildObject(Args &&...args)
    {
        static_assert(std::is_base_of<GameObject, T>::value, "T must be a GameObject derived class");

        std::shared_ptr<T> derivedObject = std::make_shared<T>(std::forward<Args>(args)...);
        AddChildObject(derivedObject);

        return derivedObject;
    }

    /**
     * @brief Returns this object as a shared pointer of derived type.
     * @tparam T The derived type to cast to.
     */
    template <typename T, typename... Args>
    std::shared_ptr<T> GetSharedPointer()
    {
        static_assert(std::is_base_of<GameObject, T>::value, "T must be a GameObject derived class");
        return std::static_pointer_cast<T>(shared_from_this());
    }

    /** @brief Adds an existing object as a child. */
    void AddChildObject(std::shared_ptr<GameObject> child);
    /** @brief Removes a child from this object. */
    void RemoveChildObject(std::shared_ptr<GameObject> child);
    /** @brief Moves a child to a new parent. */
    void TransferChildObject(std::shared_ptr<GameObject> child, std::shared_ptr<GameObject> newParent);

    /** @brief Returns the parent object, or nullptr if root. */
    std::shared_ptr<GameObject> GetParent();

    /**
     * @name Signal Connections
     * Connect callbacks to lifecycle signals.
     * @{
     */
    UUID ConnectOnObjectReady(std::function<void()> callback);
    UUID ConnectOnObjectOnEvent(std::function<void(Event)> callback);
    UUID ConnectOnObjectUpdate(std::function<void(double)> callback);
    UUID ConnectOnObjectPhysicsUpdate(std::function<void(double)> callback);
    UUID ConnectOnObjectDraw(std::function<void()> callback);
    UUID ConnectOnObjectDrawUI(std::function<void()> callback);
    UUID ConnectOnObjectDebug(std::function<void()> callback);
    /** @} */

    /**
     * @name Signal Disconnections
     * @{
     */
    bool DisconnectOnObjectReady(UUID uuid);
    bool DisconnectOnObjectOnEvent(UUID uuid);
    bool DisconnectOnObjectUpdate(UUID uuid);
    bool DisconnectOnObjectPhysicsUpdate(UUID uuid);
    bool DisconnectOnObjectDraw(UUID uuid);
    bool DisconnectOnObjectDrawUI(UUID uuid);
    bool DisconnectOnObjectDebug(UUID uuid);
    /** @} */

    /** @brief Connects all lifecycle signals at once. */
    SignalConnections ConnectAllSignals(std::function<void()> onReady, std::function<void(Event)> onEvent,
                                        std::function<void(double)> onUpdate,
                                        std::function<void(double)> onPhysicsUpdate, std::function<void()> onDraw,
                                        std::function<void()> onDrawUI, std::function<void()> onDebug);

    /** @brief Disconnects all signals from a SignalConnections bundle. */
    void DisconnectAllSignals(const SignalConnections &connections);

    /**
     * @name Lifecycle Callbacks
     * Override these in derived classes.
     * @{
     */
    virtual void Init();
    virtual void Ready();
    virtual void OnEvent(Event e);
    virtual void Update(double delta);
    virtual void PhysicsUpdate(double delta);
    virtual void Draw();
    virtual void DrawUI();
    virtual void Debug();
    /** @} */

    /**
     * @name Enable/Disable Callbacks
     * Control which callbacks are able to be called by parents.
     * @{
     */
    void Enable(bool enable);
    void EnableOnEvent(bool enable);
    void EnableUpdate(bool enable);
    void EnablePhysicsUpdate(bool enable);
    void EnableDraw(bool enable);
    void EnableDrawUI(bool enable);
    void EnableDebug(bool enable);
    void EnableChildren(bool enable);
    /** @} */

    /**
     * @name Query Callback State
     * Check which callbacks are currently enabled.
     * @{
     */
    bool IsOnEventEnabled() const;
    bool IsUpdateEnabled() const;
    bool IsPhysicsUpdateEnabled() const;
    bool IsDrawEnabled() const;
    bool IsDrawUIEnabled() const;
    bool IsDebugEnabled() const;
    bool IsChildrenEnabled() const;
    /** @} */

    /** @brief Returns this object's unique identifier. */
    UUID GetUUID() const;

    bool operator==(const GameObject &other) const;
    bool operator!=(const GameObject &other) const;

#ifndef __TESTING__
  protected:
#endif

    friend class ObjectManager;
    friend class Application;
    friend class GameStateMachine;
    friend class GameState;

    // Signals emit after main functions called: Child functions -> Main functions -> Signal functions
    Signal<> OnObjectReady;
    Signal<Event> OnObjectOnEvent;
    Signal<double> OnObjectUpdate;
    Signal<double> OnObjectPhysicsUpdate;
    Signal<> OnObjectDraw;
    Signal<> OnObjectDrawUI;
    Signal<> OnObjectDebug;

    void SetParent(std::shared_ptr<GameObject> parent);
    void ResetParent();

    // Runs children's functions first
    void ObjectReady();
    void ObjectOnEvent(Event event);
    void ObjectUpdate(double delta);
    void ObjectPhysicsUpdate(double delta);
    void ObjectDraw();
    void ObjectDrawUI();
    void ObjectDebug();

  private:
    UUID uuid;
    std::shared_ptr<GameObject> parent;
    std::vector<std::shared_ptr<GameObject>> children;
    bool onEventEnabled = true;
    bool updateEnabled = true;
    bool physicsUpdateEnabled = true;
    bool drawEnabled = true;
    bool drawUIEnabled = true;
    bool debugEnabled = true;
    bool childrenEnabled = true;
};
} // namespace duin
