#include "Components.h"
#include "Systems.h"
#include "../PlayerConstants.h"

#include <Duin/Core/Application.h>
#include <Duin/Core/Debug/DebugModule.h>

#include "../Singletons.h"

using namespace duin::ECSComponent;
using namespace duin::ECSTag;

void ExecuteQueryUpdatePlayerYaw(duin::GameWorld &world)
{
    static auto q =
        world.QueryBuilder<Transform3D, CameraYawComponent, const MouseInputVec2>().Cached().Build();

    q.Each([](duin::Entity e, Transform3D &tx, CameraYawComponent &yaw, const MouseInputVec2 &mouseDelta) {
        const float sensitivity = PlayerConstants::MOUSE_SENSITIVITY;

        float deltaYaw = mouseDelta.value.x * sensitivity;
        yaw.value += deltaYaw;

        if (yaw.value > 2.0f * PI)
            yaw.value -= 2.0f * PI;
        if (yaw.value < -2.0f * PI)
            yaw.value += 2.0f * PI;

        duin::Quaternion yawQuat = duin::QuaternionFromAxisAngle(duin::Vector3{0.0f, 1.0f, 0.0f}, deltaYaw);
        debugWatchlist.Post("Current Yaw: ", " % .2f", yaw.value);
        debugWatchlist.Post("Delta Yaw: ", " % .2f", deltaYaw);

        tx.SetRotation(duin::QuaternionMultiply(yawQuat, tx.GetRotation()));
        tx.SetRotation(duin::QuaternionNormalize(tx.GetRotation()));
    });
}

void ExecuteQueryUpdateCameraPitch(duin::GameWorld &world)
{
    static duin::Query q = world
                                .QueryBuilder<Transform3D, CameraPitchComponent, const MouseInputVec2>()
                                .Cached()
                                .Build();

    q.Each([](duin::Entity e, Transform3D &tx, CameraPitchComponent &pitch, const MouseInputVec2 &mouseDelta) {
        const float sensitivity = PlayerConstants::MOUSE_SENSITIVITY;

        float deltaPitch = -mouseDelta.value.y * sensitivity;
        pitch.value += deltaPitch;
        debugWatchlist.Post("Cam pitch: ", "{ %.2f, %.2f }", mouseDelta.value.x, -mouseDelta.value.y);

        const float maxPitch = DEG2RAD * PlayerConstants::PITCH_MAX_DEG;
        const float minPitch = DEG2RAD * PlayerConstants::PITCH_MIN_DEG;
        if (pitch.value > maxPitch)
            pitch.value = maxPitch;
        if (pitch.value < -maxPitch)
            pitch.value = minPitch;

        duin::Vector3 euler = duin::QuaternionToEuler(tx.GetRotation());
        float currentYaw = euler.y;

        tx.SetRotation(duin::QuaternionFromEuler(pitch.value, currentYaw, 0.0f));
        tx.SetRotation(duin::QuaternionNormalize(tx.GetRotation()));
    });
}

void ExecuteQueryMoveDebugCamera(duin::GameWorld &world)
{
    static auto q = world
                        .QueryBuilder<Movement3DInput, Transform3D>()
                        .With<DebugCameraTag>()
                        .Build();

    world.DeferBegin();
    q.Each([&world](duin::Entity e, Movement3DInput &input, Transform3D &tx) {
        double delta = duin::GetPhysicsFrameTime();
        duin::Vector3 inputV(input.x, input.y, input.z);
        inputV = duin::Vector3Scale(inputV, PlayerConstants::DEBUG_CAMERA_SPEED);
        world.SetGlobalPosition(
            e, duin::Vector3Add(world.GetGlobalPosition(e), duin::Vector3Scale(inputV, (float)delta)));
        input.x = 0.0f;
        input.y = 0.0f;
        input.z = 0.0f;
    });
    world.DeferEnd();
}
