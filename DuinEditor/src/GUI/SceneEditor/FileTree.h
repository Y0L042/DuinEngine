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
    duin::Signal<FSNode *> onSceneSelect;

    FileTree();
    FileTree(State_SceneEditor *sceneEditor);
    ~FileTree();

    void Init() override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;

    void SetSceneEditor(State_SceneEditor *sceneEditor);
    void SetFileManager(FileManager *fm);

    void DrawNode(FSNode *node, const std::string &nodeLabel);

  private:
    ImGuiWindowFlags imguiWindowFlags = 0;
    State_SceneEditor *sceneEditor = nullptr;
    FileManager *fm = nullptr;
    FSNode *selectedNode = nullptr;
};