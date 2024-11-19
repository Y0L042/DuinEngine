#include "ECS.h"
#include "Singletons.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

const Vector2 MOUSE_SENSITIVITY = { 0.001f, 0.001f };


void RegisterComponents(flecs::world& world)
{
    world.component<PlayerMovementInputVec3>();
    world.component<GravityComponent>();
    world.component<DebugCameraTarget>();
}



void ExecuteQueryComputePlayerInputVelocity(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        Velocity3D,
        const Rotation3D,
        const PlayerMovementInputVec3
    >()
    .term_at(1).second<Global>()
    .cached()
    .build();
     
    q.each([](
        Velocity3D& v,
        const Rotation3D& r,
        const PlayerMovementInputVec3& input
        ) {
            float speed = 7.0f;

            duin::Vector3 alignedInput = duin::Vector3RotateByQuaternion(input.value, r.value);
            v.value = duin::Vector3Scale(alignedInput, speed);
            debugWatchlist.Post("Input: ", "{ %.1f, %.1f, %.1f }", input.value.x, input.value.y, input.value.z);
    });
}

void ExecuteQueryGravity(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        Velocity3D,
        const GravityComponent
    >()
    .cached()
    .build();

    q.each([](
        Velocity3D& v,
        const GravityComponent& gravity
        ) {
            v.value = duin::Vector3Add(v.value, gravity.value);
        });
}

void ExecuteQueryGravityDebugCameraTarget(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        Camera3D,
        const DebugCameraTarget
    >()
        .cached()
        .build();

    q.each([](
        Camera3D& c,
        const DebugCameraTarget& t
        ) {
            c.target = t.value.ToRaylib();
        });
}