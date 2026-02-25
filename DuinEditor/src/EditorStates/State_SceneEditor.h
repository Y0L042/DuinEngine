#pragma once
#include <memory>
#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Scene/SceneBuilder.h>

#include "Project.h"
#include "FileManager.h"
#include "SceneManager.h"

// GUI elements
#include "../GUI/SceneEditor/SceneTabs.h"
#include "../GUI/SceneEditor/SceneTree.h"
#include "../GUI/SceneEditor/SceneViewport.h"
#include "../GUI/SceneEditor/FileTree.h"
#include "../GUI/SceneEditor/EntityProperties.h"

class State_SceneEditor : public duin::GameState
{
  public:
    duin::Signal<FileManager *> onUpdateFileManager;
    duin::Signal<duin::PackedScene &> onSetActiveScene;
    duin::Signal<duin::Entity> onSelectEntity;

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

    duin::PackedScene &LoadSceneFromFile(const FSNode *sceneFile);
    void SetActiveScene(duin::PackedScene &scene);

  private:
    std::shared_ptr<SceneTabs> sceneTabs;
    std::shared_ptr<SceneTree> sceneTree;
    std::shared_ptr<SceneViewport> sceneViewport;
    std::shared_ptr<FileTree> fileTree;
    std::shared_ptr<EntityProperties> entityProperties;

    std::function<void(void)> queuedNewScene;

    FileManager fileManager;
    SceneManager sceneManager;
    std::unique_ptr<duin::GameWorld> gameWorld;
    std::unique_ptr<duin::SceneBuilder> sceneBuilder;
};
