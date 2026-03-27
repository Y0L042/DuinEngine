#pragma once

#include "PlayerState.h"

class State_OnGround : public PlayerState
{
  public:
    State_OnGround(duin::GameStateMachine &owner) : PlayerState(owner)
    {
        stateName = "State_OnGround";
    }
    ~State_OnGround() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

  private:
    std::shared_ptr<PlayerStateMachine> onGroundStateMachine;
};
