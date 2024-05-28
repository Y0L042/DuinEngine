#pragma once

#include <Duin.h>

#include "./MovementComponent.h"
#include "./TransformComponent.h"

struct TransformCMPManager
{
    static void Update(Duin::Registry* registry, double delta) {
        auto view = registry->View<TransformCMP, MovementCMP>();
        for (auto [entity, transform, movement] : view.each()) 
        {
            transform.position += movement.velocity;
            transform.direction = movement.velocity.Normalized();
        }
    }
};