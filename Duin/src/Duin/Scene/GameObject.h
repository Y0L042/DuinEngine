#pragma once

#include "Duin/Core/Utils/UUID.h"

namespace duin {
    class SceneManager;
    class GameObject
    {
        public:
            GameObject();
            virtual ~GameObject();

            virtual void Ready();
            virtual void HandleInput();
            virtual void Update(double delta);
            virtual void PhysicsUpdate(double delta);
            virtual void Draw();
            virtual void DrawUI();
            virtual void Debug();

            UUID GetUUID();
            
            SceneManager& GetSceneManager();

            bool operator==(GameObject& other);

        private:
            friend class SceneManager;

            UUID uuid;

            void RegisterObject();
            void DeregisterObject();
    };
}
