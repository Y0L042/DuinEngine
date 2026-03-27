#pragma once

#include <Duin/ECS/ECSModule.h>
#include <Duin/ECS/GameWorld.h>

struct PlayerBlackboard
{
    duin::GameWorld* world = nullptr;
    duin::Entity player;
    duin::Entity cameraRoot;
    duin::Entity playerCamera;
};
