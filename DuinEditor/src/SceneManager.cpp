#include "SceneManager.h"

SceneManager::SceneManager()
{
}

SceneManager::~SceneManager()
{
}

duin::PackedScene &SceneManager::LoadScene(duin::JSONValue sceneJSON)
{
    duin::PackedScene pscn = duin::PackedScene::Deserialize(sceneJSON);
    loadedScenes[pscn.uuid] = pscn;

    return loadedScenes[pscn.uuid];
}

void SceneManager::UnloadScene(duin::PackedScene &scene)
{
    if (loadedScenes.contains(scene.uuid))
    {
        loadedScenes.erase(scene.uuid);
    }
}

duin::PackedScene &SceneManager::GetActiveScene()
{
    if (activeScene != nullptr)
    {
        return *activeScene;
    }
    return defaultScene;
}

duin::PackedScene &SceneManager::SetActiveScene(duin::PackedScene &scene)
{
    activeScene = &scene;
    return *activeScene;
}
