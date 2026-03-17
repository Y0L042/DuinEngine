#pragma once

#include <Duin/Objects/GameStateMachine.h>

class SceneBuilder : public duin::GameState
{
  public:
    SceneBuilder(duin::GameStateMachine &sm);
    ~SceneBuilder();

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
    void Test_Misc();
;

  private:
};