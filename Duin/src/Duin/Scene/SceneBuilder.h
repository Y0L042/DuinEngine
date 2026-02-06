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
#include "Duin/ECS/ECSManager.h"
#include "Duin/ECS/DECS/World.h"

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
    static const std::string TAG_JSON;
    std::string json; ///< JSON representation of component data.

    static PackedComponent Deserialize(const JSONValue &Comp);
    static JSONValue Serialize(const PackedComponent &pComp);
};

/**
 * @struct PackedEntity
 * @brief Serialized entity with components and children.
 * @ingroup ECS_Scene
 *
 *
 * Represents a complete entity hierarchy for serialization.
 */
struct PackedEntity
{
    static const std::string TAG_UUID;
    static const std::string TAG_NAME;
    static const std::string TAG_TAGS;
    static const std::string TAG_ENABLED;
    static const std::string TAG_CHILDREN;
    static const std::string TAG_COMPONENTS;

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
    static const std::string TAG_UUID;
    static const std::string TAG_TYPE;

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
    static const std::string TAG_EDITORVERSION;
    static const std::string TAG_ENGINEVERSION;
    static const std::string TAG_LASTMODIFIED;
    static const std::string TAG_AUTHOR;

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
 * Contains all data needed to
 * save/load a scene: metadata, entities,
 * and external dependencies.
 */
struct PackedScene
{
  public:
    static const std::string TAG_SCENEUUID;
    static const std::string TAG_SCENENAME;
    static const std::string TAG_METADATA;
    static const std::string TAG_EXTERNALDEPENDENCIES;
    static const std::string TAG_ENTITIES;

    UUID uuid;                                                  ///< Scene identifier.
    std::string name;                                           ///< Scene name.
    PackedSceneMetadata metadata;                               ///< Version/author info.
    std::vector<PackedExternalDependency> externalDependencies; ///< External refs.
    std::vector<PackedEntity> entities;                         ///< Root entities.

    static PackedScene Deserialize(const JSONValue &scene);
    static JSONValue Serialize(const PackedScene &pScene);

    /** @brief Creates entities in the given world from this packed scene. */
    void Instantiate(const Entity &e);
    static PackedScene Pack(Entity &e);

  private:
};

} // namespace duin
