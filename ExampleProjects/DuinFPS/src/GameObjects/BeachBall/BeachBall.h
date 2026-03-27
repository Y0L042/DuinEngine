#pragma once

#include <Duin/Objects/ObjectModule.h>
#include <Duin/ECS/ECSModule.h>

#include "Singletons.h"

class BeachBall : public duin::GameObject
{
  public:
    BeachBall(duin::GameWorld *world) : world(world)
    {
    }

    void Ready() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

  private:
    duin::Entity entity;
    duin::GameWorld *world = nullptr;
    std::shared_ptr<duin::DynamicBody> body;
};
