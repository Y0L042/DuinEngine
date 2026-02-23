#pragma once

#include <Duin/Core/Utils/UUID.h>
#include <Duin/Scene/SceneBuilder.h>

#include <unordered_map>
#include <Duin/IO/JSONValue.h>

class SceneManager
{
  public:
    SceneManager();
    ~SceneManager();

    duin::PackedScene &LoadScene(duin::JSONValue sceneJSON);
    void UnloadScene(duin::PackedScene &scene);

    duin::PackedScene &GetActiveScene();
    duin::PackedScene &SetActiveScene(duin::PackedScene &scene);

  private:
    duin::PackedScene defaultScene;
    duin::PackedScene *activeScene;
    std::unordered_map<duin::UUID, duin::PackedScene> loadedScenes;
};