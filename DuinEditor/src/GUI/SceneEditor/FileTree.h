#pragma once
#include <external/imgui.h>
#include <Duin/Objects/ObjectModule.h>
#include <Duin/Core/Signals/Signal.h>
#include <memory>

class State_SceneEditor;
class FileManager;
class FSNode;
class FileTree : public duin::GameObject
{
  public:
    duin::Signal<std::weak_ptr<FSNode>> onSceneSelect;

    FileTree();
    ~FileTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void SetFileManager(std::shared_ptr<FileManager> fm);

    void DrawNode(std::weak_ptr<FSNode> node, const std::string &nodeLabel);

  private:
    ImGuiWindowFlags imguiWindowFlags = 0;
    std::shared_ptr<FileManager> fm = nullptr;
    std::weak_ptr<FSNode> selectedNode;
};