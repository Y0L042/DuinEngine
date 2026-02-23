#pragma once

#include <Duin/Objects/GameStateMachine.h>

class SandboxScene : public duin::GameState
{
  public:
    SandboxScene(duin::GameStateMachine &sm);
    ~SandboxScene();

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
    void Test_FlecsJSON();
  void Test_SceneBuilder_01();

  private:
};