#pragma once

#include <Duin.h>

#include "./PlayerInputComponent.h"
#include "./MovementComponent.h"

struct MovementCMPManager
{
    static void Update(Duin::Registry* registry, double delta) 
    {
        float SPEED = 100.0f;
        auto view = registry->View<PlayerInputCMP, MovementCMP>();
        for (auto [enity, pinput, movement] : view.each())
        {
            movement.prevVelocity = movement.velocity;
            movement.velocity = pinput.inputVec * (SPEED * delta);
            //DN_INFO("MovementManager vel: {} , {}", movement.velocity.x, movement.velocity.y);
        }
    }
};