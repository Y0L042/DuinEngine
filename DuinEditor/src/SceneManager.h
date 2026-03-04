#pragma once

#include <Duin/Scene/SceneBuilder.h>

#include <unordered_map>
#include <Duin/IO/JSONValue.h>

#include "Scene.h"
#include <memory>
#include <vector>
#include <Duin/Core/Signals/Signal.h>

class SceneManager
{
  public:
    duin::Signal<std::shared_ptr<Scene>> onSetActiveScene;

    SceneManager();
    ~SceneManager();

    SceneHandle LoadSceneFromJSON(duin::JSONValue sceneJSON, bool setActive = false);
    SceneHandle LoadSceneFromPacked(const duin::PackedScene &scene, bool setActive = false);

    void UnloadScene(SceneHandle handle);

    std::shared_ptr<Scene> GetScene(SceneHandle handle);

    std::vector<std::shared_ptr<Scene>> GetLoadedScenes();
    std::shared_ptr<Scene> GetActiveScene();
    SceneHandle GetActiveSceneHandle();
    void SetActiveScene(SceneHandle handle);

  private:
    duin::PackedScene defaultScene;
    std::unordered_map<SceneHandle, std::shared_ptr<Scene>> loadedScenes;
    SceneHandle activeSceneHandle;
};