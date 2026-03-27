#include "Components.h"
#include "Systems.h"
#include "../PlayerConstants.h"

#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DebugModule.h>

#include "../Singletons.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void ExecuteQueryComputePlayerInputVelocity(duin::GameWorld &world)
{
    static auto q =
        world.QueryBuilder<PlayerMovementInputVec3, InputVelocityDirection, const Transform3D>().Cached().Build();

    q.Each([&world](duin::Entity e, PlayerMovementInputVec3 &input, InputVelocityDirection &iDir, const Transform3D &tx) {
        duin::Quaternion r = world.GetGlobalRotation(e);
        duin::Vector3 alignedInput = duin::Vector3RotateByQuaternion(input.value, r);
        iDir.value = duin::Vector3(alignedInput.x, 0.0f, alignedInput.z);
    });
}

void ExecuteQueryIdle(duin::GameWorld &world)
{
    static auto q = world.QueryBuilder<InputVelocities, const Velocity3D>()
                        .With<IdleTag>()
                        .Build();

    world.DeferBegin();
    q.Each([](duin::Entity e, InputVelocities &inputVels, const Velocity3D &velocity) {
        double delta = duin::GetPhysicsFrameTime();

        duin::Vector3 targetVel = duin::Vector3Zero();

        float smoothFactor = PlayerConstants::IDLE_SMOOTH_FACTOR;
        float alpha = 1.0f - std::expf(-smoothFactor * (float)delta);

        float outputVelX = targetVel.x - velocity.value.x;
        float outputVelZ = targetVel.z - velocity.value.z;

        debugWatchlist.Post("outputVel:", "{ %.2f, 0.00, %.2f }", outputVelX, outputVelZ);

        float friction = PlayerConstants::GROUND_FRICTION;
        if (e.Has<OnGroundTag>())
        {
            friction = PlayerConstants::GROUND_FRICTION;
        }
        else if (e.Has<InAirTag>())
        {
            friction = PlayerConstants::AIR_FRICTION;
        }

        duin::Vector3 outputVel(outputVelX, 0.0f, outputVelZ);
        outputVel = duin::Vector3Scale(outputVel, friction);
        outputVel = duin::Vector3Scale(outputVel, alpha);

        inputVels.vec.push_back(outputVel);
    });
    world.DeferEnd();
}

void ExecuteQueryRun(duin::GameWorld &world)
{
    static duin::Query q =
        world.QueryBuilder<InputVelocities, InputVelocityDirection, const CanRunComponent, const Velocity3D>()
            .With<RunTag>()
            .Build();

    world.DeferBegin();
    q.Each([](duin::Entity e, InputVelocities &inputVels, InputVelocityDirection &iDir,
              const CanRunComponent &moveSpeed, const Velocity3D &velocity) {
        float acceleration = PlayerConstants::GROUND_ACCELERATION;
        float targetSpeed = moveSpeed.speed;

        if (e.Has<OnGroundTag>())
        {
            acceleration = PlayerConstants::GROUND_ACCELERATION;
        }
        else if (e.Has<InAirTag>())
        {
            acceleration = PlayerConstants::AIR_ACCELERATION;
            duin::Vector3 velXZ(velocity.value.x, 0.0f, velocity.value.z);
            float currentSpeed = duin::Vector3LengthF(velXZ);
            targetSpeed = std::max(currentSpeed, moveSpeed.speed * PlayerConstants::AIR_SPEED_FACTOR);
        }

        double delta = duin::GetPhysicsFrameTime();

        duin::Vector3 targetVel = duin::Vector3Scale(iDir.value, targetSpeed);

        float smoothFactor = PlayerConstants::RUN_SMOOTH_FACTOR;
        float alpha = 1.0f - std::expf(-smoothFactor * (float)delta);

        float outputVelX = targetVel.x - velocity.value.x;
        float outputVelZ = targetVel.z - velocity.value.z;

        debugWatchlist.Post("outputVel:", "{ %.2f, 0.00, %.2f }", outputVelX, outputVelZ);

        duin::Vector3 outputVel(outputVelX, 0.0f, outputVelZ);

        outputVel = duin::Vector3Scale(outputVel, acceleration);
        outputVel = duin::Vector3Scale(outputVel, alpha);

        inputVels.vec.push_back(outputVel);
    });
    world.DeferEnd();
}

void ExecuteQuerySprint(duin::GameWorld &world)
{
    static auto q = world
                        .QueryBuilder<InputVelocities, InputVelocityDirection, const CanRunComponent,
                                      const CanSprintComponent, const Velocity3D>()
                        .With<SprintTag>()
                        .With<OnGroundTag>()
                        .Build();

    world.DeferBegin();
    q.Each([](duin::Entity e, InputVelocities &inputVels, InputVelocityDirection &iDir,
              const CanRunComponent &runSpeed, const CanSprintComponent &moveSpeed, const Velocity3D &velocity) {
        double delta = duin::GetPhysicsFrameTime();

        duin::Vector3 targetVel = duin::Vector3Scale(iDir.value, moveSpeed.speed);

        float smoothFactor = PlayerConstants::SPRINT_SMOOTH_FACTOR;
        if (duin::Vector3LengthSqr(velocity.value) < (runSpeed.speed * runSpeed.speed))
        {
            smoothFactor = PlayerConstants::RUN_SMOOTH_FACTOR;
        }
        float alpha = 1.0f - std::expf(-smoothFactor * (float)delta);

        float outputVelX = targetVel.x - velocity.value.x;
        float outputVelZ = targetVel.z - velocity.value.z;

        debugWatchlist.Post("outputVel:", "{ %.2f, 0.00, %.2f }", outputVelX, outputVelZ);

        duin::Vector3 outputVel(outputVelX, 0.0f, outputVelZ);
        outputVel = duin::Vector3Scale(outputVel, alpha);

        inputVels.vec.push_back(outputVel);
    });
    world.DeferEnd();
}
