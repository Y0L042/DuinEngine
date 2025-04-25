#include "dnpch.h"
#include "Camera.h"

#include "Duin/Core/Application.h"
#include "Duin/Core/Debug/DNLog.h"

#include <bgfx/bgfx.h>
#include <bx/math.h>

#define VALIDATE_CAMERA(camera, RETURN) { if (camera == nullptr|| !camera->IsValid()) { return RETURN; } }

#ifndef DISTANCE_MARGIN 
#define DISTANCE_MARGIN 0.1f
#endif /* DISTANCE_MARGIN */

namespace duin {
    Camera* activeCamera = new Camera(UUID::INVALID);
    bool rotateAroundTarget = false;

    Camera DEFAULT_CAMERA = {
        UUID(),
        {0.0f, 2.0f, -5.0f}, // Position
        {0.0f, 0.0f, 0.0f},  // Target
        {0.0f, 1.0f, 0.0f},  // Up
        60.0f // FOVY
    };


    void SetActiveCamera(Camera* camera)
    {
        activeCamera = camera;
    }

    Camera* GetActiveCamera()
    {
        if (activeCamera == nullptr || !activeCamera->IsValid()) {
            // Default camera values must not be changed
            static Camera defaultCamera;
            defaultCamera = DEFAULT_CAMERA;

			//DN_CORE_WARN("No active camera set, using default camera.");

            return &defaultCamera;
        }

        return activeCamera;
    }

    Vector3 GetCameraForward(Camera *camera)
    {
        if (camera == nullptr || !camera->IsValid()) { return Vector3(); }
        return Vector3NormalizeF(Vector3Subtract(camera->target, camera->position));
    }

    Vector3 GetCameraUp(Camera *camera)
    {
        if (camera == nullptr || !camera->IsValid()) { return Vector3(); }
        return Vector3NormalizeF(camera->up);
    }

    Vector3 GetCameraRight(Camera *camera)
    {
        if (camera == nullptr || !camera->IsValid()) { return Vector3(); }
        Vector3 forward = Vector3NormalizeF(Vector3Subtract(camera->target, camera->position));
        Vector3 up = Vector3NormalizeF(camera->up);
        return Vector3NormalizeF(Vector3CrossProduct(forward, up));
    }

    // Camera movement
    void CameraMoveForward(Camera* camera, float distance)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 forward = GetCameraForward(camera);
        camera->position = Vector3Add(camera->position, Vector3Scale(forward, distance));
        camera->target = Vector3Add(camera->target, Vector3Scale(forward, distance));
    }

    void CameraMoveUp(Camera* camera, float distance)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 up = GetCameraUp(camera);
        camera->position = Vector3Add(camera->position, Vector3Scale(up, distance));
        camera->target = Vector3Add(camera->target, Vector3Scale(up, distance));
    }

    void CameraMoveRight(Camera* camera, float distance)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 right = GetCameraRight(camera);
        camera->position = Vector3Add(camera->position, Vector3Scale(right, distance));
        camera->target = Vector3Add(camera->target, Vector3Scale(right, distance));
    }

    void CameraMoveToTarget(Camera* camera, float delta)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 direction = Vector3Subtract(camera->target, camera->position);
        float distance = Vector3LengthF(direction);

        if (distance > 0.0f) {
            direction = Vector3Scale(Vector3NormalizeF(direction), fminf(delta, distance));
            camera->position = Vector3Add(camera->position, direction);
        }
    }


    // Camera rotation
    void CameraYaw(Camera* camera, float angle, bool rotateAroundTarget)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 forward = GetCameraForward(camera);
        Vector3 up = GetCameraUp(camera);

        Matrix rotation = MatrixRotate(up, angle);
        
        if (rotateAroundTarget) {
            Vector3 offset = Vector3Subtract(camera->position, camera->target);
            offset = Vector3Transform(offset, rotation);
            camera->position = Vector3Add(camera->target, offset);
        } else {

            // Normal Operation
            forward = Vector3Transform(forward, rotation);
            camera->target = Vector3Add(camera->position, forward);
        }
        
        camera->up = Vector3Transform(camera->up, rotation);
    }

    void CameraPitch(Camera* camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 forward = GetCameraForward(camera);
        Vector3 right = GetCameraRight(camera);
        
        Matrix rotation = MatrixRotate(right, angle);
        
        if (rotateAroundTarget) {
            Vector3 offset = Vector3Subtract(camera->position, camera->target);
            offset = Vector3Transform(offset, rotation);
            camera->position = Vector3Add(camera->target, offset);
        } else {
            // Normal Operation
            forward = Vector3Transform(forward, rotation);
            camera->target = Vector3Add(camera->position, forward);
        }
        
        if (rotateUp) {
            camera->up = Vector3Transform(camera->up, rotation);
        }
        
        // Prevent camera flipping
        if (!lockView) {
            float dot = Vector3DotProduct(forward, camera->up);
            if (fabsf(dot) > 0.99f) {
                angle = (dot > 0) ? -0.1f : 0.1f;
                rotation = MatrixRotate(right, angle);
                forward = Vector3Transform(forward, rotation);
                camera->target = Vector3Add(camera->position, forward);
            }
        }
    }

    void CameraRoll(Camera* camera, float angle)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }
        Vector3 forward = GetCameraForward(camera);
        Matrix rotation = MatrixRotate(forward, angle);
        camera->up = Vector3Transform(camera->up, rotation);
    }

    Matrix GetCameraViewMatrix(Camera *camera)
    {
        VALIDATE_CAMERA(camera, MatrixIdentity());

        bx::Vec3 eye = { camera->position.x, camera->position.y, camera->position.z };
        bx::Vec3 at = { camera->target.x, camera->target.y, camera->target.z };
        bx::Vec3 up = { camera->up.x, camera->up.y, camera->up.z };

        float view[16];
        bx::mtxLookAt(view, eye, at, up);

        Matrix result;
        memcpy(&result, view, sizeof(float) * 16);
        return result;

        //Vector3 forward = Vector3Normalize(Vector3Subtract(camera->target, camera->position));
        //Vector3 right = Vector3Normalize(Vector3CrossProduct(forward, camera->up));
        //Vector3 realUp = Vector3Normalize(Vector3CrossProduct(right, forward));
        //
        //return MatrixLookAt(camera->position, camera->target, realUp);
    }

    Matrix GetCameraProjectionMatrix(Camera *camera)
    {
        VALIDATE_CAMERA(camera, MatrixIdentity());

        float aspect = (float)GetWindowWidth() / (float)GetWindowHeight();
        float proj[16];
        bx::mtxProj(proj, camera->fovy, aspect, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        Matrix result;
        memcpy(&result, proj, sizeof(float) * 16);
        return result;

        //float aspect = (float)GetWindowWidth() / (float)GetWindowHeight();
        //return MatrixPerspective(camera->fovy * DEG2RAD, aspect, 0.1f, 100.0f);
    }

    void GetBGFXMatrix(Camera* camera)
    {
        if (camera == nullptr || !camera->IsValid()) { return; }

        int WINDOW_WIDTH = GetWindowWidth();
        int WINDOW_HEIGHT = GetWindowHeight();

        Vector3 cTarget = camera->target;
        Vector3 cPosition = camera->position;
        Vector3 cUp = camera->up;
        float fovy = camera->fovy;

        const bx::Vec3 at = { cTarget.x, cTarget.y, cTarget.z };
        const bx::Vec3 eye = { cPosition.x, cPosition.y, cPosition.z };
        const bx::Vec3 up = { cUp.x, cUp.y, cUp.z };

        float view[16];
        bx::mtxLookAt(view, eye, at, up);

        float proj[16];
        bx::mtxProj(proj, fovy, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

        bgfx::setViewTransform(0, view, proj);
        bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_WIDTH), uint16_t(WINDOW_HEIGHT));
    }


}
