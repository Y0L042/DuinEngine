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
    world.component<MouseInputVec2>();
    world.component<CameraPitchComponent>();
    world.component<CameraYawComponent>();
    world.component<InputVelocities>();
    world.component<InputForces>();
    world.component<InputVelocityDirection>();
    world.component<Mass>();

    world.component<CanRunComponent>();
    world.component<RunTag>();
    world.component<CanJumpComponent>();
    world.component<JumpTag>();
    world.component<CanSprintComponent>();
    world.component<SprintTag>();
    world.component<OnGroundTag>();
    world.component<InAirTag>();
    world.component<CanGravity>();
}



void ExecuteQueryComputePlayerInputVelocity(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        PlayerMovementInputVec3,
        InputVelocityDirection,
        const Rotation3D
    >()
    .term_at(2).second<Global>()
    .cached()
    .build();
     
    q.each([](
            PlayerMovementInputVec3& input,
            InputVelocityDirection& iDir,
            const Rotation3D& r
        ) {
           duin::Vector3 alignedInput = duin::Vector3RotateByQuaternion(input.value, r.value);
           iDir.value = duin::Vector3(alignedInput.x, 0.0f, alignedInput.z);
    });
}

void ExecuteQueryDebugCameraTarget(flecs::world& world)
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

void ExecuteQueryUpdatePlayerYaw(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        Rotation3D,  
        CameraYawComponent,
        const MouseInputVec2
    >()
    .term_at(0).second<Local>()
    .cached()
    .build();

    q.each([](
            Rotation3D& r, 
            CameraYawComponent& yaw,
            const MouseInputVec2& mouseDelta
        ) {
            const float sensitivity = MOUSE_SENSITIVITY.x;

            float deltaYaw = -mouseDelta.value.x * sensitivity;
            yaw.value += deltaYaw;

            if (yaw.value > 2.0f * PI) yaw.value -= 2.0f * PI;
            if (yaw.value < -2.0f * PI) yaw.value += 2.0f * PI;

            duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{ 0.0f, 1.0f, 0.0f }, deltaYaw);

            r.value = duin::QuaternionMultiply(r.value, yawQuat);
            r.value = duin::QuaternionNormalize(r.value);
        }
    );
}

void ExecuteQueryUpdateCameraPitch(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        Rotation3D, 
        CameraPitchComponent,
        const MouseInputVec2
    >()
    .term_at(0).second<Local>()
    .cached()
    .build();

    q.each([](
            Rotation3D& r,
            CameraPitchComponent& pitch,
            const MouseInputVec2& mouseDelta
        ) {
            const float sensitivity = MOUSE_SENSITIVITY.y;

            // Update cumulative pitch based on mouse delta
            float deltaPitch = -mouseDelta.value.y * sensitivity;
            pitch.value += deltaPitch;
            debugWatchlist.Post("Cam pitch: ", "{ %.2f, %.2f }", mouseDelta.value.x, -mouseDelta.value.y);

            // Clamp the pitch angle to prevent flipping
            const float maxPitch = DEG2RAD * 89.0f;
            const float minPitch = DEG2RAD * -87.0f;
            if (pitch.value > maxPitch) pitch.value = maxPitch;
            if (pitch.value < -maxPitch) pitch.value = minPitch;

            // Extract current yaw from the rotation quaternion
            duin::Vector3 euler = duin::QuaternionToEuler(r.value);
            float currentYaw = euler.y;

            // Reconstruct the rotation quaternion with clamped pitch and existing yaw
            r.value = duin::QuaternionFromEuler(pitch.value, currentYaw, 0.0f); // Assuming roll is zero
            r.value = duin::QuaternionNormalize(r.value);
        }
    );
}

void ExecuteQueryGravity(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputForces,
        const CharacterBody3DComponent,
        const GravityComponent,
        const Mass
    >()
        .with<CanGravity>()
        .cached()
        .build();

    q.each([](
            InputForces& iF,
            const CharacterBody3DComponent& cb,
            const GravityComponent& gravity,
            const Mass& mass
        ) {
            duin::Vector3 g(duin::Vector3Scale(gravity.value, mass.value));
            if (!cb.characterBody->IsOnFloor()) {
                iF.vec.push_back(g);
            }
        });
}

void ExecuteQueryRun(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputVelocities,
        InputVelocityDirection,
        const CanRunComponent
        //const Velocity3D
    >()
        .with<RunTag>()
        .with<OnGroundTag>()
        .build();
    
    world.defer_begin();
        q.each([](
            flecs::entity e,
            InputVelocities& inputVels,
            InputVelocityDirection& iDir,
            const CanRunComponent& moveSpeed
            //const Velocity3D& velocity
            ) {
               duin::Vector3 iVel = duin::Vector3Scale(iDir.value, moveSpeed.speed);
               inputVels.vec.push_back(iVel);
               e.remove<RunTag>();
            }
        );
    world.defer_end();
}

void ExecuteQuerySprint(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputVelocities,
        InputVelocityDirection,
        const CanSprintComponent
        //const Velocity3D
    >()
        .with<SprintTag>()
        .with<OnGroundTag>()
        .build();

    world.defer_begin();
        q.each([](
            flecs::entity e,
            InputVelocities& inputVels,
            InputVelocityDirection& iDir,
            const CanSprintComponent& moveSpeed
            //const Velocity3D& velocity
            ) {
               duin::Vector3 iVel = duin::Vector3Scale(iDir.value, moveSpeed.speed);
               inputVels.vec.push_back(iVel);
               e.remove<SprintTag>();
            }
        );
    world.defer_end();
}

void ExecuteQueryJump(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputForces,
        const CanJumpComponent
    >()
        .with<JumpTag>()
        .with<OnGroundTag>()
        .build();

    world.defer_begin();
        q.each([](
            flecs::entity e,
            InputForces& inputForces,
            const CanJumpComponent& moveSpeed
            ) {
                debugConsole.Log("Jumping!");
                duin::Vector3 iForce(0.0f, moveSpeed.impulse, 0.0f);
                inputForces.vec.push_back(iForce);                
                e.remove<JumpTag>();
            }
        );
    world.defer_end();
}

void ExecuteQueryResolveInputVelocities(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputVelocities,
        Velocity3D
    >()
        .cached()
        .build();

    q.each([](
        InputVelocities& inputVels,
        Velocity3D& velocity
        ) {
            duin::Vector3 accumVel = duin::Vector3Zero();
            for (duin::Vector3& vec : inputVels.vec) {
                accumVel = duin::Vector3Add(accumVel, vec);
            }

            velocity.value = accumVel;

            inputVels.vec.clear();
        }
    );
}

void ExecuteQueryResolveInputForces(flecs::world& world)
{
    static flecs::query q = world.query_builder<
        InputForces,
        InputVelocities,
        Velocity3D,
        const Mass
    >()
        .cached()
        .build();

    q.each([](
        InputForces& inputForces,
        InputVelocities& inputVelocities,
        Velocity3D& velocity,
        const Mass& mass
        ) {
            duin::Vector3 netForce = duin::Vector3Zero();
            for (duin::Vector3& vec : inputForces.vec) {
                netForce = duin::Vector3Add(netForce, vec);
            }
            float mass_ = mass.value < 0.1f ? 1.0f : mass.value;
            duin::Vector3 a(duin::Vector3Scale(netForce, (1.0f / mass_)));

            debugWatchlist.Post("Fnet", "%.2f", duin::Vector3Length(a));

            duin::Vector3Scale(a, duin::GetPhysicsFrameTime());
            inputVelocities.vec.push_back(a);

            inputForces.vec.clear();
        }
    );
}
