#pragma once

#include "Duin/Core/Core.h"

#include "Duin/Entity/Entity.h"
#include "Duin/Entity/Registry.h"
#include "Duin/Entity/ComponentManager.h"

#include <entt/entt.hpp>

#include <memory>
#include <vector>


namespace Duin
{
    class DUIN_API SceneManager {
    public:
        SceneManager()
            : registry(std::make_shared<Registry>()) 
        {}

        ~SceneManager() = default; 

        std::shared_ptr<Entity> CreateEntity();

        std::shared_ptr<Registry> GetRegistry()
        {
            return registry;
        }
        
        template<typename CompMgr, typename... Args>
        CompMgr& CreateComponentManager(Args&&... args)
        {
            CompMgr* compMgr = new CompMgr(std::forward<Args>(args)...);
            return *compMgr;
        }

        template<typename... Component>
        auto ViewEntitiesWith()
        {
            return registry->View<Component...>();
        }

    private:
        std::shared_ptr<Registry> registry;
    };
}