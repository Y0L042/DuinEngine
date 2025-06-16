#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <unordered_map>

namespace duin {
    struct Camera {
        UUID uuid;
        Vector3 position;
        Vector3 target;

        Vector3 up;
        float fovy;

        Camera() {}
        Camera(UUID uuid, Vector3 position, Vector3 target, Vector3 up, float fovy)
            : uuid(uuid), position(position), target(target), up(Vector3()), fovy(fovy) 
        {
            Vector3 forward = Vector3NormalizeF(Vector3Subtract(target, position));
            Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, up));
            this->up = Vector3CrossProduct(right, forward);
        }

        Camera(UUID uuid) : uuid(uuid) {}
        int IsValid() { return uuid != UUID::INVALID; }
    };

    extern Camera DEFAULT_CAMERA;

    void SetActiveCamera(Camera *camera);
    Camera* GetActiveCamera();
    Vector3 GetCameraForward(Camera *camera);
    Vector3 GetCameraUp(Camera *camera);

    Vector3 GetCameraForward(Camera *camera);
    Vector3 GetCameraUp(Camera *camera);
    Vector3 GetCameraRight(Camera *camera);

    // Camera movement
    void CameraMoveForward(Camera *camera, float distance);
    void CameraMoveUp(Camera *camera, float distance);
    void CameraMoveRight(Camera *camera, float distance);
    void CameraMoveToTarget(Camera *camera, float delta);

    // Camera rotation
    void CameraYaw(Camera *camera, float angle, bool rotateAroundTarget = false);
    void CameraPitch(Camera *camera, float angle, 
                     bool lockView = true, 
                     bool rotateAroundTarget = false, 
                     bool rotateUp = false);
    void CameraRoll(Camera *camera, float angle);

    Matrix GetCameraViewMatrix(Camera *camera);
    Matrix GetCameraProjectionMatrix(Camera *camera);

    void GetBGFXMatrix(Camera *camera);
}
