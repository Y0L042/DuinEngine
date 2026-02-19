#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Core/Events/EventsModule.h>
#include "PlayerStateMachine.h"
#include "PlayerBlackboard.h"

class State_OnGround_Idle : public duin::GameState
{
  public:
    State_OnGround_Idle(duin::GameStateMachine &owner) : GameState(owner)
    {
        stateName = "State_OnGround_Idle";
    }
    ~State_OnGround_Idle() override = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

  private:
    PlayerBlackboard* GetBlackboard() { return static_cast<PlayerStateMachine*>(&owner)->GetBlackboard(); }
};
