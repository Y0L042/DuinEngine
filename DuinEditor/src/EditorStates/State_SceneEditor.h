#pragma once
#include <memory>
#include <Duin/Objects/GameStateMachine.h>

#include "Project.h"
#include "FileManager.h"
#include "SceneManager.h"

// GUI elements
#include "../GUI/SceneEditor/SceneTabs.h"
#include "../GUI/SceneEditor/SceneTree.h"
#include "../GUI/SceneEditor/SceneViewport.h"
#include "../GUI/SceneEditor/FileTree.h"

class State_SceneEditor : public duin::GameState
{
  public:
    duin::Signal<FileManager*> onUpdateFileManager;

    State_SceneEditor(duin::GameStateMachine &sm) : duin::GameState(sm) {};
    ~State_SceneEditor() = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

    void ProcessProject(Project project);
    void CreateGUIElements();

    void LoadSceneFromFile(FSNode *sceneFile);
    void SetActiveScene();

  private:
    std::shared_ptr<SceneTabs> sceneTabs;
    std::shared_ptr<SceneTree> sceneTree;
    std::shared_ptr<SceneViewport> sceneViewport;
    std::shared_ptr<FileTree> fileTree;

    FileManager fileManager;
    SceneManager sceneManager;
};
