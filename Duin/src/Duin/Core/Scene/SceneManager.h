#pragma once

#include "Duin/Core/Core.h"

#include "Duin/Entity/Entity.h"
#include "Duin/Entity/Registry.h"

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
        
        void AddEntity(std::shared_ptr<Entity> entity);
        void RemoveEntity(std::shared_ptr<Entity> entity);

        std::vector<std::shared_ptr<Entity>> GetAllEntities();

        void ViewEntitiesWithComponents();

    private:
        std::shared_ptr<Registry> registry;
        std::vector<std::shared_ptr<Entity>> entityList;
    };
}