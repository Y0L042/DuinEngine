#pragma once

#include <Duin/ECS/ECSModule.h>

struct PlayerBlackboard
{
    duin::Entity* player = nullptr;
    duin::Entity* cameraRoot = nullptr;
    duin::Entity* playerCamera = nullptr;
};
