#include "State_SceneEditor.h"
#include "Singleton.h"
#include <Duin/Core/Debug/DNLog.h>

void State_SceneEditor::Enter()
{
    CreateEditorContext();
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

void State_SceneEditor::CreateEditorContext()
{
    editorContext.fileManager = std::make_shared<FileManager>();
    editorContext.sceneManager = std::make_shared<SceneManager>();
    editorContext.editorWorld = std::make_shared<EditorWorld>();
    editorContext.sceneBuilder = std::make_shared<duin::SceneBuilder>(editorContext.editorWorld.get());
}

void State_SceneEditor::ProcessProject(Project project)
{
    editorContext.fileManager->SetRootPath(project.GetPathAsString());
    editorContext.fileManager->BuildFileSystemTree();
    signals.onUpdateFileManager.Emit(editorContext.fileManager);
}

void State_SceneEditor::CreateUIObjects()
{
    uiObjects.sceneTabs = CreateChildObject<SceneTabs>();
    uiObjects.sceneTree = CreateChildObject<SceneTree>();
    uiObjects.sceneViewport = CreateChildObject<SceneViewport>();
    uiObjects.fileTree = CreateChildObject<FileTree>();
    uiObjects.entityProperties = CreateChildObject<EntityProperties>();

    uiObjects.fileTree->onSceneSelect.Connect([&](std::weak_ptr<FSNode> sceneFile) {
        if (sceneFile.expired())
        {
            DN_WARN("Scenefile expired!");
            return;
        }
        DN_INFO("Scenefile selected: {}", sceneFile.lock()->name);
        duin::PackedScene &pscn = LoadSceneFromFile(sceneFile.lock().get());
        SetActiveScene(pscn);
        DN_INFO("Scene Selected! {}", sceneFile.lock()->name);
    });

    uiObjects.sceneTree->onSelectEntity
        .Connect([&](duin::Entity e) { uiObjects.entityProperties->SetFocusedEntity(e); });

    signals.onUpdateFileManager.Connect(
        [&](std::shared_ptr<FileManager> fm) { uiObjects.fileTree->SetFileManager(fm); });

    signals.onWorldChange.Connect([&](std::weak_ptr<duin::GameWorld> world) {
        DN_INFO("World changed!");
        uiObjects.sceneTree->SetActiveWorld(world);
    });

    signals.onWorldChange.Connect(
        [&](std::weak_ptr<duin::GameWorld> world) { uiObjects.sceneViewport->SetActiveWorld(world); });
}

duin::PackedScene &State_SceneEditor::LoadSceneFromFile(const FSNode *sceneFile)
{
    duin::JSONValue v = duin::JSONValue::ParseFromFile(sceneFile->path);
    return editorContext.sceneManager->LoadScene(v);
}

void State_SceneEditor::SetActiveScene(duin::PackedScene &scene)
{
    editorContext.sceneManager->SetActiveScene(scene);
    editorContext.editorWorld->Reset(false);
    editorContext.sceneBuilder->InstantiateScene(scene, editorContext.editorWorld.get());

    signals.onSetActiveScene.Emit(scene);
    signals.onWorldChange.Emit(editorContext.editorWorld);
}
