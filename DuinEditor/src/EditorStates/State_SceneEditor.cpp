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
}

void State_SceneEditor::Exit()
{
}

void State_SceneEditor::InitializeManagers()
{
    fileManager = std::make_shared<FileManager>();
    sceneManager = std::make_shared<SceneManager>();
    auto scene = sceneManager->GetActiveScene();
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

    ConnectSignals();
}

void State_SceneEditor::ConnectSignals()
{
    std::shared_ptr<duin::ScopedConnection> conn = nullptr;

    // --- onSetActiveScene ---
    // Scoped
    // Local activeScene handle
    onSetActiveSceneHandle =
        signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) { CacheActiveScene(scene); });

    // set sceneViewport activeScene
    conn = signals.onSetActiveScene.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneViewport->CacheActiveScene(scene); });
    uiObjects.sceneViewport->AddConnectionHandle(conn);

    // Set entityProperties activeScene
    conn = signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) {
        uiObjects.entityProperties->CacheActiveScene(scene);
        uiObjects.entityProperties->SetFocusedEntity(duin::Entity()); // clear
    });
    uiObjects.entityProperties->AddConnectionHandle(conn);

    // Set sceneTree activeScene
    conn = signals.onSetActiveScene.ConnectScoped([&](std::shared_ptr<Scene> scene) {
        DN_INFO("onSetActiveScene - sceneTree cache active scene");
        uiObjects.sceneTree->CacheActiveScene(scene);
    });
    uiObjects.sceneTree->AddConnectionHandle(conn);

    conn = signals.onSetSceneFromFile.ConnectScoped(
        [&](std::shared_ptr<Scene> scene) { uiObjects.sceneTabs->AddTab(scene); });
    uiObjects.sceneTabs->AddConnectionHandle(conn);

    // --- SceneTabs ---
    // Unscoped
    // Set sceneManager activeScene from sceneTabs
    uiObjects.sceneTabs->onSceneTabSelect.Connect([&](std::weak_ptr<Scene> scene) {
        if (!scene.expired())
        {
            auto handle = sceneManager->SetActiveScene(scene.lock()->uuid);

            if (handle)
            {
                signals.onSetActiveScene.Emit(sceneManager->GetScene(handle));
            }
        }
    });

    // --- FileTree ---
    // Scoped
    // Open selected packedScene in /
    conn = uiObjects.fileTree->onPackedSceneFileSelect.ConnectScoped([&](std::weak_ptr<FSNode> sceneFile) {
        if (sceneFile.expired())
        {
            DN_WARN("onPackedSceneFileSelect - Scenefile expired!");
            return;
        }
        auto sf = sceneFile.lock();
        DN_INFO("onPackedSceneFileSelect - Scenefile selected: {}", sf->name);
        EnsureInstantiatedScene(sf);
        DN_INFO("onPackedSceneFileSelect - Scene opened in tab: {}", sf->name);
    });
    uiObjects.sceneTabs->AddConnectionHandle(conn);

    // --- SceneTree ---
    // Scoped
    conn = uiObjects.sceneTree->onSelectEntity.ConnectScoped(
        [&](duin::Entity e) { uiObjects.entityProperties->SetFocusedEntity(e); });
    uiObjects.entityProperties->AddConnectionHandle(conn);

    // --- onUpdateFileManager ---
    // Scoped
    conn = signals.onUpdateFileManager.ConnectScoped(
        [&](std::shared_ptr<FileManager> fm) { uiObjects.fileTree->SetFileManager(fm); });
    uiObjects.fileTree->AddConnectionHandle(conn);
}

void State_SceneEditor::LoadSceneFromFile(const FSNode *sceneFile)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sceneFile->path);
    sceneManager->LoadSceneFromJSON(v);
}

/* 
* If scene already loaded, set as active.
* Else load scene, then set as active.
 */
void State_SceneEditor::EnsureInstantiatedScene(std::weak_ptr<FSNode> sceneFile)
{
    if (sceneFile.expired())
    {
        DN_WARN("SceneFile is expired!");
        return;
    }
    auto sf = sceneFile.lock();
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sf->path);
    auto uuid = duin::UUID::FromStringHex(v[duin::PackedScene::TAG_SCENEUUID].GetString());
    if (sceneManager->GetScene(uuid))
    {
        sceneManager->SetActiveScene(uuid);
        signals.onSetActiveScene.Emit(sceneManager->GetActiveScene());
        signals.onSetSceneFromFile.Emit(sceneManager->GetActiveScene());
    }
    else
    {
        auto handle = sceneManager->LoadSceneFromJSON(v, true);
        if (sceneManager->InstantiateScene(handle))
        {
            sceneManager->SetActiveScene(handle);
            //signals.onSetActiveScene.Emit(sceneManager->GetActiveScene());
            signals.onSetSceneFromFile.Emit(sceneManager->GetActiveScene());
        }
        else
        {
            DN_WARN("Scene {} failed to instantiate!", handle.ToStrHex());
        }
    }
}

void State_SceneEditor::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    cachedActiveScene = scene;
}
