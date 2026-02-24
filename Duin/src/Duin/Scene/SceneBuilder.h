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
#include "Duin/ECS/GameWorld.h"
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
    static const std::string TAG_RELATIONSHIPNAME;
    static const std::string TAG_RELATIONSHIPUUID;
    static const std::string TAG_RELATIONSHIP_IS_COMPONENT;
    static const std::string TAG_RELATIONSHIPPATH;
    static const std::string TAG_TARGETNAME;
    static const std::string TAG_TARGETUUID;
    static const std::string TAG_TARGET_IS_COMPONENT;
    static const std::string TAG_TARGETPATH;
    static const std::string TAG_DATA;

    std::string relationshipName; ///< Short display name of the relationship (first element)
    UUID relationshipUUID;
    bool relationshipIsComponent = false; ///< True if relationship is a component/tag type, not a plain entity
    std::string relationshipPath;         ///< Full FLECS path (for component-type lookup across worlds)
    std::string targetName; ///< Short display name of the target (second element)
    UUID targetUUID;
    bool targetIsComponent = false; ///< True if target is a component/tag type, not a plain entity
    std::string targetPath;         ///< Full FLECS path (for component-type lookup across worlds)
    std::string jsonData; ///< Optional data stored with the pair
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
};

class SceneBuilder
{
  public:
    SceneBuilder(duin::World *world);
    ~SceneBuilder() = default;

    PackedScene PackScene(const std::vector<Entity> &vecEntities);
    PackedScene PackScene(std::shared_ptr<World> world);
    PackedScene PackScene(World *world);
    void InstantiateScene(PackedScene &pscn, World *world);
    void InstantiateSceneAsChildren(PackedScene &pscn, Entity parent);
    JSONValue SerializeScene(const PackedScene &pscn);
    PackedScene DeserializeScene(const JSONValue &scene);

    JSONValue SerializeMetadata(const PackedSceneMetadata &metadata);
    PackedSceneMetadata DeserializeMetadata(const JSONValue &metadata);

    PackedEntity PackEntity(Entity e);
    void InstantiateEntity(const PackedEntity &pe, Entity e);
    JSONValue SerializeEntity(const PackedEntity &pe);
    PackedEntity DeserializeEntity(const JSONValue &entity);

    PackedComponent PackComponent(Entity e, Entity cmp);
    void InstantiateComponent(const PackedComponent &pc, Entity e);
    JSONValue SerializeComponent(const PackedComponent &pc);
    PackedComponent DeserializeComponent(const JSONValue &comp);

    PackedPair PackPair(Entity e, Entity::ID pairId);
    PackedPair PackPairWRelationshipEntity(Entity::ID pairID, UUID rUUID);
    PackedPair PackPairWTargetEntity(Entity::ID pairID, UUID tUUID);
    PackedPair PackPairWBothEntities(Entity::ID pairID, UUID rUUID, UUID tUUID);
    void InstantiatePair(const PackedPair &pp, Entity e);
    JSONValue SerializePair(const PackedPair &pp);
    PackedPair DeserializePair(const JSONValue &pair);

    JSONValue SerializeExternalDependency(const PackedExternalDependency &ped);
    PackedExternalDependency DeserializeExternalDependency(const JSONValue &exdep);

  private:
    duin::World *world = nullptr;
    std::unordered_map<uint64_t, UUID> instanceToPackedEntityMap;
    std::unordered_map<UUID, uint64_t> packedEntityToInstanceMap;

    void PrePassEntity(Entity e);
    void PrePassInstantiate(const PackedEntity &pe, Entity parent);
};

} // namespace duin
