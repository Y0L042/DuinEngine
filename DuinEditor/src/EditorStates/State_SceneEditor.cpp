#include "State_SceneEditor.h"
#include "Singleton.h"
#include <Duin/Core/Debug/DNLog.h>

void State_SceneEditor::Enter()
{
    CreateGUIElements();
    ProcessProject(Singleton::GetActiveProject());

    gameWorld = std::make_unique<duin::GameWorld>();
    gameWorld->Initialize(false);
    sceneBuilder = std::make_unique<duin::SceneBuilder>(gameWorld.get());
}

void State_SceneEditor::OnEvent(duin::Event e)
{
}

void State_SceneEditor::Update(double delta)
{
}

void State_SceneEditor::PhysicsUpdate(double delta)
{
    //queuedNewScene();

    sceneTree->UpdateTree(gameWorld.get());
}

void State_SceneEditor::Draw()
{
}

void State_SceneEditor::DrawUI()
{
}

void State_SceneEditor::Exit()
{
}

void State_SceneEditor::ProcessProject(Project project)
{
    // Initialize FileManager
    fileManager.SetRootPath(project.GetPathAsString());
    fileManager.BuildFileSystemTree();
    onUpdateFileManager.Emit(&fileManager);
}

void State_SceneEditor::CreateGUIElements()
{
    sceneTabs = CreateChildObject<SceneTabs>();
    sceneTree = CreateChildObject<SceneTree>();
    sceneViewport = CreateChildObject<SceneViewport>();
    fileTree = CreateChildObject<FileTree>(this);
    entityProperties = CreateChildObject<EntityProperties>();

    fileTree->onSceneSelect.Connect([&](FSNode *sceneFile) {
        duin::PackedScene &pscn = LoadSceneFromFile(sceneFile);
        SetActiveScene(pscn);
        DN_INFO("Scene Selected! {}", sceneFile->name);
    });
}

duin::PackedScene &State_SceneEditor::LoadSceneFromFile(const FSNode *sceneFile)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sceneFile->path);
    return sceneManager.LoadScene(v);
}

void State_SceneEditor::SetActiveScene(duin::PackedScene &scene)
{
    sceneManager.SetActiveScene(scene);
    gameWorld->Clear();
    sceneBuilder->InstantiateScene(scene, gameWorld.get());
    onSetActiveScene.Emit(scene);

    //queuedNewScene = [&]() {
    //    sceneBuilder->InstantiateScene(scene, gameWorld.get());
    //    onSetActiveScene.Emit(scene);
    //};
}
