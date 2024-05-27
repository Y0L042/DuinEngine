#pragma once

#include <Duin.h>

#include "./MovementComponent.h"
#include "./TransformComponent.h"

struct TransformCMPManager
{
    static void Update(double delta) {
        auto view = Duin::Registry::GetRegistry().view<TransformCMP, MovementCMP>();
        for (auto [entity, transform, movement] : view.each()) 
        {
            transform.position += movement.velocity;
            transform.direction = movement.velocity.Normalized();
        }
    }
};