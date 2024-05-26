#pragma once

#include "./PlayerComponent.h"
#include "./MovementComponent.h"

struct PlayerCMPManager : entt::process<PlayerCMPManager, uint32_t>
{
    static void update(uint32_t delta) 
    {

    }
};