/**
 * @file Camera.h
 * @brief 3D camera for rendering.
 * @ingroup Render_Camera
 */

#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "rfl.hpp"

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
struct CameraImpl
{
    UUID uuid;
    Vector3 position;
    Vector3 target;
    Vector3 globalUp;
    float fovy;
};

class Camera
{
  public:
    Camera()
    {
    }

    Camera(UUID uuid, Vector3 position, Vector3 target, Vector3 globalUp, float fovy)
        : impl(uuid, position, target, globalUp, fovy)
    {
    }

    Camera(UUID uuid) : impl(uuid)
    {
    }

    CameraImpl GetImpl() const
    {
        return impl;
    }

    Vector3 Forward() const
    {
        return Vector3NormalizeF(Vector3Subtract(impl.target, impl.position));
    }

    Vector3 Right() const
    {
        Vector3 forward = Vector3NormalizeF(Vector3Subtract(impl.target, impl.position));
        return Vector3NormalizeF(Vector3CrossProduct(forward, impl.globalUp));
    }

    Vector3 Up() const
    {
        Vector3 forward = Vector3NormalizeF(Vector3Subtract(impl.target, impl.position));
        Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, impl.globalUp));
        return Vector3NormalizeF(Vector3CrossProduct(right, forward));
    }

    UUID GetUUID() const
    {
        return impl.uuid;
    }

    void SetUUID(UUID uuid)
    {
        impl.uuid = uuid;
    }

    Vector3 GetPosition() const
    {
        return impl.position;
    }

    void SetPosition(Vector3 pos)
    {
        impl.position = pos;
    }

    Vector3 GetTarget() const
    {
        return impl.target;
    }

    void SetTarget(Vector3 target)
    {
        impl.target = target;
    }

    Vector3 GetGlobalUp() const
    {
        return impl.globalUp;
    }

    void SetGlobalUp(Vector3 up)
    {
        impl.globalUp = up;
    }

    float GetFOVY() const
    {
        return impl.fovy;
    }

    void SetFOVY(float fov)
    {
        impl.fovy = fov;
    }

    int IsValid() const
    {
        return impl.uuid != UUID::INVALID;
    }

    /**
     * @name Camera Movement
     * @{
     */
    void MoveForward(float distance);
    void MoveUp(float distance);
    void MoveRight(float distance);
    void MoveToTarget(float delta);
    /** @} */

    /**
     * @name Camera Rotation
     * @{
     */
    /** @brief Rotates camera around vertical axis. */
    void Yaw(float angle, bool rotateAroundTarget = false);
    /** @brief Rotates camera around horizontal axis. */
    void Pitch(float angle, bool lockView = true, bool rotateAroundTarget = false, bool rotateUp = false);
    /** @brief Rotates camera around forward axis. */
    void Roll(float angle);
    /** @} */

    /** @brief Computes view matrix from camera state. */
    Matrix ViewMatrix();
    /** @brief Computes perspective projection matrix. */
    Matrix ProjectionMatrix();

    void ApplyBGFXMatrix();

    using ReflectionType = CameraImpl;
    Camera(const ReflectionType &impl) : impl(impl)
    {
    }
    ReflectionType reflection() const
    {
        return impl;
    }

  private:
    CameraImpl impl;
};

extern Camera DEFAULT_CAMERA; ///< Default camera instance.

/** @brief Sets the active camera for rendering. */
void SetActiveCamera(Camera *camera);
/** @brief Returns the current active camera. */
Camera *GetActiveCamera();

Vector3 CameraForward(const CameraImpl &cameraImpl);
Vector3 CameraRight(const CameraImpl &cameraImpl);
Vector3 CameraUp(const CameraImpl &cameraImpl);
bool IsCameraValid(const CameraImpl &cameraImpl);

/**
 * @name CameraImpl Movement
 * @{
 */
void CameraMoveForward(CameraImpl &cameraImpl, float distance);
void CameraMoveUp(CameraImpl &cameraImpl, float distance);
void CameraMoveRight(CameraImpl &cameraImpl, float distance);
void CameraMoveToTarget(CameraImpl &cameraImpl, float delta);
/** @} */

/**
 * @name CameraImpl Rotation
 * @{
 */
/** @brief Rotates camera around vertical axis. */
void CameraYaw(CameraImpl &cameraImpl, float angle, bool rotateAroundTarget = false);
/** @brief Rotates camera around horizontal axis. */
void CameraPitch(CameraImpl &cameraImpl, float angle, bool lockView = true, bool rotateAroundTarget = false,
                 bool rotateUp = false);
/** @brief Rotates camera around forward axis. */
void CameraRoll(CameraImpl &cameraImpl, float angle);
/** @} */

/** @brief Computes view matrix from camera state. */
Matrix GetCameraViewMatrix(const CameraImpl &cameraImpl);
/** @brief Computes perspective projection matrix. */
Matrix GetCameraProjectionMatrix(const CameraImpl &cameraImpl);

void ApplyBGFXMatrix(const CameraImpl &cameraImpl);

} // namespace duin
