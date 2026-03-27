#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Events/Event.h"
#include "Duin/Core/Signals/SignalsModule.h"

#include <vector>
#include <memory>
#include <algorithm>

namespace duin
{

class GameObject;

class GameObjectImpl : public std::enable_shared_from_this<GameObjectImpl>
{
  public:
    GameObjectImpl(GameObject *owner);
    ~GameObjectImpl();

    void AddChild(std::shared_ptr<GameObjectImpl> childImpl, std::shared_ptr<GameObject> childOwner);
    void RemoveChild(const std::shared_ptr<GameObjectImpl> &childImpl);

    std::vector<std::weak_ptr<GameObject>> GetChildOwners();
    size_t GetChildrenCount();

    // Lifecycle dispatch — iterates children, emits signals, calls owner virtual methods
    void DispatchReady();
    void DispatchOnEvent(Event event);
    void DispatchUpdate(double delta);
    void DispatchPhysicsUpdate(double delta);
    void DispatchDraw();
    void DispatchDrawUI();
    void DispatchDebug();

    UUID uuid;
    GameObject *owner = nullptr;
    GameObjectImpl *parentImpl = nullptr;

    std::vector<std::shared_ptr<GameObjectImpl>> childImpls;
    std::vector<std::shared_ptr<GameObject>> childOwners;

    Signal<> OnObjectReady;
    Signal<Event> OnObjectOnEvent;
    Signal<double> OnObjectUpdate;
    Signal<double> OnObjectPhysicsUpdate;
    Signal<> OnObjectDraw;
    Signal<> OnObjectDrawUI;
    Signal<> OnObjectDebug;

    bool onEventEnabled = true;
    bool updateEnabled = true;
    bool physicsUpdateEnabled = true;
    bool drawEnabled = true;
    bool drawUIEnabled = true;
    bool debugEnabled = true;
    bool childrenEnabled = true;
};

} // namespace duin
