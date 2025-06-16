#include "EditorCamera.h"

#include <Duin/Render/RenderModule.h>


EditorCamera::EditorCamera(duin::Vector3 pos, duin::Vector3 target)
{
    camera.position = pos;
    camera.target = target;
}

void EditorCamera::Draw()
{
    duin::DrawSphere(camera.target);
}

void EditorCamera::Enable(bool enable)
{
    if (enable) {
        duin::SetActiveCamera(&camera);
    } else {
        duin::SetActiveCamera(nullptr);
    }
}

void EditorCamera::PhysicsUpdate(double delta)
{

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
