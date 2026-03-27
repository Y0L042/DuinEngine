#include "Components.h"
#include "Systems.h"
#include "../PlayerConstants.h"

#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DebugModule.h>

#include "../Singletons.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void ExecuteQueryGravity(duin::GameWorld &world)
{
    static auto q =
        world.QueryBuilder<InputVelocities, const CharacterBodyComponent, const GravityComponent, const Mass>()
            .With<CanGravity>()
            .Build();

    q.Each([](duin::Entity e, InputVelocities &inputVelocities, const CharacterBodyComponent &cb,
              const GravityComponent &gravity, const Mass &mass) {
        duin::Vector3 gravityVel = duin::Vector3Scale(gravity.value, duin::GetPhysicsFrameTime());
        inputVelocities.vec.push_back(gravityVel);
    });
}

void ExecuteQueryOnGroundJump(duin::GameWorld &world)
{
    static auto q =
        world.QueryBuilder<InputForces, const CanJumpComponent>()
            .With<JumpTag>()
            .With<OnGroundTag>()
            .Build();

    world.DeferBegin();
    q.Each([](duin::Entity e, InputForces &inputForces, const CanJumpComponent &moveSpeed) {
        debugConsole.Log("Jumping!");
        duin::Vector3 iForce(0.0f, moveSpeed.impulse, 0.0f);
        inputForces.vec.push_back(iForce);
        e.Remove<JumpTag>();
    });
    world.DeferEnd();
}

void ExecuteQueryResolveInputVelocities(duin::GameWorld &world)
{
    static auto q = world.QueryBuilder<InputVelocities, Velocity3D>().Cached().Build();

    q.Each([](duin::Entity e, InputVelocities &inputVels, Velocity3D &velocity) {
        duin::Vector3 accumVel = duin::Vector3Zero();
        for (duin::Vector3 &vec : inputVels.vec)
        {
            accumVel = duin::Vector3Add(accumVel, vec);
        }
        velocity.value = duin::Vector3Add(velocity.value, accumVel);

        inputVels.vec.clear();

        debugWatchlist.Post("accumVel:", "{ %.2f, %.2f, %.2f }", accumVel.x, accumVel.y, accumVel.z);
        debugWatchlist.Post("accumVel size:", "%d", inputVels.vec.size());
    });
}

void ExecuteQueryResolveInputForces(duin::GameWorld &world)
{
    static duin::Query q =
        world.QueryBuilder<InputForces, InputVelocities, Velocity3D, const Mass>().Cached().Build();

    q.Each([](duin::Entity e, InputForces &inputForces, InputVelocities &inputVelocities, Velocity3D &velocity, const Mass &mass) {
        duin::Vector3 netForce = duin::Vector3Zero();
        for (duin::Vector3 &vec : inputForces.vec)
        {
            netForce = duin::Vector3Add(netForce, vec);
        }
        float mass_ = mass.value < 0.1f ? 1.0f : mass.value;
        duin::Vector3 a(duin::Vector3Scale(netForce, (1.0f / mass_)));
        duin::Vector3Scale(a, duin::GetPhysicsFrameTime());

        if (inputForces.vec.size() > 0)
        {
            inputVelocities.vec.push_back(a);
        }

        debugWatchlist.Post("Forces size", "%d", inputForces.vec.size());
        debugWatchlist.Post("Fnet", "%.2f", duin::Vector3Length(a));

        inputForces.vec.clear();
    });
}
