#pragma once

#include <Duin/Core/Maths/MathsModule.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/Core/Debug/DebugModule.h>

#include <vector>

struct InputVelocities
{
    std::vector<duin::Vector3> vec;
};

struct InputForces
{
    std::vector<duin::Vector3> vec;
};

struct InputVelocityDirection
{
    duin::Vector3 value;
};

struct PlayerMovementInputVec3
{
    duin::Vector3 value;
};

struct GravityComponent
{
    duin::Vector3 value = {0.0f, -9.81f, 0.0f};
};
struct CanGravity
{
};

struct DebugCameraTarget
{
    duin::Vector3 value;
};

struct MouseInputVec2
{
    duin::Vector2 value;
};

struct CameraPitchComponent
{
    float value = 0.0f;
};

struct CameraYawComponent
{
    float value = 0.0f;
};

struct Mass
{
    float value = 1.0f;
};

struct GroundFriction
{
    float value = 0.85f;
};

struct AirFriction
{
    float value = 0.25f;
};

struct IdleTag
{
};

struct CanRunComponent
{
    float speed = 7.5f;
};
struct RunTag
{
};

struct CanJumpComponent
{
    float impulse = 29000.61f;
};
struct JumpTag
{
};

struct CanSprintComponent
{
    float speed = 12.0f;
};
struct SprintTag
{
};

struct VelocityBob
{
    float frequency = 1.0f;
    float amplitude = 1.0f;
};

struct Movement3DInput
{
    float x, y, z;
};

struct DebugCameraTag
{
};

struct OnGroundTag
{
};
struct InAirTag
{
};

void RegisterComponents(flecs::world &world);

void ExecuteQueryUpdatePlayerYaw(flecs::world &world);
void ExecuteQueryUpdateCameraPitch(flecs::world &world);
void ExecuteQueryComputePlayerInputVelocity(flecs::world &world);
void ExecuteQueryGravity(flecs::world &world);
void ExecuteQueryDebugCameraTarget(flecs::world &world);
void ExecuteQueryVelocityBob(flecs::world &world);

void ExecuteQueryMoveDebugCamera(flecs::world &world);

void ExecuteQueryIdle(flecs::world &world);
void ExecuteQueryRun(flecs::world &world);
void ExecuteQuerySprint(flecs::world &world);
void ExecuteQueryOnGroundJump(flecs::world &world);

void ExecuteQueryResolveInputVelocities(flecs::world &world);
void ExecuteQueryResolveInputForces(flecs::world &world);
