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

SceneHandle SceneManager::LoadSceneFromFSNode(std::weak_ptr<FSNode> node, bool setActive)
{
    if (node.expired())
        return duin::UUID::INVALID;

    duin::JSONValue v = duin::JSONValue::ParseFromFile(node.lock()->path);
    duin::SceneBuilder builder;
    duin::PackedScene pscn = builder.DeserializeScene(v);

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
    if (loadedScenes.find(handle) != loadedScenes.end())
    {
        return loadedScenes[handle];
    }
    return nullptr;
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

SceneHandle SceneManager::SetActiveScene(SceneHandle handle)
{
    SceneHandle res = duin::UUID::INVALID;

    bool found = loadedScenes.find(handle) != loadedScenes.end();
    if (found)
    {
        res = handle;
        activeSceneHandle = res;
    }

    return activeSceneHandle;
}

SceneHandle SceneManager::InstantiateScene(SceneHandle handle)
{
    auto scene = GetScene(handle);
    if (scene)
    {
        std::shared_ptr<duin::SceneBuilder> sceneBuilder =
            std::make_shared<duin::SceneBuilder>();
        sceneBuilder->InstantiateScene(scene->packedScene, scene->ctx.editorWorld.get());
    }
    return SceneHandle();
}
