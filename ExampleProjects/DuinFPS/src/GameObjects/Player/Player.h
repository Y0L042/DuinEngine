#pragma once

#include <Duin/Scene/SceneModule.h>

#include "Singletons.h"
#include "GameObjects/Player/Player.h"

#include <Duin/ECS/ECSModule.h>


class Player : public duin::GameObject
{
    public:
        void Ready() override;
        void PhysicsUpdate(double delta) override;

    private:
        flecs::entity player;
        flecs::entity cameraRoot;
        flecs::entity playerCamera;
};
