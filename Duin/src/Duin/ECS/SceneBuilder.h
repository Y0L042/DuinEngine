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
struct PackedComponent
{
    const std::string TAG_JSON = "json";

    std::string json;

    static PackedComponent Deserialize(const JSONValue &Comp);
    static JSONValue Serialize(const PackedComponent &pComp);
};

struct PackedEntity
{
    const std::string TAG_UUID = "uuid";
    const std::string TAG_NAME = "name";
    const std::string TAG_TAGS = "tags";
    const std::string TAG_ENABLED = "enabled";
    const std::string TAG_CHILDREN = "children";
    const std::string TAG_COMPONENTS = "components";

    UUID uuid;
    std::string name;
    std::vector<std::string> tags;
    bool enabled;
    std::vector<PackedComponent> components;
    std::vector<PackedEntity> children;

    static PackedEntity Deserialize(const JSONValue &entity);
    static JSONValue Serialize(const PackedEntity &pEntity);
};

struct PackedExternalDependency
{
    const std::string TAG_UUID = "uuid";
    const std::string TAG_TYPE = "type";

    UUID uuid;
    std::string type;

    static PackedExternalDependency Deserialize(const JSONValue &exdep);
    static JSONValue Serialize(const PackedExternalDependency &pExdep);
};

struct PackedSceneMetadata
{
    const std::string TAG_EDITORVERSION = "editorVersion";
    const std::string TAG_ENGINEVERSION = "engineVersion";
    const std::string TAG_LASTMODIFIED = "lastModified";
    const std::string TAG_AUTHOR = "author";

    std::string editorVersion;
    std::string engineVersion;
    std::string lastModified;
    std::string author;

    static PackedSceneMetadata Deserialize(const JSONValue &meta);
    static JSONValue Serialize(const PackedSceneMetadata &pMeta);
};

struct PackedScene
{
    const std::string TAG_SCENEUUID = "sceneUUID";
    const std::string TAG_SCENENAME = "sceneName";
    const std::string TAG_METADATA = "metadata";
    const std::string TAG_EXTERNALDEPENDENCIES = "externalDependencies";
    const std::string TAG_ENTITIES = "entities";

    UUID uuid;
    std::string name;
    PackedSceneMetadata metadata;
    std::vector<PackedExternalDependency> externalDependencies;
    std::vector<PackedEntity> entities;

    static PackedScene Deserialize(const JSONValue &scene);
    static JSONValue Serialize(const PackedScene &pScene);

    void Instantiate(World& world);
};

} // namespace duin