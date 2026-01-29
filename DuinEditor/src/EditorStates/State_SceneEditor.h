#pragma once
#include <memory>
#include <Duin/Objects/GameStateMachine.h>

// GUI elements
#include "../GUI/SceneEditor/SceneViewport.h"

class State_SceneEditor : public duin::GameState
{
  public:
    State_SceneEditor(duin::GameStateMachine &sm) : duin::GameState(sm) {};
    ~State_SceneEditor() = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

  private:
    std::shared_ptr<SceneViewport> sceneViewport;
};
