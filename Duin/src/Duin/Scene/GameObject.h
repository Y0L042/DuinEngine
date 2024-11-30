#pragma once

#include "Duin/Core/Utils/UUID.h"

namespace duin {
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

		bool operator==(GameObject& other);

    private:
        UUID uuid;

    };
}
