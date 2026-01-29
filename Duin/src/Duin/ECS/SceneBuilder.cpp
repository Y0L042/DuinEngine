#include "dnpch.h"
#include "SceneBuilder.h"
#include "ECSManager.h"
#include "DECS/Entity.h"
#include "Duin/IO/JSONValue.h"
#include "Duin/IO/FileUtils.h"
#include <flecs.h>

#define PRETTY_WRITE_JSON

#ifdef PRETTY_WRITE_JSON
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif

// PackedComponent
duin::PackedComponent duin::PackedComponent::Deserialize(const JSONValue &json)
{
    PackedComponent pc;

    if (json.HasMember(pc.TAG_JSON))
    {
        pc.json = UUID::FromStringHex(json.GetMember(pc.TAG_JSON).GetString());
    }

    return pc;
}

duin::JSONValue duin::PackedComponent::Serialize(const PackedComponent &pComp)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(pComp.TAG_JSON, json);

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
                // pe.children.emplace_back(it.GetValue().GetInt());
            }
        }
    }

    if (json.HasMember(pe.TAG_COMPONENTS))
    {
        JSONValue componentsArray = json.GetMember(pe.TAG_COMPONENTS);
        if (componentsArray.IsArray())
        {
            // TODO: Deserialize components when PackedComponent is implemented
        }
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

    JSONValue componentsArray;
    componentsArray.SetArray();
    for (const auto &component : entity.components)
    {
        // TODO: Serialize components when PackedComponent is implemented
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

void duin::PackedScene::Instantiate(World &world)
{
    for (PackedEntity &pEntity : entities)
    {
        Entity e = world.CreateEntity(pEntity.name);
    }
}

static void UnpackEntities(duin::World &world, std::vector<duin::PackedEntity> &entities, duin::Entity parent)
{
    for (duin::PackedEntity &pEntity : entities)
    {
        duin::Entity e = world.CreateEntity(pEntity.name);
        if (parent.IsValid())
            e.ChildOf(parent);

        UnpackEntities(world, pEntity.children, e);
    }
}
