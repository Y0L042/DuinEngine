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


    class Camera3D 
    {
        public:
            static Camera3D* Create();
            static Camera3D& GetActiveCamera3D();
            static void SetActiveCamera3D(Camera3D* camera);
            static void SetActiveCamera3D(Camera3D& camera);
            static void SetActiveCamera3D(UUID uuid);

            Camera3D();
            Camera3D(UUID uuid);
            ~Camera3D();

            UUID GetUUID();
            int IsValid();
            Vector3 GetPosition();
            void SetPosition(Vector3 newPosition);
            Quaternion GetRotation();
            void SetRotation(Quaternion newRotation);

            void LookAt(Vector3 target);

            void Delete();

            void SetBGFXView();

        private:
            static std::unordered_map<UUID, Camera3D> cameraMap;
            static UUID activeCamera3DUUID;

            UUID uuid;
            Vector3 position;
            Quaternion rotation;
            Vector3 cameraUp;// = Vector3::UP;

    };
}
