#include "SceneManager.h"
#include "EditorWorld.h"

SceneManager::SceneManager()
{
    auto handle = LoadSceneFromPacked(defaultScene);
    SetActiveScene(handle);
}

SceneManager::~SceneManager()
{
}

SceneHandle SceneManager::LoadSceneFromJSON(duin::JSONValue sceneJSON, bool setActive)
{
    duin::SceneBuilder builder;
    duin::PackedScene pscn = builder.DeserializeScene(sceneJSON);

    return LoadSceneFromPacked(pscn, setActive);
}

SceneHandle SceneManager::LoadSceneFromPacked(const duin::PackedScene &scene, bool setActive)
{
    auto scn = std::make_shared<Scene>();
    scn->packedScene = scene;
    scn->uuid = scn->packedScene.uuid;
    loadedScenes[scn->uuid] = scn;

    scn->ctx.editorWorld = std::make_shared<EditorWorld>();
    scn->ctx.editorWorld->Initialize();

    if (setActive)
    {
        SetActiveScene(scn->uuid);
    }

    return scn->uuid;
}

void SceneManager::UnloadScene(SceneHandle handle)
{
    if (loadedScenes.contains(handle))
    {
        loadedScenes.erase(handle);
    }
}

std::shared_ptr<Scene> SceneManager::GetScene(SceneHandle handle)
{
    return loadedScenes[handle];
}

std::vector<std::shared_ptr<Scene>> SceneManager::GetLoadedScenes()
{
    std::vector<std::shared_ptr<Scene>> v{loadedScenes.size()};
    std::for_each(loadedScenes.begin(), loadedScenes.end(),
                  [&](std::pair<SceneHandle, std::shared_ptr<Scene>> pair) { v.push_back(pair.second); });

    return v;
}

std::shared_ptr<Scene> SceneManager::GetActiveScene()
{
    return GetScene(activeSceneHandle);
}

SceneHandle SceneManager::GetActiveSceneHandle()
{
    return activeSceneHandle;
}

void SceneManager::SetActiveScene(SceneHandle handle)
{
    activeSceneHandle = handle;
    onSetActiveScene.Emit(GetScene(activeSceneHandle));
}