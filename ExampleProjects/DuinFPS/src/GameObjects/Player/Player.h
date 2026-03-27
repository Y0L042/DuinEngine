#pragma once

#include <Duin/Objects/ObjectModule.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/ECS/GameWorld.h>

#include "PlayerBlackboard.h"
#include "PlayerStateMachine.h"

#include <memory>

class Player : public duin::GameObject
{
  public:
    Player(duin::GameWorld* world) : world(world) {}

    void Init() override;
    void Ready() override;
    void OnEvent(duin::Event e) override;
    void PhysicsUpdate(double delta) override;
    void DrawUI() override;

  private:
    duin::GameWorld* world = nullptr;
    duin::Entity player;
    duin::Entity cameraRoot;
    duin::Entity playerCamera;
    PlayerBlackboard blackboard;
    std::shared_ptr<PlayerStateMachine> playerStateMachine;
};
