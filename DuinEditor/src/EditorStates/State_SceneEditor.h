#pragma once
#include <memory>
#include <Duin/Objects/GameStateMachine.h>
#include <Duin/Scene/SceneBuilder.h>

#include "Project.h"
#include "FileManager.h"
#include "SceneManager.h"
#include "EditorWorld.h"

// GUI elements
#include "GUI/SceneEditor/SceneTabs.h"
#include "GUI/SceneEditor/SceneTree.h"
#include "GUI/SceneEditor/SceneViewport.h"
#include "GUI/SceneEditor/FileTree.h"
#include "GUI/SceneEditor/EntityProperties.h"

class State_SceneEditor : public duin::GameState
{
  public:
    struct EditorContext
    {
        std::shared_ptr<FileManager> fileManager;
        std::shared_ptr<SceneManager> sceneManager;
        std::shared_ptr<EditorWorld> editorWorld;
        std::shared_ptr<duin::SceneBuilder> sceneBuilder;
    };

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
        duin::Signal<duin::PackedScene &> onSetActiveScene;
        duin::Signal<std::weak_ptr<duin::GameWorld>> onWorldChange;
    };

    State_SceneEditor(duin::GameStateMachine &sm) : duin::GameState(sm) {};
    ~State_SceneEditor() = default;

    void Enter() override;
    void OnEvent(duin::Event e) override;
    void Update(double delta) override;
    void PhysicsUpdate(double delta) override;
    void Draw() override;
    void DrawUI() override;
    void Exit() override;

    void CreateEditorContext();
    void ProcessProject(Project project);
    void CreateUIObjects();

    duin::PackedScene &LoadSceneFromFile(const FSNode *sceneFile);
    void SetActiveScene(duin::PackedScene &scene);

  private:
    EditorContext editorContext;
    UIObjects uiObjects;
    Signals signals;

    std::function<void(void)> queuedNewScene;
};
