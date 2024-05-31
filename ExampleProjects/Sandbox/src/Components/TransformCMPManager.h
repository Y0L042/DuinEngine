#pragma once

#include <Duin.h>

#include "./MovementComponent.h"
#include "./TransformComponent.h"

struct TransformCMPManager
{
    static void Update(Duin::Registry* registry, double delta) {
        auto view = registry->View<TransformCMP, MovementCMP, Duin::QuadTreeComponent>();
        for (auto [entity, transform, movement, qtcomp] : view.each()) 
        {
            transform.position += movement.velocity;
            transform.direction = movement.velocity.Normalized();
            qtcomp.UpdatePosition(transform.position);
        }
    }
};