#include "ECS.h"
#include "Singletons.h"

#include <iostream>

#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DebugModule.h>

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

const duin::Vector2 MOUSE_SENSITIVITY = {0.001f, 0.001f};

void RegisterComponents(duin::World &world)
{
    world.Component<PlayerMovementInputVec3>();
    world.Component<GravityComponent>();
    world.Component<DebugCameraTarget>();
    world.Component<DebugCameraTag>();
    world.Component<MouseInputVec2>();
    world.Component<CameraPitchComponent>();
    world.Component<CameraYawComponent>();
    world.Component<InputVelocities>();
    world.Component<Movement3DInput>();
    world.Component<InputForces>();
    world.Component<InputVelocityDirection>();
    world.Component<Mass>();
    world.Component<GroundFriction>();
    world.Component<AirFriction>();
    world.Component<IdleTag>();
    world.Component<VelocityBob>();

    world.Component<CanRunComponent>();
    world.Component<RunTag>();
    world.Component<CanJumpComponent>();
    world.Component<JumpTag>();
    world.Component<CanSprintComponent>();
    world.Component<SprintTag>();
    world.Component<OnGroundTag>();
    world.Component<InAirTag>();
    world.Component<CanGravity>();
}

void ExecuteQueryComputePlayerInputVelocity(duin::World &world)
{
    static auto q =
        world.QueryBuilder<PlayerMovementInputVec3, InputVelocityDirection, const Transform3D>().Cached().Build();

    q.Each([](duin::Entity e, PlayerMovementInputVec3 &input, InputVelocityDirection &iDir, const Transform3D &tx) {
        duin::Quaternion r = Transform3D::GetGlobalRotation(e.GetFlecsEntity());
        duin::Vector3 alignedInput = duin::Vector3RotateByQuaternion(input.value, r);
        iDir.value = duin::Vector3(alignedInput.x, 0.0f, alignedInput.z);
    });
}

void ExecuteQueryDebugCameraTarget(duin::World &world)
{
    // static flecs::query q = world.query_builder<
    //     Camera3D,
    //     const DebugCameraTarget
    //>()
    //     .cached()
    //     .build();

    // q.each([](
    //         Camera3D& c,
    //         const DebugCameraTarget& t
    //     ) {
    //         c.target = t.value.ToRaylib();
    //     });
}

void ExecuteQueryUpdatePlayerYaw(duin::World &world)
{
    static auto q =
        world.QueryBuilder<Transform3D, CameraYawComponent, const MouseInputVec2>().Cached().Build();

    q.Each([](duin::Entity e, Transform3D &tx, CameraYawComponent &yaw, const MouseInputVec2 &mouseDelta) {
        const float sensitivity = MOUSE_SENSITIVITY.x;

        float deltaYaw = -mouseDelta.value.x * sensitivity;
        yaw.value += deltaYaw;

        if (yaw.value > 2.0f * PI)
            yaw.value -= 2.0f * PI;
        if (yaw.value < -2.0f * PI)
            yaw.value += 2.0f * PI;

        duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{0.0f, 1.0f, 0.0f}, deltaYaw);
        debugWatchlist.Post("Current Yaw: ", " % .2f", yaw.value);
        debugWatchlist.Post("Delta Yaw: ", " % .2f", deltaYaw);

        // duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{ 0.0f, 1.0f, 0.0f }, yaw.value);

        tx.SetRotation(duin::QuaternionMultiply(yawQuat, tx.GetRotation()));
        tx.SetRotation(duin::QuaternionNormalize(tx.GetRotation()));
    });
}

void ExecuteQueryUpdateCameraPitch(duin::World &world)
{
    static duin::Query q = world
                                .QueryBuilder<
                                    // Rotation3D,
                                    Transform3D, CameraPitchComponent, const MouseInputVec2>()
                                // .term_at(0).second<Local>()
                                .Cached()
                                .Build();

    q.Each([](duin::Entity e,
               // Rotation3D& r,
               Transform3D &tx, CameraPitchComponent &pitch, const MouseInputVec2 &mouseDelta) {
        const float sensitivity = MOUSE_SENSITIVITY.y;

        // Update cumulative pitch based on mouse delta
        float deltaPitch = -mouseDelta.value.y * sensitivity;
        pitch.value += deltaPitch;
        debugWatchlist.Post("Cam pitch: ", "{ %.2f, %.2f }", mouseDelta.value.x, -mouseDelta.value.y);

        // Clamp the pitch angle to prevent flipping
        const float maxPitch = DEG2RAD * 89.0f;
        const float minPitch = DEG2RAD * -87.0f;
        if (pitch.value > maxPitch)
            pitch.value = maxPitch;
        if (pitch.value < -maxPitch)
            pitch.value = minPitch;

        // Extract current yaw from the rotation quaternion
        duin::Vector3 euler = duin::QuaternionToEuler(tx.GetRotation());
        float currentYaw = euler.y;

        // Reconstruct the rotation quaternion with clamped pitch and existing yaw
        tx.SetRotation(duin::QuaternionFromEuler(pitch.value, currentYaw, 0.0f)); // Assuming roll is zero
        tx.SetRotation(duin::QuaternionNormalize(tx.GetRotation()));
    });
}

void ExecuteQueryGravity(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q =
        flecsWorld.query_builder<InputVelocities, const CharacterBodyComponent, const GravityComponent, const Mass>()
            .with<CanGravity>()
            // .cached()
            .build();

    q.each([](InputVelocities &inputVelocities, const CharacterBodyComponent &cb, const GravityComponent &gravity,
              const Mass &mass) {
        duin::Vector3 gravityVel = duin::Vector3Scale(gravity.value, duin::GetPhysicsFrameTime());
        inputVelocities.vec.push_back(gravityVel);
    });
}

// UNUSED
void ExecuteQueryFriction(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q =
        flecsWorld.query_builder<InputVelocities, const GroundFriction, const Mass>().with<CanGravity>().cached().build();

    q.each([](InputVelocities &inputVelocities, const GroundFriction &friction, const Mass &mass) {

    });
}

void ExecuteQueryIdle(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q = flecsWorld.query_builder<InputVelocities, const Velocity3D>().with<IdleTag>().build();

    flecsWorld.defer_begin();
    q.each([](flecs::entity e, InputVelocities &inputVels, const Velocity3D &velocity) {
        double delta = duin::GetPhysicsFrameTime();

        // Scale movement input vector by speed to get target velocity
        duin::Vector3 targetVel = duin::Vector3Zero();

        float smoothFactor = 20.75f;
        float alpha = 1.0f - std::expf(-smoothFactor * (float)delta);

        float outputVelX = targetVel.x - velocity.value.x;
        float outputVelZ = targetVel.z - velocity.value.z;

        debugWatchlist.Post("outputVel:", "{ %.2f, 0.00, %.2f }", outputVelX, outputVelZ);

        float friction = 1.0f;
        if (e.has<OnGroundTag>())
        {
            friction = 1.0f;
        }
        else if (e.has<InAirTag>())
        {
            friction = 0.01f;
        }

        duin::Vector3 outputVel(outputVelX, 0.0f, outputVelZ);
        outputVel = duin::Vector3Scale(outputVel, friction);
        outputVel = duin::Vector3Scale(outputVel, alpha);

        inputVels.vec.push_back(outputVel);
    });
    flecsWorld.defer_end();
}

void ExecuteQueryRun(duin::World &world)
{
    static duin::Query q =
        world.QueryBuilder<InputVelocities, InputVelocityDirection, const CanRunComponent, const Velocity3D>()
            .With<RunTag>()
            .Build();

    world.DeferBegin();
    q.Each([](duin::Entity e, InputVelocities &inputVels, InputVelocityDirection &iDir,
              const CanRunComponent &moveSpeed, const Velocity3D &velocity) {
        float acceleration = 1.0f;
        float targetSpeed = moveSpeed.speed;

        if (e.Has<OnGroundTag>())
        {
            acceleration = 1.0f;
        }
        else if (e.Has<InAirTag>())
        {
            constexpr float airSpeedFactor = 0.65f;
            acceleration = 0.25f;
            duin::Vector3 velXZ(velocity.value.x, 0.0f, velocity.value.z);
            float currentSpeed = duin::Vector3LengthF(velXZ);
            targetSpeed = std::max(currentSpeed, moveSpeed.speed * airSpeedFactor);
        }

        double delta = duin::GetPhysicsFrameTime();

        // Scale movement input vector by speed to get target velocity
        duin::Vector3 targetVel = duin::Vector3Scale(iDir.value, targetSpeed);

        float smoothFactor = 10.75f;
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

void ExecuteQuerySprint(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q = flecsWorld
                                .query_builder<InputVelocities, InputVelocityDirection, const CanRunComponent,
                                               const CanSprintComponent, const Velocity3D>()
                                .with<SprintTag>()
                                .with<OnGroundTag>()
                                .build();

    flecsWorld.defer_begin();
    q.each([](flecs::entity e, InputVelocities &inputVels, InputVelocityDirection &iDir,
              const CanRunComponent &runSpeed, const CanSprintComponent &moveSpeed, const Velocity3D &velocity) {
        double delta = duin::GetPhysicsFrameTime();

        // Scale movement input vector by speed to get target velocity
        duin::Vector3 targetVel = duin::Vector3Scale(iDir.value, moveSpeed.speed);

        float smoothFactor = 4.5f;
        if (duin::Vector3LengthSqr(velocity.value) < (runSpeed.speed * runSpeed.speed))
        {
            smoothFactor = 10.75f;
        }
        float alpha = 1.0f - std::expf(-smoothFactor * (float)delta);

        float outputVelX = targetVel.x - velocity.value.x;
        float outputVelZ = targetVel.z - velocity.value.z;

        debugWatchlist.Post("outputVel:", "{ %.2f, 0.00, %.2f }", outputVelX, outputVelZ);

        duin::Vector3 outputVel(outputVelX, 0.0f, outputVelZ);
        outputVel = duin::Vector3Scale(outputVel, alpha);

        inputVels.vec.push_back(outputVel);
    });
    flecsWorld.defer_end();
}

void ExecuteQueryVelocityBob(duin::World &world)
{
    static flecs::query q = world.GetFlecsWorld()
                                .query_builder<VelocityBob,
                                               // Position3D,
                                               Transform3D, const Velocity3D *>()
                                // .term_at(1).second<Local>()
                                .term_at(2)
                                .parent()
                                .cascade()
                                .with<OnGroundTag>()
                                .cached()
                                .build();

    world.DeferBegin();
    q.each([](flecs::entity e, VelocityBob &bob,
              // Position3D& localPos,
              Transform3D &tx, const Velocity3D *velocity) {
        // if (velocity) {
        //     duin::Vector3 hVelocity(velocity->value.x, 0.0f, velocity->value.z);
        //     float velocityMagnitude = duin::Vector3LengthF(hVelocity);
        //     float bobEffectX = std::sin(GetTime() * bob.frequency / 2.0f) * (velocityMagnitude / 15000.0f) *
        //     bob.amplitude; float bobEffectY = std::sin(GetTime() * bob.frequency) * (velocityMagnitude / 10000.0f) *
        //     bob.amplitude; duin::Vector3 txPos = tx.GetPosition(); tx.SetPosition({ bobEffectX *
        //     duin::GetPhysicsFPS(), bobEffectY * duin::GetPhysicsFPS(), txPos.z });
        // }
    });
    world.DeferEnd();
}

void ExecuteQueryMoveDebugCamera(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q = flecsWorld
                                .query_builder<Movement3DInput,
                                               // Position3D
                                               Transform3D>()
                                // .term_at(1).second<Global>()
                                .with<DebugCameraTag>()
                                .build();

    flecsWorld.defer_begin();
    q.each([](flecs::entity e, Movement3DInput &input,
              // Position3D& gPos
              Transform3D &tx) {
        double delta = duin::GetPhysicsFrameTime();
        duin::Vector3 inputV(input.x, input.y, input.z);
        inputV = duin::Vector3Scale(inputV, 5.0f);
        Transform3D::SetGlobalPosition(
            e, duin::Vector3Add(Transform3D::GetGlobalPosition(e), duin::Vector3Scale(inputV, (float)delta)));
        // e.remove<Movement3DInput>();
        input.x = 0.0f;
        input.y = 0.0f;
        input.z = 0.0f;
    });
    flecsWorld.defer_end();
}

void ExecuteQueryOnGroundJump(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();
    static flecs::query q =
        flecsWorld.query_builder<InputForces, const CanJumpComponent>().with<JumpTag>().with<OnGroundTag>().build();

    flecsWorld.defer_begin();
    q.each([](flecs::entity e, InputForces &inputForces, const CanJumpComponent &moveSpeed) {
        debugConsole.Log("Jumping!");
        duin::Vector3 iForce(0.0f, moveSpeed.impulse, 0.0f);
        inputForces.vec.push_back(iForce);
        e.remove<JumpTag>();
    });
    flecsWorld.defer_end();
}

void ExecuteQueryResolveInputVelocities(duin::World &world)
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

void ExecuteQueryResolveInputForces(duin::World &world)
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
