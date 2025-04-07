#include "dnpch.h"
#include "Camera.h"

#include "Duin/Core/Application.h"
#include "Duin/Core/Debug/DNLog.h"

#include <bgfx/bgfx.h>
#include <bx/math.h>

namespace duin {
    Camera *activeCamera = new Camera(UUID::INVALID);

#define VALIDATE_CAMERA if (!camera || !camera->IsValid()) { return; }

    void SetActiveCamera(Camera *camera)
    {
        activeCamera = camera;
    }

    Camera *GetActiveCamera()
    {
        return activeCamera;
    }

    // Camera movement
    void CameraMoveForward(Camera *camera, float distance)
	{
	
	}

    void CameraMoveUp(Camera *camera, float distance)
	{
	
	}

    void CameraMoveRight(Camera *camera, float distance)
	{
	
	}

    void CameraMoveToTarget(Camera *camera, float delta)
	{
	
	}


    // Camera rotation
    void CameraYaw(Camera *camera, float angle, bool rotateAroundTarget)
	{
	
	}

    void CameraPitch(Camera *camera, float angle, bool lockView, bool rotateAroundTarget, bool rotateUp)
	{
	
	}

    void CameraRoll(Camera *camera, float angle)
	{
	
	}

    void GetBGFXMatrix(Camera *camera)
    {
        VALIDATE_CAMERA

        int WINDOW_WIDTH = GetWindowWidth();
        int WINDOW_HEIGHT = GetWindowHeight();

        Vector3 target = camera->target;
        Vector3 position = camera->position;
        
        const bx::Vec3 at = { target.x, target.y, target.z };
        const bx::Vec3 eye  = { position.x, position.y, position.z };

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH)/float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_WIDTH), uint16_t(WINDOW_HEIGHT));
    }


    std::unordered_map<UUID, Camera3D> Camera3D::cameraMap;
    UUID Camera3D::activeCamera3DUUID;

    Camera3D* Camera3D::Create()
    {
        UUID uuid;
        bool setAsActive = false;
        setAsActive = cameraMap.empty();
        cameraMap.try_emplace(uuid, Camera3D(uuid));
        if (setAsActive) {
            SetActiveCamera3D(uuid);
        }

		return &cameraMap[uuid];
    }

    Camera3D& Camera3D::GetActiveCamera3D()
    {
		if (cameraMap.contains(activeCamera3DUUID)) {
			return cameraMap[activeCamera3DUUID];
		}
		static Camera3D invalidCamera3D(UUID::INVALID);
		return invalidCamera3D;
    }

    void Camera3D::SetActiveCamera3D(Camera3D* camera)
    {
        if (cameraMap.contains(camera->uuid)) {
            activeCamera3DUUID = camera->uuid;
        } else {
            DN_CORE_WARN("Camera3D is not properly registered/created! Use the static Create() function.");
        }
    }

    void Camera3D::SetActiveCamera3D(Camera3D& camera)
    {
        if (cameraMap.contains(camera.uuid)) {
            activeCamera3DUUID = camera.uuid;
        } else {
            DN_CORE_WARN("Camera3D is not properly registered/created! Use the static Create() function.");
        }
    }

    void Camera3D::SetActiveCamera3D(UUID uuid)
    {
        if (cameraMap.contains(uuid)) {
            activeCamera3DUUID = uuid;
        } else {
            DN_CORE_WARN("UUID of camera is not properly registered/created! Use the static Create() function.");
        }
    }


    Camera3D::Camera3D() : uuid(UUID::INVALID) {}
    Camera3D::Camera3D(UUID uuid) : uuid(uuid) {}

    Camera3D::~Camera3D() {}

    UUID Camera3D::GetUUID() { return uuid; }
    int Camera3D::IsValid() { return uuid != UUID::INVALID; }

    Vector3 Camera3D::GetPosition() { return position; }
    void Camera3D::SetPosition(Vector3 newPosition)
    {
        position = newPosition;
        // TODO
    }

    Quaternion Camera3D::GetRotation() { return rotation; }
    void Camera3D::SetRotation(Quaternion newRotation)
    {
        rotation = newRotation;
        // TODO
    }
    void Camera3D::Delete()
    {
        if (cameraMap.contains(uuid)) {
            cameraMap.erase(uuid);
        }
    }

    void Camera3D::LookAt(Vector3 target)
    {

    }

    void Camera3D::SetBGFXView()
    {
        int WINDOW_WIDTH = GetWindowWidth();
        int WINDOW_HEIGHT = GetWindowHeight();

        
        const bx::Vec3 at = { 0.0f, 0.0f, 0.0f };
        const bx::Vec3 eye  = { position.x, position.y, position.z };

        float view[16];
        bx::mtxLookAt(view, eye, at);

        float proj[16];
        bx::mtxProj(proj, 60.0f, float(WINDOW_WIDTH)/float(WINDOW_HEIGHT), 0.1f, 100.0f, bgfx::getCaps()->homogeneousDepth);
        bgfx::setViewTransform(0, view, proj);

        // Set view 0 default viewport.
        bgfx::setViewRect(0, 0, 0, uint16_t(WINDOW_WIDTH), uint16_t(WINDOW_HEIGHT));
    }
}
