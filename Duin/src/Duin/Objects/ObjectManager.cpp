#include "dnpch.h"
#include "ObjectManager.h"

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    ObjectManager& ObjectManager::Get()
    {
        static ObjectManager sceneManager;
        return sceneManager;
    }

    void ObjectManager::ExecuteReady()
    {}

    void ObjectManager::ExecuteOnEvent(InputEvent event)
    {}

    void ObjectManager::ExecuteUpdate(double delta)
    {}

    void ObjectManager::ExecutePhysicsUpdate(double delta)
    {}

    void ObjectManager::ExecuteDraw()
    {}

    void ObjectManager::ExecuteDrawUI()
    {}

    void ObjectManager::ExecuteDebug()
    {}

    void ObjectManager::RegisterReady(UUID uuid, std::function<void()> callback)
	{}

    void ObjectManager::RegisterOnEvent(UUID uuid, std::function<void(InputEvent)> callback)
	{}

    void ObjectManager::RegisterUpdate(UUID uuid, std::function<void(double)> callback)
	{}

    void ObjectManager::RegisterPhysicsUpdate(UUID uuid, std::function<void(double)> callback)
	{}

    void ObjectManager::RegisterDraw(UUID uuid, std::function<void()> callback)
	{}

    void ObjectManager::RegisterDrawUI(UUID uuid, std::function<void()> callback)
	{}

    void ObjectManager::RegisterDebug(UUID uuid, std::function<void()> callback)
	{}

}
