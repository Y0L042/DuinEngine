#pragma once
#include <external/imgui.h>
#include <external/imguizmo/ImGuizmo.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Objects/ObjectModule.h>
#include <GameObjects/EditorCamera.h>
#include <memory>
#include "EditorWorld.h"

#include "GUI/EditorPanel.h"

class State_SceneEditor;
class SceneViewport : public duin::GameObject
{
  public:
    SceneViewport();
    ~SceneViewport();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void StartTextureRender();
    void EndTextureRender();
    void SetActiveWorld(std::weak_ptr<duin::GameWorld> world);

  private:
    ImGuiWindowFlags imguiWindowFlags;

    std::shared_ptr<EditorCamera> mainCamera;
    duin::RenderTexture renderTarget;

    std::weak_ptr<duin::GameWorld> activeWorld;

    // Gizmo test state
    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
    float gizmoMatrix[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1,
    };
};
