#pragma once

#include <Duin/Scene/SceneBuilder.h>

#include <unordered_map>
#include <Duin/IO/JSONValue.h>

#include "Scene.h"
#include <memory>
#include <vector>
#include <Duin/Core/Signals/Signal.h>
#include "FileManager.h"

/**
 * @brief Manages loading, unloading, and instantiation of scenes in the editor.
 */
class SceneManager
{
  public:
    SceneManager();
    ~SceneManager();

    /**
     * @brief Load a scene from a JSON value.
     * @param sceneJSON The JSON representation of the scene.
     * @param setActive If true, set the loaded scene as the active scene.
     * @return Handle to the loaded scene.
     */
    SceneHandle LoadSceneFromJSON(duin::JSONValue sceneJSON, bool setActive = false);

    /**
     * @brief Load a scene from a filesystem node.
     * @param node Weak pointer to the filesystem node containing the scene file.
     * @param setActive If true, set the loaded scene as the active scene.
     * @return Handle to the loaded scene, or UUID::INVALID if the node is expired.
     */
    SceneHandle LoadSceneFromFSNode(std::weak_ptr<FSNode> node, bool setActive = false);

    /**
     * @brief Load a scene from a packed scene object.
     * @param scene The packed scene to load.
     * @param setActive If true, set the loaded scene as the active scene.
     * @return Handle to the loaded scene.
     */
    SceneHandle LoadSceneFromPacked(const duin::PackedScene &scene, bool setActive = false);

    /**
     * @brief Unload a previously loaded scene.
     * @param handle Handle of the scene to unload.
     */
    void UnloadScene(SceneHandle handle);

    /**
     * @brief Get a loaded scene by handle.
     * @param handle The scene handle.
     * @return Shared pointer to the scene, or nullptr if not found.
     */
    std::shared_ptr<Scene> GetScene(SceneHandle handle);

    /**
     * @brief Get all currently loaded scenes.
     * @return Vector of shared pointers to loaded scenes.
     */
    std::vector<std::shared_ptr<Scene>> GetLoadedScenes();

    /**
     * @brief Get the currently active scene.
     * @return Shared pointer to the active scene, or nullptr if none is active.
     */
    std::shared_ptr<Scene> GetActiveScene();

    /**
     * @brief Get the handle of the currently active scene.
     * @return The active scene handle.
     */
    SceneHandle GetActiveSceneHandle();

    /**
     * @brief Set the active scene by handle.
     * @param handle The scene handle to activate.
     * @return The active scene handle, or the previous active handle if not found.
     */
    SceneHandle SetActiveScene(SceneHandle handle);

    /**
     * @brief Instantiate a loaded scene into the editor world.
     * @param handle The scene handle to instantiate.
     * @return The scene handle.
     */
    SceneHandle InstantiateScene(SceneHandle handle);

    /**
     * @brief Ensure all external dependencies for a scene are available.
     * @param handle The scene handle to check.
     * @return True if all dependencies are satisfied.
     */
    bool EnsureExternalDependencies(SceneHandle handle);

  private:
    duin::PackedScene defaultScene;
    std::unordered_map<SceneHandle, std::shared_ptr<Scene>> loadedScenes;
    SceneHandle activeSceneHandle;

    /**
     * @brief Remap external dependency paths in a packed scene.
     * @param scene The packed scene to remap.
     */
    void RemapExternalDependencies(duin::PackedScene &scene);

    /**
     * @brief Remap external dependency paths in a packed entity and its children.
     * @param entity The packed entity to remap.
     */
    void RemapEntityDependencies(duin::PackedEntity &entity);
};