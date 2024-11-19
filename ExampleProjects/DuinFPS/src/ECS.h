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

struct MouseInputVec2 {
    duin::Vector2 value;
};

struct CameraPitchComponent {
    float value = 0.0f;
};

struct CameraYawComponent {
    float value = 0.0f;
};

void RegisterComponents(flecs::world& world);

void ExecuteQueryUpdatePlayerYaw(flecs::world& world);
void ExecuteQueryUpdateCameraPitch(flecs::world& world);
void ExecuteQueryComputePlayerInputVelocity(flecs::world& world);
void ExecuteQueryGravity(flecs::world& world);
void ExecuteQueryGravityDebugCameraTarget(flecs::world& world);

