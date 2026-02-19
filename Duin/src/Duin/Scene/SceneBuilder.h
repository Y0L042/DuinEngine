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
 * @brief Serialized component data. {"type":"TestStructX","x":"3"}
 * @ingroup ECS_Scene
 */
struct PackedComponent
{
    static const std::string TAG_JSON;

    std::string componentTypeName;
    std::string jsonData;

    static PackedComponent Deserialize(const JSONValue &Comp);
    static JSONValue Serialize(const PackedComponent &pComp);

    // ComponentSerializer::Serialize() -> PackedComponent::Pack()
    static PackedComponent Pack(Entity e, Entity cmp);
    static void Instantiate(const PackedComponent &p, Entity e);
};

/**
 * @struct PackedPair
 * @brief Serialized FLECS pair (relationship, target).
 * @ingroup ECS_Scene
 *
 * Represents a FLECS pair relationship such as IsA, ChildOf, or custom relationships.
 * Format: {"relationship":"RelationshipName","target":"TargetName","data":"..."}
 */
struct PackedPair
{
    static const std::string TAG_RELATIONSHIP;
    static const std::string TAG_TARGET;
    static const std::string TAG_DATA;

    std::string relationshipName; ///< Name of the relationship (first element)
    std::string targetName;       ///< Name or UUID of the target (second element)
    std::string jsonData;         ///< Optional data stored with the pair

    static PackedPair Deserialize(const JSONValue &pair);
    static JSONValue Serialize(const PackedPair &ppair);

    static PackedPair Pack(Entity e, Entity::ID pairId);
    static void Instantiate(const PackedPair &p, Entity e);
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
    static const std::string TAG_ENABLED;
    static const std::string TAG_CHILDREN;
    static const std::string TAG_COMPONENTS;
    static const std::string TAG_TAGS;
    static const std::string TAG_PAIRS;

    UUID uuid;                               ///< Entity unique identifier.
    std::string name;                        ///< Entity display name.
    bool enabled;                            ///< Whether entity is active.
    std::vector<PackedComponent> tags;       ///< Attached tags.
    std::vector<PackedPair> pairs;           ///< FLECS pairs (relationships).
    std::vector<PackedComponent> components; ///< Attached components.
    std::vector<PackedEntity> children;      ///< Child entities.

    static PackedEntity Deserialize(const JSONValue &entity);
    static JSONValue Serialize(const PackedEntity &pEntity);

    static PackedEntity Pack(Entity e);
    static void Instantiate(const PackedEntity &pe, Entity e);
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
    static void Instantiate(PackedScene &pscn, World *world);
    static PackedScene Pack(const std::vector<Entity> &vecEntities);

  private:
};

} // namespace duin
