#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include <unordered_map>

namespace duin {
    class Camera 
    {
        public:
            static Camera& Create();
            static Camera *GetActiveCamera();

            Camera();
            Camera(UUID uuid);
            ~Camera();

            UUID GetUUID();
            Vector3 GetPosition();
            void SetPosition(Vector3 newPosition);
            Quaternion GetRotation();
            void SetRotation(Quaternion newRotation);

            void Delete();


        private:
            static std::unordered_map<UUID, Camera> cameraMap;
            static UUID activeCameraUUID;

            UUID uuid;
            Vector3 position;
            Quaternion rotation;
            Vector3 cameraUp;// = Vector3::UP;

    

    };
}
