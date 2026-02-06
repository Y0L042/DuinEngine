#pragma once

#include <Duin/Objects/ObjectModule.h>
#include <Duin/ECS/ECSModule.h>

#include "Singletons.h"
#include "GameObjects/Player/Player.h"

#include <memory>

class Player : public duin::GameObject
{
  public:
    void Ready() override;
    void OnEvent(duin::Event e) override;
    void PhysicsUpdate(double delta) override;
    void DrawUI() override;

  private:
};

extern flecs::entity player;
extern flecs::entity cameraRoot;
extern flecs::entity playerCamera;
extern std::shared_ptr<duin::GameStateMachine> playerStateMachine;
