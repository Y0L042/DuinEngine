#pragma once
#include <external/imgui.h>
#include <Duin/Render/RenderModule.h>
#include <Duin/Objects/ObjectModule.h>
#include <GameObjects/EditorCamera.h>
#include <memory>

class SceneTree : public duin::GameObject
{
  public:
    SceneTree();
    ~SceneTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

  private:
    ImGuiWindowFlags imguiWindowFlags;
};