#pragma once

#include <Duin/Objects/GameStateMachine.h>

class MultiWindowSandbox : public duin::GameState
{
  public:
    MultiWindowSandbox(duin::GameStateMachine &sm);
    ~MultiWindowSandbox();

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;
    void SetPause() override;
    void SetUnpause() override;

    void Tests();

  private:
};
