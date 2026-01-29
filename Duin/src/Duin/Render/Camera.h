/**
 * @file Camera.h
 * @brief 3D camera for rendering.
 * @ingroup Render_Camera
 */

#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"

#include <unordered_map>

namespace duin
{

/**
 * @struct Camera
 * @brief 3D camera with position, target, and orientation.
 * @ingroup Render_Camera
 *
 * Uses an orthogonal axis system derived from position, target, and up reference:
 * - FORWARD = normalize(target - position)
 * - RIGHT = normalize(FORWARD × up_reference)
 * - UP = normalize(RIGHT × FORWARD)
 *
 * Coordinate system: Right-hand rule, +Z toward viewer (same as Godot).
 */
struct Camera
{
    UUID uuid;
    Vector3 position;
    Vector3 target;

    Vector3 up;
    float fovy = 72.0f;

    Camera()
    {
    }
    Camera(UUID uuid, Vector3 position, Vector3 target, Vector3 up_, float fovy)
        : uuid(uuid), position(position), target(target), up(Vector3()), fovy(fovy)
    {
        Vector3 forward = Vector3NormalizeF(Vector3Subtract(target, position));
        Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, up_));
        this->up = Vector3NormalizeF(Vector3CrossProduct(right, forward));
    }

    Camera(UUID uuid) : uuid(uuid)
    {
    }
    void SetAxis(Vector3 position, Vector3 target, Vector3 up_)
    {
        this->position = position;
        this->target = target;

        Vector3 forward = Vector3NormalizeF(Vector3Subtract(target, position));
        Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, up_));
        this->up = Vector3NormalizeF(Vector3CrossProduct(right, forward));
    }
    int IsValid() const
    {
        return uuid != UUID::INVALID;
    }
};

extern Camera DEFAULT_CAMERA; ///< Default camera instance.

/** @brief Sets the active camera for rendering. */
void SetActiveCamera(Camera *camera);
/** @brief Returns the current active camera. */
Camera *GetActiveCamera();

/**
 * @name Camera Direction Queries
 * @{
 */
Vector3 GetCameraForward(Camera *camera);
Vector3 GetCameraUp(Camera *camera);
Vector3 GetCameraRight(Camera *camera);
/** @} */

/**
 * @name Camera Movement
 * @{
 */
void CameraMoveForward(Camera *camera, float distance);
void CameraMoveUp(Camera *camera, float distance);
void CameraMoveRight(Camera *camera, float distance);
void CameraMoveToTarget(Camera *camera, float delta);
/** @} */

/**
 * @name Camera Rotation
 * @{
 */
/** @brief Rotates camera around vertical axis. */
void CameraYaw(Camera *camera, float angle, bool rotateAroundTarget = false);
/** @brief Rotates camera around horizontal axis. */
void CameraPitch(Camera *camera, float angle, bool lockView = true, bool rotateAroundTarget = false,
                 bool rotateUp = false);
/** @brief Rotates camera around forward axis. */
void CameraRoll(Camera *camera, float angle);
/** @} */

/** @brief Computes view matrix from camera state. */
Matrix GetCameraViewMatrix(Camera *camera);
/** @brief Computes perspective projection matrix. */
Matrix GetCameraProjectionMatrix(Camera *camera);

void GetBGFXMatrix(Camera *camera);

} // namespace duin
