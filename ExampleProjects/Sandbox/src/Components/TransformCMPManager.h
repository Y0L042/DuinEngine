#pragma once

#include <Duin.h>

#include "./MovementComponent.h"
#include "./TransformComponent.h"

struct TransformCMPManager : entt::process<TransformCMPManager, uint32_t>
{
    static void update(uint32_t delta) {
        auto view = Duin::Registry::GetRegistry().view<TransformCMP, MovementCMP>();
        for (auto [entity, transform, movement] : view.each()) 
        {
            transform.position += movement.velocity;
            transform.direction = movement.velocity.Normalized();
        }
    }
};