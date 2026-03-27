#pragma once

#include "PlayerState.h"

class State_InAir_Strafe : public PlayerState
{
  public:
    State_InAir_Strafe(duin::GameStateMachine &owner) : PlayerState(owner)
    {
        stateName = "State_InAir_Strafe";
    }
    ~State_InAir_Strafe() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;
};
