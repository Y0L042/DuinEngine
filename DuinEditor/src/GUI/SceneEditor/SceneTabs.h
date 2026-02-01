#pragma once
#include <external/imgui.h>
#include <Duin/Objects/ObjectModule.h>
#include <memory>

class SceneTabs : public duin::GameObject
{
  public:
    SceneTabs();
    ~SceneTabs();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
  private:
    ImGuiWindowFlags imguiWindowFlags;
};