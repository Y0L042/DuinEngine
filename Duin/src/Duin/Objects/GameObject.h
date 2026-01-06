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
class GameObject : public std::enable_shared_from_this<GameObject>
{
  public:
    GameObject();
    virtual ~GameObject();

    template <typename T, typename... Args>
    std::shared_ptr<T> CreateChildObject(Args &&...args)
    {
        static_assert(std::is_base_of<GameObject, T>::value, "T must be a GameObject derived class");

        std::shared_ptr<T> derivedObject = std::make_shared<T>(std::forward<Args>(args)...);
        AddChildObject(derivedObject);

        return derivedObject;
    }

    template <typename T, typename... Args>
    std::shared_ptr<T> GetSharedPointer()
    {
        static_assert(std::is_base_of<GameObject, T>::value, "T must be a GameObject derived class");
        return std::static_pointer_cast<T>(shared_from_this());
    }

    void AddChildObject(std::shared_ptr<GameObject> child);
    void RemoveChildObject(std::shared_ptr<GameObject> child);
    void TransferChildObject(std::shared_ptr<GameObject> child, std::shared_ptr<GameObject> newParent);

    std::shared_ptr<GameObject> GetParent();

    // Signal connection functions
    UUID ConnectOnObjectReady(std::function<void()> callback);
    UUID ConnectOnObjectOnEvent(std::function<void(Event)> callback);
    UUID ConnectOnObjectUpdate(std::function<void(double)> callback);
    UUID ConnectOnObjectPhysicsUpdate(std::function<void(double)> callback);
    UUID ConnectOnObjectDraw(std::function<void()> callback);
    UUID ConnectOnObjectDrawUI(std::function<void()> callback);
    UUID ConnectOnObjectDebug(std::function<void()> callback);

    // Signal disconnection functions
    bool DisconnectOnObjectReady(UUID uuid);
    bool DisconnectOnObjectOnEvent(UUID uuid);
    bool DisconnectOnObjectUpdate(UUID uuid);
    bool DisconnectOnObjectPhysicsUpdate(UUID uuid);
    bool DisconnectOnObjectDraw(UUID uuid);
    bool DisconnectOnObjectDrawUI(UUID uuid);
    bool DisconnectOnObjectDebug(UUID uuid);

    SignalConnections ConnectAllSignals(std::function<void()> onReady, std::function<void(Event)> onEvent,
                                        std::function<void(double)> onUpdate,
                                        std::function<void(double)> onPhysicsUpdate, std::function<void()> onDraw,
                                        std::function<void()> onDrawUI, std::function<void()> onDebug);

    void DisconnectAllSignals(const SignalConnections &connections);

    virtual void Init();
    virtual void Ready();
    virtual void OnEvent(Event e);
    virtual void Update(double delta);
    virtual void PhysicsUpdate(double delta);
    virtual void Draw();
    virtual void DrawUI();
    virtual void Debug();

    void Enable(bool enable);
    void EnableOnEvent(bool enable);
    void EnableUpdate(bool enable);
    void EnablePhysicsUpdate(bool enable);
    void EnableDraw(bool enable);
    void EnableDrawUI(bool enable);
    void EnableDebug(bool enable);

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
};
} // namespace duin
