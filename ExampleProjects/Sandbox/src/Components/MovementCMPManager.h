#pragma once

#include <Duin.h>

#include "./MovementComponent.h"

struct MovementCMPManager : entt::process<MovementCMPManager, uint32_t>
{
    static void update(uint32_t delta) 
    {
        auto view = Duin::Registry::GetRegistry().view<MovementCMP>();
        for (auto [enity, movement] : view.each())
        {
            float SPEED = 100.0f;
            movement.prevVelocity = movement.velocity;
            movement.velocity *= (SPEED * delta);
            DN_INFO("MovementManager vel: {} , {}", movement.velocity.x, movement.velocity.y);
        }
    }
};