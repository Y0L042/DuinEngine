#pragma once

#include "PlayerState.h"

class State_InAir : public PlayerState
{
  public:
    State_InAir(duin::GameStateMachine &owner) : PlayerState(owner)
    {
        stateName = "State_InAir";
    }
    ~State_InAir() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

  private:
    std::shared_ptr<PlayerStateMachine> inAirStateMachine;
};
