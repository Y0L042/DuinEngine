#pragma once

#include <Duin.h>

struct PlayerMovementInputVec3 {
    duin::Vector3 value;
};

struct GravityComponent {
    duin::Vector3 value = { 0.0f, -9.81f, 0.0f };
};

struct DebugCameraTarget {
    duin::Vector3 value;
};

void RegisterComponents(flecs::world& world);

void ExecuteQueryComputePlayerInputVelocity(flecs::world& world);
void ExecuteQueryGravity(flecs::world& world);
void ExecuteQueryGravityDebugCameraTarget(flecs::world& world);

