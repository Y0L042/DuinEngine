#pragma once

#include "Entity.h"
#include "World.h"

namespace duin
{

// ========== Template Method Implementations that require World definition ==========

template <typename Func>
Entity &Entity::WithScope(Func &&func)
{
    // Use flecs scope builder pattern
    if (world)
    {
        flecsEntity.scope([&]() {
            func(*this);
        });
    }
    else
    {
        func(*this);
    }
    return *this;
}

template <typename Func>
Entity &Entity::WithComponent(Func &&func)
{
    // Use flecs with builder pattern
    if (world)
    {
        flecsEntity.with([&]() {
            func(*this);
        });
    }
    else
    {
        func(*this);
    }
    return *this;
}

template <typename Relationship, typename Func>
void Entity::ForEachTarget(Func &&func) const
{
    if (!world)
    {
        return;
    }
    
    // Get the relationship component ID through the world
    Entity relationshipEntity = world->Component<Relationship>();
    flecs::id_t relationId = relationshipEntity.GetID();
    
    // Iterate through the entity's type to find pair relationships
    flecsEntity.each([&](flecs::id id) {
        if (id.is_pair() && id.first() == relationId)
        {
            Entity targetEntity(id.second(), world);
            func(targetEntity);
        }
    });
}

} // namespace duin
