#pragma once

#include <Duin/Objects/ObjectModule.h>
#include <Duin/ECS/ECSModule.h>

#include "Singletons.h"



class BeachBall : public duin::GameObject
{
    public:
        void Ready() override;
        void PhysicsUpdate(double delta) override;
        void Draw() override;
        void DrawUI() override;

    private:
        flecs::entity entity;
        std::shared_ptr<duin::DynamicBody> body;

};
