#pragma once

#include "PlayerState.h"

class State_InAir_Idle : public PlayerState
{
  public:
    State_InAir_Idle(duin::GameStateMachine &owner) : PlayerState(owner)
    {
        stateName = "State_InAir_Idle";
    }
    ~State_InAir_Idle() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;
};
