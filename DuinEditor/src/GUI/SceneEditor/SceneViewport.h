#pragma once
#include <external/imgui.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Objects/ObjectModule.h>
#include <GameObjects/EditorCamera.h>
#include <memory>

class SceneViewport : public duin::GameObject
{
  public:
    SceneViewport();
    ~SceneViewport();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void RenderToTexture();

  private:
    std::shared_ptr<EditorCamera> mainCamera;
    duin::RenderTexture renderTarget;
    ImGuiWindowFlags imguiWindowFlags;
};