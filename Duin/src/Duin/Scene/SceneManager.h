#pragma once

#include <functional>
#include "Duin/Core/Events/Event.h"

namespace duin {
    class UUID;
    class SceneManager
    {
        public:
            static SceneManager& Get();

            SceneManager() = default;
            ~SceneManager() = default;

            void ExecuteReady();
            void ExecuteHandleInput(InputEvent event);
            void ExecuteUpdate(double delta);
            void ExecutePhysicsUpdate(double delta);
            void ExecuteDraw();
            void ExecuteDrawUI();
            void ExecuteDebug();

            void RegisterReady(UUID uuid, std::function<void()> callback);
            void RegisterHandleInput(UUID uuid, std::function<void(InputEvent)> callback);
            void RegisterUpdate(UUID uuid, std::function<void(double)> callback);
            void RegisterPhysicsUpdate(UUID uuid, std::function<void(double)> callback);
            void RegisterDraw(UUID uuid, std::function<void()> callback);
            void RegisterDrawUI(UUID uuid, std::function<void()> callback);
            void RegisterDebug(UUID uuid, std::function<void()> callback);

        private:
    };
}
