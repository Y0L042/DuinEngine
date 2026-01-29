/**
 * @file SceneBuilder.h
 * @brief Scene serialization and deserialization structures.
 * @ingroup ECS_Scene
 *
 * Provides structures for packing/unpacking scenes to/from JSON.
 * Supports hierarchical entity structures with components, tags,
 * and external dependencies.
 */

#pragma once

#include "Duin/IO/JSONValue.h"
#include "ECSManager.h"
#include "DECS/World.h"

#include <rfl.hpp>

#include <flecs.h>
#include <string>
#include <vector>

namespace duin
{

/**
 * @struct PackedComponent
 * @brief Serialized component data.
 * @ingroup ECS_Scene
 */
struct PackedComponent
{
    const std::string TAG_JSON = "json";
    std::string json; ///< JSON representation of component data.

    static PackedComponent Deserialize(const JSONValue &Comp);
    static JSONValue Serialize(const PackedComponent &pComp);
};

/**
 * @struct PackedEntity
 * @brief Serialized entity with components and children.
 * @ingroup ECS_Scene
 *
 * Represents a complete entity hierarchy for serialization.
 */
struct PackedEntity
{
    const std::string TAG_UUID = "uuid";
    const std::string TAG_NAME = "name";
    const std::string TAG_TAGS = "tags";
    const std::string TAG_ENABLED = "enabled";
    const std::string TAG_CHILDREN = "children";
    const std::string TAG_COMPONENTS = "components";

    UUID uuid;                               ///< Entity unique identifier.
    std::string name;                        ///< Entity display name.
    std::vector<std::string> tags;           ///< Tag names applied to entity.
    bool enabled;                            ///< Whether entity is active.
    std::vector<PackedComponent> components; ///< Attached components.
    std::vector<PackedEntity> children;      ///< Child entities.

    static PackedEntity Deserialize(const JSONValue &entity);
    static JSONValue Serialize(const PackedEntity &pEntity);
};

/**
 * @struct PackedExternalDependency
 * @brief Reference to external scene or asset.
 * @ingroup ECS_Scene
 */
struct PackedExternalDependency
{
    const std::string TAG_UUID = "uuid";
    const std::string TAG_TYPE = "type";

    UUID uuid;        ///< Dependency identifier.
    std::string type; ///< Dependency type (scene, asset).

    static PackedExternalDependency Deserialize(const JSONValue &exdep);
    static JSONValue Serialize(const PackedExternalDependency &pExdep);
};

/**
 * @struct PackedSceneMetadata
 * @brief Scene metadata for versioning and tracking.
 * @ingroup ECS_Scene
 */
struct PackedSceneMetadata
{
    const std::string TAG_EDITORVERSION = "editorVersion";
    const std::string TAG_ENGINEVERSION = "engineVersion";
    const std::string TAG_LASTMODIFIED = "lastModified";
    const std::string TAG_AUTHOR = "author";

    std::string editorVersion; ///< Editor version that saved the scene.
    std::string engineVersion; ///< Engine version.
    std::string lastModified;  ///< Last modification timestamp.
    std::string author;        ///< Author name.

    static PackedSceneMetadata Deserialize(const JSONValue &meta);
    static JSONValue Serialize(const PackedSceneMetadata &pMeta);
};

/**
 * @struct PackedScene
 * @brief Complete serialized scene.
 * @ingroup ECS_Scene
 *
 * Contains all data needed to save/load a scene: metadata, entities,
 * and external dependencies.
 */
struct PackedScene
{
    const std::string TAG_SCENEUUID = "sceneUUID";
    const std::string TAG_SCENENAME = "sceneName";
    const std::string TAG_METADATA = "metadata";
    const std::string TAG_EXTERNALDEPENDENCIES = "externalDependencies";
    const std::string TAG_ENTITIES = "entities";

    UUID uuid;                                                  ///< Scene identifier.
    std::string name;                                           ///< Scene name.
    PackedSceneMetadata metadata;                               ///< Version/author info.
    std::vector<PackedExternalDependency> externalDependencies; ///< External refs.
    std::vector<PackedEntity> entities;                         ///< Root entities.

    static PackedScene Deserialize(const JSONValue &scene);
    static JSONValue Serialize(const PackedScene &pScene);

    /** @brief Creates entities in the given world from this packed scene. */
    void Instantiate(World &world);
};

} // namespace duin
