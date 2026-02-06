#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Core/Events/EventsModule.h>

class State_InAir : public duin::GameState
{
  public:
    State_InAir(duin::GameStateMachine &owner) : GameState(owner)
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
};
