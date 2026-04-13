#include "dnpch.h"
#include "Camera.h"

#include "Duin/Core/Application.h"
#include "Duin/Core/Debug/DNLog.h"

#include <bgfx/bgfx.h>
#include <bx/math.h>

#ifndef DISTANCE_MARGIN
#define DISTANCE_MARGIN 0.1f
#endif /* DISTANCE_MARGIN */

namespace duin
{
Camera DEFAULT_CAMERA = {
    UUID(),
    {5.0f, 5.0f, 5.0f}, // Position
    {0.0f, 0.0f, 0.0f}, // Target
    {0.0f, 1.0f, 0.0f}, // Up
    60.0f               // FOVY
};
Camera *activeCamera = new Camera(UUID::INVALID);

void SetActiveCamera(Camera *camera)
{
    activeCamera = camera;
}

Camera *GetActiveCamera()
{
    if (activeCamera == nullptr || !activeCamera->IsValid())
    {
        static Camera defaultCamera;
        defaultCamera = DEFAULT_CAMERA;
        return &defaultCamera;
    }

    return activeCamera;
}

// ============================================================================
// Camera methods — delegate to standalone CameraImpl functions
// ============================================================================

void Camera::MoveForward(float distance)
{
    CameraMoveForward(impl, distance);
}
void Camera::MoveUp(float distance)
{
    CameraMoveUp(impl, distance);
}
void Camera::MoveRight(float distance)
{
    CameraMoveRight(impl, distance);
}
void Camera::MoveToTarget(float delta)
{
    CameraMoveToTarget(impl, delta);
}

void Camera::Yaw(float angle, bool rotateAroundTarget)
{
    CameraYaw(impl, angle, rotateAroundTarget);
}
void Camera::Pitch(float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    CameraPitch(impl, angle, lockView, rotateAroundTarget, rotateUp);
}
void Camera::Roll(float angle)
{
    CameraRoll(impl, angle);
}

Matrix Camera::ViewMatrix()
{
    return GetCameraViewMatrix(impl);
}
Matrix Camera::ProjectionMatrix()
{
    return GetCameraProjectionMatrix(impl);
}
void Camera::ApplyBGFXMatrix()
{
    duin::ApplyBGFXMatrix(impl);
}

// ============================================================================
// Standalone CameraImpl equivalents
// ============================================================================

Vector3 CameraForward(const CameraImpl &cameraImpl)
{
    return Vector3NormalizeF(Vector3Subtract(cameraImpl.target, cameraImpl.position));
}

Vector3 CameraRight(const CameraImpl &cameraImpl)
{
    Vector3 forward = Vector3NormalizeF(Vector3Subtract(cameraImpl.target, cameraImpl.position));
    return Vector3NormalizeF(Vector3CrossProduct(forward, cameraImpl.globalUp));
}

Vector3 CameraUp(const CameraImpl &cameraImpl)
{
    Vector3 forward = Vector3NormalizeF(Vector3Subtract(cameraImpl.target, cameraImpl.position));
    Vector3 right = Vector3NormalizeF(Vector3CrossProduct(forward, cameraImpl.globalUp));
    return Vector3NormalizeF(Vector3CrossProduct(right, forward));
}

bool IsCameraValid(const CameraImpl &cameraImpl)
{
    return cameraImpl.uuid != UUID::INVALID;
}

void CameraMoveForward(CameraImpl &cameraImpl, float distance)
{
    Vector3 forward = CameraForward(cameraImpl);
    cameraImpl.position = Vector3Add(cameraImpl.position, Vector3Scale(forward, distance));
    cameraImpl.target = Vector3Add(cameraImpl.target, Vector3Scale(forward, distance));
}

void CameraMoveUp(CameraImpl &cameraImpl, float distance)
{
    Vector3 up = CameraUp(cameraImpl);
    cameraImpl.position = Vector3Add(cameraImpl.position, Vector3Scale(up, distance));
    cameraImpl.target = Vector3Add(cameraImpl.target, Vector3Scale(up, distance));
}

void CameraMoveRight(CameraImpl &cameraImpl, float distance)
{
    Vector3 right = CameraRight(cameraImpl);
    cameraImpl.position = Vector3Add(cameraImpl.position, Vector3Scale(right, distance));
    cameraImpl.target = Vector3Add(cameraImpl.target, Vector3Scale(right, distance));
}

void CameraMoveToTarget(CameraImpl &cameraImpl, float delta)
{
    Vector3 direction = Vector3Subtract(cameraImpl.target, cameraImpl.position);
    float distance = Vector3LengthF(direction);

    if (distance > 0.0f)
    {
        direction = Vector3Scale(Vector3NormalizeF(direction), fminf(delta, distance));
        cameraImpl.position = Vector3Add(cameraImpl.position, direction);
    }
}

void CameraYaw(CameraImpl &cameraImpl, float angle, bool rotateAroundTarget)
{
    if (rotateAroundTarget)
    {
        Vector3 offset = Vector3Subtract(cameraImpl.position, cameraImpl.target);
        Vector3 rotOffset = Vector3RotateByAxisAngle(offset, Vector3::UP, angle);
        cameraImpl.position = Vector3Add(cameraImpl.target, rotOffset);
    }
    else
    {
        Vector3 forward = CameraForward(cameraImpl);
        Vector3 up = CameraUp(cameraImpl);
        Matrix rotation = MatrixRotate(up, angle);
        forward = Vector3Transform(forward, rotation);
        cameraImpl.target = Vector3Add(cameraImpl.position, forward);
    }
}

void CameraPitch(CameraImpl &cameraImpl, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
{
    if (rotateAroundTarget)
    {
        Vector3 right = CameraRight(cameraImpl);
        Matrix rotation = MatrixRotate(right, angle);
        Vector3 offset = Vector3Subtract(cameraImpl.position, cameraImpl.target);
        Vector3 rotOffset = Vector3Transform(offset, rotation);
        cameraImpl.position = Vector3Add(cameraImpl.target, rotOffset);
    }
    else
    {
        Vector3 right = CameraRight(cameraImpl);
        Vector3 forward = CameraForward(cameraImpl);
        Matrix rotation = MatrixRotate(right, angle);
        forward = Vector3Transform(forward, rotation);
        cameraImpl.target = Vector3Add(cameraImpl.position, forward);
    }
}

void CameraRoll(CameraImpl &cameraImpl, float angle)
{
    Vector3 forward = CameraForward(cameraImpl);
    Matrix rotation = MatrixRotate(forward, angle);
    cameraImpl.globalUp = Vector3Transform(cameraImpl.globalUp, rotation);
}

Matrix GetCameraViewMatrix(const CameraImpl &cameraImpl)
{
    Vector3 up = CameraUp(cameraImpl);

    bx::Vec3 eye = {cameraImpl.position.x, cameraImpl.position.y, cameraImpl.position.z};
    bx::Vec3 at = {cameraImpl.target.x, cameraImpl.target.y, cameraImpl.target.z};
    bx::Vec3 bxUp = {up.x, up.y, up.z};

    float view[16];
    bx::mtxLookAt(view, eye, at, bxUp);

    Matrix result;
    memcpy(&result, view, sizeof(float) * 16);
    return result;
}

Matrix GetCameraProjectionMatrix(const CameraImpl &cameraImpl)
{
    float aspect = (float)GetWindowWidth() / (float)GetWindowHeight();
    float proj[16];
    bx::mtxProj(proj, cameraImpl.fovy, aspect, 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);

    Matrix result;
    memcpy(&result, proj, sizeof(float) * 16);
    return result;
}

void ApplyBGFXMatrix(const CameraImpl &cameraImpl)
{
    int WINDOW_WIDTH = GetWindowWidth();
    int WINDOW_HEIGHT = GetWindowHeight();

    Vector3 up = CameraUp(cameraImpl);

    const bx::Vec3 at = {cameraImpl.target.x, cameraImpl.target.y, cameraImpl.target.z};
    const bx::Vec3 eye = {cameraImpl.position.x, cameraImpl.position.y, cameraImpl.position.z};
    const bx::Vec3 bxUp = {up.x, up.y, up.z};

    float view[16];
    bx::mtxLookAt(view, eye, at, bxUp);

    float proj[16];
    bx::mtxProj(proj, cameraImpl.fovy, float(WINDOW_WIDTH) / float(WINDOW_HEIGHT), 0.1f, 100.0f,
                bgfx::getCaps()->homogeneousDepth);

    bgfx::setViewTransform(0, view, proj);
    bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_WIDTH), uint16_t(WINDOW_HEIGHT));
}

} // namespace duin
