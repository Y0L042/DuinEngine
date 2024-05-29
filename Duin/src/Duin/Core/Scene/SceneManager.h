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

        Entity& CreateEntity();

        template<typename... Component>
        auto ViewEntitiesWith()
        {
            return registry->View<Component...>();
        }

    private:
        std::shared_ptr<Registry> registry;
    };
}