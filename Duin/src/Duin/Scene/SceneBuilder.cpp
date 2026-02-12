#include "dnpch.h"
#include "SceneBuilder.h"
#include "Duin/ECS/ECSManager.h"
#include "Duin/ECS/DECS/Entity.h"
#include "Duin/IO/JSONValue.h"
#include "Duin/IO/FileUtils.h"
#include <flecs.h>
#include "Duin/ECS/ComponentSerializer.h"

#define PRETTY_WRITE_JSON

#ifdef PRETTY_WRITE_JSON
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif

// Static TAG definitions
const std::string duin::PackedComponent::TAG_JSON = "json";

const std::string duin::PackedEntity::TAG_UUID = "uuid";
const std::string duin::PackedEntity::TAG_NAME = "name";
const std::string duin::PackedEntity::TAG_TAGS = "tags";
const std::string duin::PackedEntity::TAG_ENABLED = "enabled";
const std::string duin::PackedEntity::TAG_CHILDREN = "children";
const std::string duin::PackedEntity::TAG_COMPONENTS = "components";

const std::string duin::PackedExternalDependency::TAG_UUID = "uuid";
const std::string duin::PackedExternalDependency::TAG_TYPE = "type";

const std::string duin::PackedSceneMetadata::TAG_EDITORVERSION = "editorVersion";
const std::string duin::PackedSceneMetadata::TAG_ENGINEVERSION = "engineVersion";
const std::string duin::PackedSceneMetadata::TAG_LASTMODIFIED = "lastModified";
const std::string duin::PackedSceneMetadata::TAG_AUTHOR = "author";

const std::string duin::PackedScene::TAG_SCENEUUID = "sceneUUID";
const std::string duin::PackedScene::TAG_SCENENAME = "sceneName";
const std::string duin::PackedScene::TAG_METADATA = "metadata";
const std::string duin::PackedScene::TAG_EXTERNALDEPENDENCIES = "externalDependencies";
const std::string duin::PackedScene::TAG_ENTITIES = "entities";

// PackedComponent
duin::PackedComponent duin::PackedComponent::Deserialize(const JSONValue &json)
{
    PackedComponent pc;
    pc.componentTypeName = json.GetMember("type").GetString();
    pc.jsonData = json.Write();

    return pc;
}

duin::JSONValue duin::PackedComponent::Serialize(const PackedComponent &pComp)
{
    JSONValue json = JSONValue::Parse(pComp.jsonData);

    return json;
}

// PackedEntity
duin::PackedEntity duin::PackedEntity::Deserialize(const JSONValue &json)
{
    PackedEntity pe;

    if (json.HasMember(pe.TAG_UUID))
    {
        pe.uuid = UUID::FromStringHex(json.GetMember(pe.TAG_UUID).GetString());
    }

    if (json.HasMember(pe.TAG_NAME))
    {
        pe.name = json.GetMember(pe.TAG_NAME).GetString();
    }

    if (json.HasMember(pe.TAG_ENABLED))
    {
        pe.enabled = json.GetMember(pe.TAG_ENABLED).GetBool();
    }

    if (json.HasMember(pe.TAG_TAGS))
    {
        JSONValue tagsArray = json.GetMember(pe.TAG_TAGS);
        if (tagsArray.IsArray())
        {
            for (auto it = tagsArray.Begin(); it != tagsArray.End(); ++it)
            {
                pe.tags.push_back(it.GetValue().GetString());
            }
        }
    }

    if (json.HasMember(pe.TAG_CHILDREN))
    {
        JSONValue tagsArray = json.GetMember(pe.TAG_CHILDREN);
        if (tagsArray.IsArray())
        {
            for (auto it = tagsArray.Begin(); it != tagsArray.End(); ++it)
            {
                JSONValue eJSON(it.GetValue());
                PackedEntity e = PackedEntity::Deserialize(eJSON);
                pe.children.push_back(e);
            }
        }
    }

    if (json.HasMember(pe.TAG_COMPONENTS))
    {
        JSONValue componentsArray = json.GetMember(pe.TAG_COMPONENTS);
        if (componentsArray.IsArray())
        {
            for (auto it = componentsArray.Begin(); it != componentsArray.End(); ++it)
            {
                JSONValue cmpJSON(it.GetValue());
                PackedComponent cmp = PackedComponent::Deserialize(cmpJSON);
                pe.components.push_back(cmp);
            }
        }
    }
    else
    {
        DN_CORE_INFO("No components found!");
    }

    return pe;
}

duin::JSONValue duin::PackedEntity::Serialize(const PackedEntity &entity)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(entity.TAG_UUID, UUID::ToStringHex(entity.uuid));
    json.AddMember(entity.TAG_NAME, entity.name);
    json.AddMember(entity.TAG_ENABLED, entity.enabled);

    JSONValue tagsArray;
    tagsArray.SetArray();
    for (const auto &tag : entity.tags)
    {
        JSONValue t(tag);
        tagsArray.PushBack(t);
    }
    json.AddMember(entity.TAG_TAGS, tagsArray);

    JSONValue childrenArray;
    childrenArray.SetArray();
    for (const auto &child : entity.children)
    {
        JSONValue eJSON = PackedEntity::Serialize(child);
        childrenArray.PushBack(eJSON);
    }
    json.AddMember(entity.TAG_CHILDREN, childrenArray);

    JSONValue componentsArray;
    componentsArray.SetArray();
    for (const auto &component : entity.components)
    {
        JSONValue cmpJSON = PackedComponent::Serialize(component);
        componentsArray.PushBack(cmpJSON);
    }
    json.AddMember(entity.TAG_COMPONENTS, componentsArray);

    return json;
}

// PackedExternalDependency
duin::PackedExternalDependency duin::PackedExternalDependency::Deserialize(const JSONValue &json)
{
    PackedExternalDependency ped;

    if (json.HasMember(ped.TAG_UUID))
    {
        ped.uuid = UUID::FromStringHex(json.GetMember(ped.TAG_UUID).GetString());
    }

    if (json.HasMember(ped.TAG_TYPE))
    {
        ped.type = json.GetMember(ped.TAG_TYPE).GetString();
    }

    return ped;
}

duin::JSONValue duin::PackedExternalDependency::Serialize(const PackedExternalDependency &dependency)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(dependency.TAG_UUID, UUID::ToStringHex(dependency.uuid));
    json.AddMember(dependency.TAG_TYPE, dependency.type);

    return json;
}

// PackedSceneMetadata
duin::PackedSceneMetadata duin::PackedSceneMetadata::Deserialize(const JSONValue &json)
{
    PackedSceneMetadata psm;

    if (json.HasMember(psm.TAG_EDITORVERSION))
    {
        psm.editorVersion = json.GetMember(psm.TAG_EDITORVERSION).GetString();
    }

    if (json.HasMember(psm.TAG_ENGINEVERSION))
    {
        psm.engineVersion = json.GetMember(psm.TAG_ENGINEVERSION).GetString();
    }

    if (json.HasMember(psm.TAG_LASTMODIFIED))
    {
        psm.lastModified = json.GetMember(psm.TAG_LASTMODIFIED).GetString();
    }

    if (json.HasMember(psm.TAG_AUTHOR))
    {
        psm.author = json.GetMember(psm.TAG_AUTHOR).GetString();
    }

    return psm;
}

duin::JSONValue duin::PackedSceneMetadata::Serialize(const PackedSceneMetadata &metadata)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(metadata.TAG_EDITORVERSION, metadata.editorVersion);
    json.AddMember(metadata.TAG_ENGINEVERSION, metadata.engineVersion);
    json.AddMember(metadata.TAG_LASTMODIFIED, metadata.lastModified);
    json.AddMember(metadata.TAG_AUTHOR, metadata.author);

    return json;
}

// PackedScene
duin::PackedScene duin::PackedScene::Deserialize(const JSONValue &json)
{
    PackedScene ps;

    if (json.HasMember(ps.TAG_SCENEUUID))
    {
        ps.uuid = UUID::FromStringHex(json.GetMember(ps.TAG_SCENEUUID).GetString());
    }

    if (json.HasMember(ps.TAG_SCENENAME))
    {
        ps.name = json.GetMember(ps.TAG_SCENENAME).GetString();
    }

    if (json.HasMember(ps.TAG_METADATA))
    {
        PackedSceneMetadata metadataTemp = PackedSceneMetadata::Deserialize(json.GetMember(ps.TAG_METADATA));
        ps.metadata.editorVersion = metadataTemp.editorVersion;
        ps.metadata.engineVersion = metadataTemp.engineVersion;
        ps.metadata.lastModified = metadataTemp.lastModified;
        ps.metadata.author = metadataTemp.author;
    }

    if (json.HasMember(ps.TAG_EXTERNALDEPENDENCIES))
    {
        JSONValue depsArray = json.GetMember(ps.TAG_EXTERNALDEPENDENCIES);
        if (depsArray.IsArray())
        {
            for (auto it = depsArray.Begin(); it != depsArray.End(); ++it)
            {
                ps.externalDependencies.push_back(PackedExternalDependency::Deserialize(it.GetValue()));
            }
        }
    }

    if (json.HasMember(ps.TAG_ENTITIES))
    {
        JSONValue entitiesArray = json.GetMember(ps.TAG_ENTITIES);
        if (entitiesArray.IsArray())
        {
            for (auto it = entitiesArray.Begin(); it != entitiesArray.End(); ++it)
            {
                ps.entities.push_back(PackedEntity::Deserialize(it.GetValue()));
            }
        }
    }

    return ps;
}

duin::JSONValue duin::PackedScene::Serialize(const PackedScene &scene)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(scene.TAG_SCENEUUID, UUID::ToStringHex(scene.uuid));
    json.AddMember(scene.TAG_SCENENAME, scene.name);
    json.AddMember(scene.TAG_METADATA, PackedSceneMetadata::Serialize(scene.metadata));

    JSONValue depsArray;
    depsArray.SetArray();
    for (const auto &dep : scene.externalDependencies)
    {
        depsArray.PushBack(PackedExternalDependency::Serialize(dep));
    }
    json.AddMember(scene.TAG_EXTERNALDEPENDENCIES, depsArray);

    JSONValue entitiesArray;
    entitiesArray.SetArray();
    for (const auto &entity : scene.entities)
    {
        entitiesArray.PushBack(PackedEntity::Serialize(entity));
    }
    json.AddMember(scene.TAG_ENTITIES, entitiesArray);

    return json;
}

void duin::PackedScene::Instantiate(const Entity &e)
{
    for (PackedEntity &pEntity : entities)
    {
    }
}

duin::PackedScene duin::PackedScene::Pack(Entity &e)
{
    PackedScene packedScene;

    std::vector<std::string> sercomVec;
    e.ForEachComponent([&](duin::Entity &ent) {
        std::string sercom = duin::ComponentSerializer::Get().Serialize(ent.GetName(), e.Get(ent));
        sercomVec.push_back(sercom);
        DN_INFO("SERCOM: {}", sercom);
    });

    return packedScene;
}
