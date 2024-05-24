#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Entity/Entity.h"
#include "Duin/Object/Object.h"
#include "Duin/Object/Node/Node2D/Node2D.h"

#include <entt/entt.hpp>

#include <memory>

namespace Duin
{
    class DUIN_API SceneManager {
    public:
        SceneManager()
            : rootNode(Node2D::Instantiate<Node2D>()), registry(std::make_unique<entt::registry>()) {}

    private:
        std::shared_ptr<Node2D> rootNode;
        std::unique_ptr<entt::registry> registry;
    };
}