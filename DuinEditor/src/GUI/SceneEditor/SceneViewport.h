#pragma once
#include <external/imgui.h>
#include <external/imguizmo/ImGuizmo.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Objects/ObjectModule.h>
#include <GameObjects/EditorCamera.h>
#include <memory>
#include "EditorWorld.h"
#include "Scene.h"
#include "GUI/UIObject.h"

#include "GUI/EditorPanel.h"

class State_SceneEditor;
class SceneViewport : public duin::GameObject, public UIObject
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
    void CacheActiveScene(std::weak_ptr<Scene> scene);

  private:
    ImGuiWindowFlags imguiWindowFlags;
    std::weak_ptr<Scene> cachedActiveScene;

    std::shared_ptr<EditorCamera> mainCamera;
    duin::RenderTexture renderTarget;

    // Gizmo test state
    ImGuizmo::OPERATION gizmoOperation = ImGuizmo::TRANSLATE;
    float gizmoMatrix[16] = {
        1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1,
    };
};
