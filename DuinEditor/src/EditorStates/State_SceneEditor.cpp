#include "State_SceneEditor.h"
#include "Singleton.h"
#include <Duin/Core/Debug/DNLog.h>

void State_SceneEditor::Enter()
{
    InitializeManagers();
    CreateUIObjects();

    ProcessProject(Singleton::GetActiveProject());
}

void State_SceneEditor::OnEvent(duin::Event e)
{
}

void State_SceneEditor::Update(double delta)
{
}

void State_SceneEditor::PhysicsUpdate(double delta)
{
}

void State_SceneEditor::Draw()
{
}

void State_SceneEditor::DrawUI()
{
    uiObjects.sceneViewport->StartTextureRender();

    uiObjects.sceneViewport->EndTextureRender();
}

void State_SceneEditor::Exit()
{
}

void State_SceneEditor::InitializeManagers()
{
    fileManager = std::make_shared<FileManager>();
    sceneManager = std::make_shared<SceneManager>();
    auto scene = sceneManager->GetActiveScene();
    sceneBuilder = std::make_shared<duin::SceneBuilder>();
}

void State_SceneEditor::ProcessProject(Project project)
{
    fileManager->SetRootPath(project.GetPathAsString());
    fileManager->BuildFileSystemTree();
    signals.onUpdateFileManager.Emit(fileManager);
}

void State_SceneEditor::CreateUIObjects()
{
    uiObjects.sceneTabs = CreateChildObject<SceneTabs>();
    uiObjects.sceneTree = CreateChildObject<SceneTree>();
    uiObjects.sceneViewport = CreateChildObject<SceneViewport>();
    uiObjects.fileTree = CreateChildObject<FileTree>();
    uiObjects.entityProperties = CreateChildObject<EntityProperties>();

    ConnectOnActiveTabChanged();
    ConnectOnSceneSelect();
    ConnectOnSetActiveScene();
    ConnectOnSelectEntity();
    ConnectOnUpdateFileManager();
}

void State_SceneEditor::LoadSceneFromFile(const FSNode *sceneFile)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sceneFile->path);
    sceneManager->LoadSceneFromJSON(v);
}

void State_SceneEditor::OpenSceneInTab(std::weak_ptr<FSNode> sceneFile)
{
    if (sceneFile.expired())
    {
        DN_WARN("SceneFile is expired!");
        return;
    }
    auto sf = sceneFile.lock();
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sf->path);
    auto handle = sceneManager->LoadSceneFromJSON(v, true);
    auto scene = sceneManager->GetScene(handle);
    sceneBuilder->InstantiateScene(scene->packedScene, scene->ctx.editorWorld.get());
}

void State_SceneEditor::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    cachedActiveScene = scene;
}

void State_SceneEditor::ConnectOnActiveTabChanged()
{
    uiObjects.sceneTabs->onActiveTabChanged.Connect([&](std::weak_ptr<Scene> scene) {
        if (!scene.expired())
            sceneManager->SetActiveScene(scene.lock()->uuid);
    });
}

void State_SceneEditor::ConnectOnSceneSelect()
{
    uiObjects.fileTree->onSceneSelect.Connect([&](std::weak_ptr<FSNode> sceneFile) {
        if (sceneFile.expired())
        {
            DN_WARN("Scenefile expired!");
            return;
        }
        auto sf = sceneFile.lock();
        DN_INFO("Scenefile selected: {}", sf->name);
        OpenSceneInTab(sf);
        DN_INFO("Scene opened in tab: {}", sf->name);
    });
}

void State_SceneEditor::ConnectOnSetActiveScene()
{
    std::shared_ptr<duin::ScopedConnection> conn = nullptr;

    onSetActiveSceneHandle =
        sceneManager->onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) { CacheActiveScene(scene); });

    conn = sceneManager->onSetActiveScene.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneTree->CacheActiveScene(scene); });
    uiObjects.sceneTree->AddConnectionHandle(conn);

    conn = sceneManager->onSetActiveScene.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneViewport->CacheActiveScene(scene); });
    uiObjects.sceneViewport->AddConnectionHandle(conn);
}

void State_SceneEditor::ConnectOnSelectEntity()
{
    std::shared_ptr<duin::ScopedConnection> conn = nullptr;

    conn = uiObjects.sceneTree->onSelectEntity.ConnectScoped(
        [&](duin::Entity e) { uiObjects.entityProperties->SetFocusedEntity(e); });
    uiObjects.entityProperties->AddConnectionHandle(conn);
}

void State_SceneEditor::ConnectOnUpdateFileManager()
{
    std::shared_ptr<duin::ScopedConnection> conn = nullptr;

    conn = signals.onUpdateFileManager.ConnectScoped(
        [&](std::shared_ptr<FileManager> fm) { uiObjects.fileTree->SetFileManager(fm); });
    uiObjects.fileTree->AddConnectionHandle(conn);
}
