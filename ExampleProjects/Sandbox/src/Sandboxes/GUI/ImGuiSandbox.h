#pragma once

#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Render/RenderModule.h>
#include <external/imgui.h>
#include <ImGuizmo.h>

class ImGuiSandbox : public duin::GameState
{
  public:
    ImGuiSandbox(duin::GameStateMachine &sm);
    ~ImGuiSandbox();

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
    void Test_ImGuizmo();

  private:
    duin::Camera camera;
    duin::RenderTexture renderTarget;

    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
    ImGuizmo::MODE      gizmoMode      = ImGuizmo::WORLD;
    bool useSnap = false;
    float snapTranslate[3] = { 1.0f, 1.0f, 1.0f };
    float snapRotate = 15.0f;
    float snapScale  = 0.5f;
    float objectMatrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
};