#pragma once
#include <external/imgui.h>
#include <Duin/Objects/ObjectModule.h>
#include <memory>

class FileTree : public duin::GameObject
{
  public:
    FileTree();
    ~FileTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
  private:
    ImGuiWindowFlags imguiWindowFlags;
};