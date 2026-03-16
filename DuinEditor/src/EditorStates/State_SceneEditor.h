#pragma once
#include <memory>
#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Scene/SceneBuilder.h>

#include "Project.h"
#include "FileManager.h"
#include "SceneManager.h"
#include "EditorWorld.h"
#include "EditorContext.h"
#include "Scene.h"
#include "CommandManager.h"
#include "GUI/SceneEditor/EditorUIObjects.h"

// GUI elements
#include "GUI/SceneEditor/SceneTabs.h"
#include "GUI/SceneEditor/SceneTree.h"
#include "GUI/SceneEditor/SceneViewport.h"
#include "GUI/SceneEditor/FileTree.h"
#include "GUI/SceneEditor/EntityProperties.h"
#include <Duin/Core/Signals/Signal.h>

struct UIObjects
{
    std::shared_ptr<FileTree> fileTree;
    std::shared_ptr<SceneTree> sceneTree;
    std::shared_ptr<EntityProperties> entityProperties;
    std::shared_ptr<SceneTabs> sceneTabs;
    std::shared_ptr<SceneViewport> sceneViewport;
};

struct Signals
{
    duin::Signal<std::shared_ptr<FileManager>> onUpdateFileManager;
    duin::Signal<std::shared_ptr<Scene>> onSetActiveScene;
    duin::Signal<std::shared_ptr<Scene>> onSetSceneFromFile;
    duin::Signal<std::weak_ptr<duin::GameWorld>> onWorldChange;
};

class State_SceneEditor : public duin::GameState
{
  public:
    State_SceneEditor(duin::GameStateMachine &sm) : duin::GameState(sm) {};
    ~State_SceneEditor() = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

    void InitializeManagers();
    void ProcessProject(Project project);
    void CreateUIObjects();
    void ConnectSignals();
    void SetupInput();

    void LoadSceneFromFile(const FSNode *sceneFile);
    void EnsureInstantiatedScene(std::weak_ptr<FSNode> sceneFile);
    void CacheActiveScene(std::weak_ptr<Scene> scene);

  private:
    Signals signals;
    UIObjects uiObjects;
    std::shared_ptr<FileManager> fileManager;
    std::shared_ptr<SceneManager> sceneManager;
    std::weak_ptr<Scene> cachedActiveScene;
    std::shared_ptr<duin::ScopedConnection> onSetActiveSceneHandle;

    std::function<void(void)> queuedNewScene;
    std::shared_ptr<CommandManager> commandManager = std::make_shared<CommandManager>();
};
