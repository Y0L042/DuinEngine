#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Events/Event.h"

#include <vector>
#include <memory>
#include <utility>

#define __TESTING__

namespace duin
{
class ObjectManager;
class GameObject : public std::enable_shared_from_this<GameObject>
{
  public:
    GameObject();
    virtual ~GameObject();

    template <typename T, typename ...Args>
    std::shared_ptr<T> CreateChildObject(Args&&... args)
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
    void SetParent(std::shared_ptr<GameObject> parent);
    std::shared_ptr<GameObject> GetParent();
    void ResetParent();

    virtual void Init();
    virtual void Ready();
    virtual void OnEvent(Event e);
    virtual void Update(double delta);
    virtual void PhysicsUpdate(double delta);
    virtual void Draw();
    virtual void DrawUI();
    virtual void Debug();

    UUID GetUUID() const;

    bool operator==(const GameObject &other) const;
    bool operator!=(const GameObject &other) const;

#ifndef __TESTING__
  protected:
#endif

    friend class ObjectManager;
    friend class Application;
    friend class StateMachine;
    friend class State;

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
};
} // namespace duin
