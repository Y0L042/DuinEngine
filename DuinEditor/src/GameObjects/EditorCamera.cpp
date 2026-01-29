#include "EditorCamera.h"

#include <Duin/Render/RenderModule.h>
#include <Duin/Core/Events/EventsModule.h>

EditorCamera::EditorCamera(duin::Vector3 pos, duin::Vector3 target)
{
    SetupInput();
    camera.SetAxis(pos, target, duin::Vector3::UP);
    camera.position = duin::Vector3(0.0f, 15.0f, 5.0f);
}

void EditorCamera::SetupInput()
{
    duin::AddInputActionBinding("OnEditorCameraOrbit", DN_MOUSE_01, DN_MOUSE_BUTTON_RIGHT, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraOrbit", DN_KEYBOARD_01, DN_KEY_LALT, DN_KEVENT_HELD);

    duin::AddInputActionBinding("OnEditorCameraMoveLeft", DN_KEYBOARD_01, DN_KEY_LEFT, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraMoveRight", DN_KEYBOARD_01, DN_KEY_RIGHT, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraMoveForward", DN_KEYBOARD_01, DN_KEY_UP, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraMoveBackward", DN_KEYBOARD_01, DN_KEY_DOWN, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraMoveAxisLock", DN_KEYBOARD_01, DN_KEY_LCTRL, DN_KEVENT_HELD);
    duin::AddInputActionBinding("OnEditorCameraMoveAxisLock", DN_KEYBOARD_01, DN_KEY_RCTRL, DN_KEVENT_HELD);

    duin::AddInputActionBinding("OnEditorCameraZoom", DN_MOUSE_01, DN_MOUSE_WHEEL, DN_KEVENT_PRESSED);
}

void EditorCamera::Render()
{
    duin::DrawSphere(camera.target);
}

void EditorCamera::Enable(bool enable)
{
    if (enable)
    {
        duin::SetActiveCamera(&camera);
    }
    else
    {
        duin::SetActiveCamera(nullptr);
    }
}

void EditorCamera::PhysicsUpdate(double delta)
{
    MovePosition(delta);
}

void EditorCamera::UpdatePosition(duin::Vector3 pos)
{
    camera.position = pos;
}

void EditorCamera::UpdateTarget(duin::Vector3 target)
{
    camera.target = target;
}

void EditorCamera::Yaw(float angle, bool aroundTarget)
{
    duin::CameraYaw(&camera, angle, aroundTarget);
}

void EditorCamera::Pitch(float angle, bool aroundTarget)
{
    duin::CameraPitch(&camera, angle, false, aroundTarget, true);
}

void EditorCamera::Roll(float angle)
{
    duin::CameraRoll(&camera, angle);
}

void EditorCamera::MovePosition(double delta)
{
    const float MOVE_SPEED_XZ = 10.0f;
    const float SENSITIVITY_X = 1.00f / 180.0f;
    const float SENSITIVITY_Y = 1.0f / 180.0f;

    if (duin::IsInputActionTriggered("OnEditorCameraOrbit"))
    {
        duin::Input::CaptureMouse(true);
        float mouseXDelta = duin::Input::GetMouseDelta().x;
        float angleXDelta = mouseXDelta * SENSITIVITY_X;
        Yaw(angleXDelta, true);

        float mouseYDelta = duin::Input::GetMouseDelta().y;
        float angleYDelta = mouseYDelta * SENSITIVITY_Y;
        Pitch(-angleYDelta, true);
    }
    else
    {
        duin::Input::CaptureMouse(false);
    }

    int L = 0;
    int R = 0;
    int F = 0;
    int B = 0;

    duin::OnInputActionTriggered("OnEditorCameraMoveLeft", [&L]() { L = 1; });
    duin::OnInputActionTriggered("OnEditorCameraMoveRight", [&R]() { R = 1; });
    duin::OnInputActionTriggered("OnEditorCameraMoveForward", [&F]() { F = 1; });
    duin::OnInputActionTriggered("OnEditorCameraMoveBackward", [&B]() { B = 1; });

    float dx = (R - L);
    float dz = (F - B);

    if ((std::abs(dx) || std::abs(dz)))
    {
        duin::Vector3 delta_xz;

        if (duin::IsInputActionTriggered("OnEditorCameraMoveAxisLock"))
        {
            delta_xz = duin::Vector3(dx, 0.0f, -dz); // -dz makes UP key move camera forward along -Z
            delta_xz = duin::Vector3NormalizeF(delta_xz);
            delta_xz = duin::Vector3Scale(delta_xz, (float)delta * MOVE_SPEED_XZ);
        }
        else
        {
            duin::Vector3 right_xz = duin::GetCameraRight(&camera);
            right_xz.y = 0.0f;
            duin::Vector3 forward_xz = duin::Vector3CrossProduct(duin::Vector3::UP, right_xz);
            forward_xz.y = 0.0f;
            delta_xz = duin::Vector3Add(duin::Vector3Scale(right_xz, dx), duin::Vector3Scale(forward_xz, dz));
            delta_xz = duin::Vector3NormalizeF(delta_xz);
            delta_xz = duin::Vector3Scale(delta_xz, (float)delta * MOVE_SPEED_XZ);
        }

        camera.position = duin::Vector3Add(camera.position, delta_xz);
        camera.target = duin::Vector3Add(camera.target, delta_xz);
    }

    // duin::Vector2 mouseWheel;
    // duin::GetInputActionTriggeredValue<duin::Vector2>(mouseWheel);
}
