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
            //// Define sensitivity factor for yaw
            //const float sensitivity = MOUSE_SENSITIVITY.x; // Adjust as needed

            //// Calculate yaw rotation quaternion based on mouse X movement
            //float yawAngle = -mouseDelta.value.x * sensitivity;
            //duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{ 0.0f, 1.0f, 0.0f }, yawAngle);

            //// Update current rotation by applying yaw
            //r.value = duin::QuaternionMultiply(yawQuat, r.value);
            //r.value = duin::QuaternionNormalize(r.value);

                        // Define sensitivity factor for yaw (ensure this is appropriately scaled)
            const float sensitivity = MOUSE_SENSITIVITY.x; // Adjust as needed

            // Update cumulative yaw based on mouse delta
            float deltaYaw = -mouseDelta.value.x * sensitivity;
            yaw.value += deltaYaw;

            // (Optional) Clamp the yaw angle to prevent excessive rotation
            // For a typical first-person camera, yaw can usually wrap around 360 degrees
            // Hence, clamping might not be necessary. If needed, implement wrapping:
            if (yaw.value > 2.0f * PI) yaw.value -= 2.0f * PI;
            if (yaw.value < -2.0f * PI) yaw.value += 2.0f * PI;

            // Create a yaw quaternion around the Y-axis
            duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{ 0.0f, 1.0f, 0.0f }, deltaYaw);

            // Apply the yaw rotation by multiplying the current rotation by the yaw quaternion
            // Note the order: current rotation * yawQuat
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
            const float minPitch = DEG2RAD *  -87.0f;
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