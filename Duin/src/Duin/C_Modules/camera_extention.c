#include "camera_extention.h"

#include <raylib.h>
#include <raymath.h>
#include <rcamera.h>

// Function to calculate the yaw angle from the camera's direction
float CalculateCameraYaw(Camera3D *camera)
{
    Vector3 direction = GetCameraForward(camera);
    float yaw = atan2f(direction.x, direction.z);
    return yaw;
}

// Function to calculate the pitch angle from the camera's direction
float CalculateCameraPitch(Camera3D *camera)
{
    Vector3 direction = GetCameraForward(camera);
    float pitch = asinf(direction.y / Vector3Length(direction));
    return pitch;
}

