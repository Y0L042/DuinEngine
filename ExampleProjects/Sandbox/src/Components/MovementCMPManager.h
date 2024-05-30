#pragma once

#include <Duin.h>

#include "./PlayerInputComponent.h"
#include "./MovementComponent.h"
#include "./TransformComponent.h"

struct MovementCMPManager
{
    static void Update(Duin::Registry* registry, double delta) 
    {
        double SPEED = 100.0;
        auto view = registry->View<PlayerInputCMP, MovementCMP, TransformCMP>();
        for (auto [enity, pinput, movement, transform] : view.each())
        {
            movement.prevVelocity = movement.velocity;
            if (transform.position.x < 1)
            {
                movement.velocity.x = abs(movement.velocity.x) * 1;
            }
            if (transform.position.x > 800)
            {
                movement.velocity.x = abs(movement.velocity.x) * -1;
            }

            if (transform.position.y < 1)
            {
                movement.velocity.y = abs(movement.velocity.y) * 1;
            }
            if (transform.position.y > 600)
			{
                movement.velocity.y = abs(movement.velocity.y) * -1;
			}
            movement.velocity = movement.velocity.Normalized() * SPEED * delta;
            //movement.velocity = pinput.inputVec * (SPEED * delta);
        }
    }
};