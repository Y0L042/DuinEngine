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
            : uuid(uuid), position(position), target(target), up(up), fovy(fovy) {}
        Camera(UUID uuid) : uuid(uuid) {}
        int IsValid() { return uuid != UUID::INVALID; }
    };

    void SetActiveCamera(Camera *camera);
    Camera* GetActiveCamera();
    Vector3 GetCameraForward(Camera *camera);
    Vector3 GetCameraUp(Camera *camera);

    // Camera movement
    void CameraMoveForward(Camera *camera, float distance);
    void CameraMoveUp(Camera *camera, float distance);
    void CameraMoveRight(Camera *camera, float distance);
    void CameraMoveToTarget(Camera *camera, float delta);

    // Camera rotation
    void CameraYaw(Camera *camera, float angle, bool rotateAroundTarget);
    void CameraPitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp);
    void CameraRoll(Camera *camera, float angle);

    void GetBGFXMatrix(Camera *camera);
}
