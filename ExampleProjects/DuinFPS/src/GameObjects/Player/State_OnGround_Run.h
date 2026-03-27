#pragma once

#include "PlayerState.h"

class State_OnGround_Run : public PlayerState
{
  public:
    State_OnGround_Run(duin::GameStateMachine &owner) : PlayerState(owner)
    {
        stateName = "State_OnGround_Run";
    }
    ~State_OnGround_Run() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;
};
