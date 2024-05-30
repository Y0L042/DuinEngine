#pragma once

#include <Duin.h>

#include "TransformComponent.h"
#include "MovementComponent.h"

struct NewTransformCMPManager : public Duin::ComponentManager
{
    void Update(double delta) {
        auto view = registry->View<TransformCMP, MovementCMP>();
        for (auto [entity, transform, movement] : view.each())
        {
            transform.position += movement.velocity;
            transform.direction = movement.velocity.Normalized();
        }
    }
};