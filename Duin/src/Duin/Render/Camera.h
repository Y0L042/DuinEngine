#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <unordered_map>

namespace duin {

    /**
     * @class Camera
     * @brief The struct for the primitive camera. The camera uses an orthogonal axis system of FORWARD, RIGHT, UP.
     * FORWARD is gotten from $target - position$.
     * RIGHT is derived from the cross-product of FORWARD and a user-given global up reference.
     * UP is derived from the cross-product of FORWARD and RIGHT.
     * 
     * The world axis orientation use the right-hand rule, with positive Z pointing "out of" the screen, similar to Godot.
     *
     */
    struct Camera {
        UUID uuid;
        Vector3 position;
        Vector3 target;

        Vector3 up;
        float fovy = 72.0f;

        Camera() {}
        Camera(UUID uuid, Vector3 position, Vector3 target, Vector3 up_, float fovy)
            : uuid(uuid), position(position), target(target), up(Vector3()), fovy(fovy) 
        {
            Vector3 forward = Vector3NormalizeF(Vector3Subtract(target, position));
            Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, up_));
            this->up = Vector3NormalizeF(Vector3CrossProduct(right, forward));
        }

        Camera(UUID uuid) : uuid(uuid) {}
        void SetAxis(Vector3 position, Vector3 target, Vector3 up_)
        {
            this->position = position;
            this->target = target;

            Vector3 forward = Vector3NormalizeF(Vector3Subtract(target, position));
            Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, up_));
            this->up = Vector3NormalizeF(Vector3CrossProduct(right, forward));
        }
        int IsValid() const { return uuid != UUID::INVALID; }
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
