#include "dnpch.h"
#include "SceneManager.h"

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    SceneManager& SceneManager::Get()
    {
        static SceneManager sceneManager;
        return sceneManager;
    }

    void SceneManager::ExecuteReady()
    {}

    void SceneManager::ExecuteOnEvent(InputEvent event)
    {}

    void SceneManager::ExecuteUpdate(double delta)
    {}

    void SceneManager::ExecutePhysicsUpdate(double delta)
    {}

    void SceneManager::ExecuteDraw()
    {}

    void SceneManager::ExecuteDrawUI()
    {}

    void SceneManager::ExecuteDebug()
    {}

    void SceneManager::RegisterReady(UUID uuid, std::function<void()> callback)
	{}

    void SceneManager::RegisterOnEvent(UUID uuid, std::function<void(InputEvent)> callback)
	{}

    void SceneManager::RegisterUpdate(UUID uuid, std::function<void(double)> callback)
	{}

    void SceneManager::RegisterPhysicsUpdate(UUID uuid, std::function<void(double)> callback)
	{}

    void SceneManager::RegisterDraw(UUID uuid, std::function<void()> callback)
	{}

    void SceneManager::RegisterDrawUI(UUID uuid, std::function<void()> callback)
	{}

    void SceneManager::RegisterDebug(UUID uuid, std::function<void()> callback)
	{}

}
