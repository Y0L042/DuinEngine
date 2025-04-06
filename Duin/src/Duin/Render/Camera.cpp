#include "dnpch.h"
#include "Camera.h"

namespace duin {
    Camera& Camera::Create()
    {
        UUID uuid;
        cameraMap[uuid] = Camera(uuid);

		return cameraMap[uuid];
    }

    Camera *Camera::GetActiveCamera()
    {
		if (cameraMap.contains(activeCameraUUID)) {
			return &cameraMap[activeCameraUUID];
		}
		return nullptr;
    }

    Camera::Camera() : uuid(UUID::INVALID) {}
    Camera::Camera(UUID uuid) : uuid(uuid) {}

    Camera::~Camera()
    {

    }

    UUID Camera::GetUUID() { return uuid; }

    Vector3 Camera::GetPosition() { return position; }
    void Camera::SetPosition(Vector3 newPosition)
    {
        position = newPosition;
        // TODO
    }

    Quaternion Camera::GetRotation() { return rotation; }
    void Camera::SetRotation(Quaternion newRotation)
    {
        rotation = newRotation;
        // TODO
    }
    void Camera::Delete()
    {
        if (cameraMap.contains(uuid)) {
            cameraMap.erase(uuid);
        }
    }
}
