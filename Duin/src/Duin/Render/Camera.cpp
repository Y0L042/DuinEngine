#include "dnpch.h"
#include "Camera.h"

#include "Duin/Core/Application.h"
#include "Duin/Core/Debug/DNLog.h"

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace duin {
    Camera* activeCamera = new Camera(UUID::INVALID);

#define VALIDATE_CAMERA if (!camera || !camera->IsValid()) { return; }

    void SetActiveCamera(Camera* camera)
    {
        activeCamera = camera;
    }

    Camera* GetActiveCamera()
    {
        return activeCamera;
    }

    // Camera movement
    void CameraMoveForward(Camera* camera, float distance)
    {

    }

    void CameraMoveUp(Camera* camera, float distance)
    {

    }

    void CameraMoveRight(Camera* camera, float distance)
    {

    }

    void CameraMoveToTarget(Camera* camera, float delta)
    {

    }


    // Camera rotation
    void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget)
    {

    }

    void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
    {

    }

    void CameraRoll(Camera* camera, float angle)
    {

    }

    void GetBGFXMatrix(Camera* camera)
    {
        VALIDATE_CAMERA

        int WINDOW_WIDTH = GetWindowWidth();
        int WINDOW_HEIGHT = GetWindowHeight();

        Vector3 target = camera->target;
        Vector3 position = camera->position;

        const bx::Vec3 at = { target.x, target.y, target.z };
        const bx::Vec3 eye = { position.x, position.y, position.z };

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_WIDTH), uint16_t(WINDOW_HEIGHT));
    }


}
