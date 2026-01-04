#pragma once

#include <functional>
#include "Duin/Core/Events/Event.h"

namespace duin
{
class UUID;
class ObjectManager
{
  public:
    static ObjectManager &Get();

    ObjectManager() = default;
    ~ObjectManager() = default;

    void ExecuteReady();
    void ExecuteOnEvent(InputEvent event);
    void ExecuteUpdate(double delta);
    void ExecutePhysicsUpdate(double delta);
    void ExecuteDraw();
    void ExecuteDrawUI();
    void ExecuteDebug();

    void RegisterReady(UUID uuid, std::function<void()> callback);
    void RegisterOnEvent(UUID uuid, std::function<void(InputEvent)> callback);
    void RegisterUpdate(UUID uuid, std::function<void(double)> callback);
    void RegisterPhysicsUpdate(UUID uuid, std::function<void(double)> callback);
    void RegisterDraw(UUID uuid, std::function<void()> callback);
    void RegisterDrawUI(UUID uuid, std::function<void()> callback);
    void RegisterDebug(UUID uuid, std::function<void()> callback);

  private:
};
} // namespace duin
