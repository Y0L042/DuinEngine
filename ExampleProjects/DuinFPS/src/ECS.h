#pragma once

#include <Duin.h>

#include <vector>

struct InputVelocities {
    std::vector<duin::Vector3> vec;
};

struct InputForces {
    std::vector<duin::Vector3> vec;
};

struct InputVelocityDirection {
    duin::Vector3 value;
};

struct PlayerMovementInputVec3 {
    duin::Vector3 value;
};

struct GravityComponent {
    duin::Vector3 value = { 0.0f, -9.81f, 0.0f };
};
struct CanGravity {};

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

struct Mass {
    float value = 1.0f;
};

struct CanRunComponent {
    float speed = 7.5f;
};
struct RunTag {};

struct CanJumpComponent {
    float impulse = 29000.61f;
};
struct JumpTag {};

struct CanSprintComponent {
    float speed = 12.0f;
};
struct SprintTag {};

struct OnGroundTag {};
struct InAirTag {};

void RegisterComponents(flecs::world& world);

void ExecuteQueryUpdatePlayerYaw(flecs::world& world);
void ExecuteQueryUpdateCameraPitch(flecs::world& world);
void ExecuteQueryComputePlayerInputVelocity(flecs::world& world);
void ExecuteQueryGravity(flecs::world& world);
void ExecuteQueryDebugCameraTarget(flecs::world& world);

void ExecuteQueryRun(flecs::world& world);
void ExecuteQuerySprint(flecs::world& world);
void ExecuteQueryJump(flecs::world& world);

void ExecuteQueryResolveInputVelocities(flecs::world& world);
void ExecuteQueryResolveInputForces(flecs::world& world);
