#include "dnpch.h"
#include "SceneBuilder.h"
#include "Duin/ECS/GameWorld.h"
#include "Duin/ECS/DECS/Entity.h"
#include "Duin/IO/JSONValue.h"
#include "Duin/IO/FileUtils.h"
#include <flecs.h>
#include "Duin/ECS/ComponentSerializer.h"
#include "Duin/ECS/PrefabRegistry.h"

#include <functional>
#include <unordered_map>

#define PRETTY_WRITE_JSON

#ifdef PRETTY_WRITE_JSON
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif
#include <cstdint>
#include <memory>
#include <string>
#include <vector>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/ECS/DECS/World.h>
#include <flecs/addons/cpp/c_types.hpp>
#include <flecs/addons/cpp/entity.hpp>

// Static TAG definitions
const std::string duin::PackedComponent::TAG_JSON = "json";

const std::string duin::PackedPair::TAG_RELATIONSHIPNAME = "relationship";
const std::string duin::PackedPair::TAG_RELATIONSHIPUUID = "relationshipUUID";
const std::string duin::PackedPair::TAG_RELATIONSHIP_IS_COMPONENT = "relationshipIsComponent";
const std::string duin::PackedPair::TAG_RELATIONSHIPPATH = "relationshipPath";
const std::string duin::PackedPair::TAG_TARGETNAME = "target";
const std::string duin::PackedPair::TAG_TARGETUUID = "targetUUID";
const std::string duin::PackedPair::TAG_TARGET_IS_COMPONENT = "targetIsComponent";
const std::string duin::PackedPair::TAG_TARGETPATH = "targetPath";
const std::string duin::PackedPair::TAG_DATA = "data";

const std::string duin::PackedEntity::TAG_UUID = "uuid";
const std::string duin::PackedEntity::TAG_NAME = "name";
const std::string duin::PackedEntity::TAG_ENABLED = "enabled";
const std::string duin::PackedEntity::TAG_CHILDREN = "children";
const std::string duin::PackedEntity::TAG_COMPONENTS = "components";
const std::string duin::PackedEntity::TAG_TAGS = "tags";
const std::string duin::PackedEntity::TAG_PAIRS = "pairs";

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

// ============================================================
// SceneBuilder
// ============================================================

duin::SceneBuilder::SceneBuilder(duin::World *world) : world(world)
{
}

// PackComponent
duin::PackedComponent duin::SceneBuilder::PackComponent(Entity e, Entity cmp)
{
    std::string cmpstr;
    if (cmp.IsTag())
    {
        cmpstr = duin::ComponentSerializer::Get().Serialize(cmp.GetName(), nullptr);
    }
    else if (cmp.IsPair())
    {
        Entity first = cmp.First();
        Entity second = cmp.Second();
        cmpstr = duin::ComponentSerializer::Get().Serialize(first.GetName(), second.Get(cmp));
    }
    else
    {
        if (!duin::ComponentSerializer::Get().IsRegistered(cmp.GetName()))
        {
            return PackedComponent();
        }
        cmpstr = duin::ComponentSerializer::Get().Serialize(cmp.GetName(), e.Get(cmp));
    }

    if (cmpstr.empty() || cmpstr == "" || cmpstr == "{}")
    {
        DN_CORE_WARN("SceneBuilder::PackComponent - Failed to serialize component: {}", cmp.GetName());
        return PackedComponent();
    }

    JSONValue v = JSONValue::Parse(cmpstr);

    if (!v.HasMember("type"))
    {
        DN_CORE_WARN("SceneBuilder::PackComponent - Serialized component missing 'type' field: {}", cmp.GetName());
        return PackedComponent();
    }

    PackedComponent pcmp;
    pcmp.componentTypeName = v["type"].GetString();
    pcmp.jsonData = v.Write();

    return pcmp;
}

void duin::SceneBuilder::InstantiateComponent(const PackedComponent &pc, Entity e)
{
    void *data = nullptr;
    ComponentSerializer::Get().Deserialize(e, pc.componentTypeName, data, pc.jsonData);
}

duin::JSONValue duin::SceneBuilder::SerializeComponent(const PackedComponent &pc)
{
    JSONValue json = JSONValue::Parse(pc.jsonData);
    return json;
}

duin::PackedComponent duin::SceneBuilder::DeserializeComponent(const JSONValue &comp)
{
    PackedComponent pc;

    if (!comp.HasMember("type"))
    {
        DN_CORE_WARN("SceneBuilder::DeserializeComponent - Component missing 'type' field, skipping");
        return pc;
    }

    pc.componentTypeName = comp.GetMember("type").GetString();
    pc.jsonData = comp.Write();

    return pc;
}

// PackPair
duin::PackedPair duin::SceneBuilder::PackPair(Entity e, Entity::ID pairId)
{
    PackedPair pp;

    if (!pairId.IsPair())
    {
        DN_CORE_WARN("SceneBuilder::PackPair - ID is not a pair: {}", pairId.Str());
        return pp;
    }

    Entity first = pairId.First();
    pp.relationshipName = first.GetName();

    Entity second = pairId.Second();
    pp.targetName = second.GetName();

    return pp;
}

duin::PackedPair duin::SceneBuilder::PackPairWRelationshipEntity(Entity::ID pairID, UUID rUUID)
{
    PackedPair pp;

    pp.relationshipName = pairID.First().GetName();
    pp.relationshipUUID = rUUID;

    pp.targetName = pairID.Second().GetName();
    pp.targetUUID = UUID::INVALID;

    return pp;
}

duin::PackedPair duin::SceneBuilder::PackPairWTargetEntity(Entity::ID pairID, UUID tUUID)
{
    PackedPair pp;

    pp.relationshipName = pairID.First().GetName();
    pp.relationshipUUID = UUID::INVALID;

    pp.targetName = pairID.Second().GetName();
    pp.targetUUID = tUUID;

    return pp;
}

duin::PackedPair duin::SceneBuilder::PackPairWBothEntities(Entity::ID pairID, UUID rUUID, UUID tUUID)
{
    PackedPair pp;

    pp.relationshipName = pairID.First().GetName();
    pp.relationshipUUID = rUUID;

    pp.targetName = pairID.Second().GetName();
    pp.targetUUID = tUUID;

    return pp;
}

void duin::SceneBuilder::InstantiatePair(const PackedPair &pp, Entity e)
{
    World *w = e.GetWorld();
    if (!w)
    {
        DN_CORE_WARN("SceneBuilder::InstantiatePair - Entity has no world");
        return;
    }

    // Special case: IsA uses PrefabRegistry for prefab lookup.
    if (pp.relationshipName == "IsA")
    {
        Entity targetEntity = PrefabRegistry::Get().Find(pp.targetName);
        if (targetEntity.IsValid())
        {
            e.IsA(targetEntity);
            DN_CORE_INFO("Instantiated IsA pair: ({}, {}) on entity {}", pp.relationshipName, pp.targetName,
                         e.GetName());
            return;
        }
    }
    if (pp.relationshipName == "ChildOf")
    {
        return;
    }

    // For component/tag-type relationships we must look up the entity using its full FLECS path
    // (e.g. "::TestSceneBuilder::Targets") because the short name alone is insufficient when
    // the type lives inside a C++ namespace scope.  World::Lookup wraps flecs::world::lookup
    // and uses "::" as the separator, matching how C++ component paths are stored.
    auto lookupComponent = [&](const std::string &path, const std::string &name) -> Entity {
        const std::string &key = path.empty() ? name : path;
        return w->Lookup(key);
    };

    Entity relationship;
    if (pp.relationshipIsComponent)
    {
        relationship = lookupComponent(pp.relationshipPath, pp.relationshipName);
    }
    else
    {
        if (pp.relationshipUUID != UUID::INVALID)
        {
            auto it = packedEntityToInstanceMap.find(pp.relationshipUUID);
            if (it != packedEntityToInstanceMap.end())
            {
                relationship = w->MakeAlive(it->second);
            }
        }
        if (!relationship.IsValid())
        {
            relationship = w->Lookup(pp.relationshipName);
        }
    }

    // Resolve target entity.
    // Same logic: component types found via full path; plain entities via UUID/name.
    Entity target;
    if (pp.targetIsComponent)
    {
        target = lookupComponent(pp.targetPath, pp.targetName);
    }
    else
    {
        if (pp.targetUUID != UUID::INVALID)
        {
            auto it = packedEntityToInstanceMap.find(pp.targetUUID);
            if (it != packedEntityToInstanceMap.end())
            {
                target = w->MakeAlive(it->second);
            }
        }
        if (!target.IsValid())
        {
            target = w->Lookup(pp.targetName);
        }
    }

    if (relationship.IsValid() && target.IsValid())
    {
        // Build the pair ID from the two resolved entity IDs.  For component-type
        // relationships this produces (component_type_id, target_id) which matches
        // what has<Relation>(target) and Has<Relation>(target) check.
        //e.Add(flecs::id(w->GetFlecsWorld().c_ptr(), relationship.GetID(), target.GetID()));
        e.Add(relationship, target);
        DN_CORE_INFO("Instantiated pair: ({}, {}) on entity {}", pp.relationshipName, pp.targetName, e.GetName());
    }
    else
    {
        DN_CORE_WARN("SceneBuilder::InstantiatePair - Could not resolve ({}, {})", pp.relationshipName, pp.targetName);
    }
}

duin::JSONValue duin::SceneBuilder::SerializePair(const PackedPair &pp)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(PackedPair::TAG_RELATIONSHIPNAME, pp.relationshipName);
    json.AddMember(PackedPair::TAG_RELATIONSHIPUUID, pp.relationshipUUID.ToStrHex());
    json.AddMember(PackedPair::TAG_RELATIONSHIP_IS_COMPONENT, pp.relationshipIsComponent);
    if (!pp.relationshipPath.empty())
        json.AddMember(PackedPair::TAG_RELATIONSHIPPATH, pp.relationshipPath);
    json.AddMember(PackedPair::TAG_TARGETNAME, pp.targetName);
    json.AddMember(PackedPair::TAG_TARGETUUID, pp.targetUUID.ToStrHex());
    json.AddMember(PackedPair::TAG_TARGET_IS_COMPONENT, pp.targetIsComponent);
    if (!pp.targetPath.empty())
        json.AddMember(PackedPair::TAG_TARGETPATH, pp.targetPath);

    if (!pp.jsonData.empty())
    {
        json.AddMember(PackedPair::TAG_DATA, pp.jsonData);
    }

    return json;
}

duin::PackedPair duin::SceneBuilder::DeserializePair(const JSONValue &pair)
{
    PackedPair pp;

    if (pair.HasMember(PackedPair::TAG_RELATIONSHIPNAME))
    {
        pp.relationshipName = pair.GetMember(PackedPair::TAG_RELATIONSHIPNAME).GetString();
    }

    if (pair.HasMember(PackedPair::TAG_RELATIONSHIPUUID))
    {
        pp.relationshipUUID = UUID::FromStringHex(pair.GetMember(PackedPair::TAG_RELATIONSHIPUUID).GetString());
    }

    if (pair.HasMember(PackedPair::TAG_RELATIONSHIP_IS_COMPONENT))
    {
        pp.relationshipIsComponent = pair.GetMember(PackedPair::TAG_RELATIONSHIP_IS_COMPONENT).GetBool();
    }

    if (pair.HasMember(PackedPair::TAG_RELATIONSHIPPATH))
    {
        pp.relationshipPath = pair.GetMember(PackedPair::TAG_RELATIONSHIPPATH).GetString();
    }

    if (pair.HasMember(PackedPair::TAG_TARGETNAME))
    {
        pp.targetName = pair.GetMember(PackedPair::TAG_TARGETNAME).GetString();
    }

    if (pair.HasMember(PackedPair::TAG_TARGETUUID))
    {
        pp.targetUUID = UUID::FromStringHex(pair.GetMember(PackedPair::TAG_TARGETUUID).GetString());
    }

    if (pair.HasMember(PackedPair::TAG_TARGET_IS_COMPONENT))
    {
        pp.targetIsComponent = pair.GetMember(PackedPair::TAG_TARGET_IS_COMPONENT).GetBool();
    }

    if (pair.HasMember(PackedPair::TAG_TARGETPATH))
    {
        pp.targetPath = pair.GetMember(PackedPair::TAG_TARGETPATH).GetString();
    }

    if (pair.HasMember(PackedPair::TAG_DATA))
    {
        pp.jsonData = pair.GetMember(PackedPair::TAG_DATA).GetString();
    }

    return pp;
}

// PackEntity (SceneBuilder instance method)
duin::PackedEntity duin::SceneBuilder::PackEntity(Entity e)
{
    PackedEntity pe;
    if (pe.name.find('#') == pe.name.npos)
    {
        pe.name = e.GetName();
    }
    else
    {
        pe.name = e.GetName().substr(0, pe.name.find('#'));
    }
    pe.enabled = e.IsAlive();

    // Use UUID pre-assigned by PrePassEntity (if available), otherwise generate one now.
    auto mapIt = instanceToPackedEntityMap.find(e.GetID());
    if (mapIt != instanceToPackedEntityMap.end())
    {
        pe.uuid = mapIt->second;
    }
    else
    {
        instanceToPackedEntityMap[e.GetID()] = pe.uuid;
    }

    // Check if entity is a "plain entity" (no type): returns true for bare entities.
    auto isEntity = [](const Entity &ent) -> bool {
        flecs::entity fe = ent.GetFlecsEntity();
        if (!fe.is_valid())
            return false;
        if (fe.is_entity() && !fe.try_get<flecs::Component>())
            return true;
        return false;
        // const ecs_type_t *type = ecs_get_type(fe.world().c_ptr(), fe.id());
        // return (type == nullptr);
    };

    e.ForEachComponent([this, e, &pe, &isEntity](duin::Entity::ID id) {
        if (id.IsPair())
        {
            // Filter FLECS-internal pairs; ChildOf is captured via GetChildren hierarchy.
            std::string relName = id.First().GetName();
            if (relName == "ChildOf" || relName == "Identifier")
            {
                return;
            }

            bool relationshipIsEntity = isEntity(id.First());
            bool targetIsEntity = isEntity(id.Second());

            UUID rUUID = UUID::INVALID;
            if (relationshipIsEntity)
            {
                auto it = instanceToPackedEntityMap.find(id.First().GetID());
                if (it != instanceToPackedEntityMap.end())
                {
                    rUUID = it->second;
                }
            }

            UUID tUUID = UUID::INVALID;
            if (targetIsEntity)
            {
                auto it = instanceToPackedEntityMap.find(id.Second().GetID());
                if (it != instanceToPackedEntityMap.end())
                {
                    tUUID = it->second;
                }
            }

            PackedPair pp;
            if (rUUID != UUID::INVALID && tUUID != UUID::INVALID)
            {
                pp = PackPairWBothEntities(id, rUUID, tUUID);
            }
            else if (rUUID != UUID::INVALID)
            {
                pp = PackPairWRelationshipEntity(id, rUUID);
            }
            else if (tUUID != UUID::INVALID)
            {
                pp = PackPairWTargetEntity(id, tUUID);
            }
            else
            {
                pp = PackPair(e, id);
            }

            pp.relationshipIsComponent = !relationshipIsEntity;
            if (pp.relationshipIsComponent)
                pp.relationshipPath = id.First().GetPath();

            pp.targetIsComponent = !targetIsEntity;
            if (pp.targetIsComponent)
                pp.targetPath = id.Second().GetPath();

            pe.pairs.push_back(pp);
        }
        else if (id.IsEntity())
        {
            duin::Entity cmp = id.GetEntity();
            duin::PackedComponent pc = PackComponent(e, cmp);

            if (!pc.componentTypeName.empty())
            {
                if (cmp.IsTag())
                    pe.tags.push_back(pc);
                else
                    pe.components.push_back(pc);
            }
            else
            {
                DN_CORE_WARN("Skipping invalid component during pack: {}", cmp.GetName());
            }
        }
        // Other ID types (wildcards, etc.) are silently ignored.
    });

    for (Entity child : e.GetChildren())
    {
        pe.children.push_back(PackEntity(child));
    }

    // Record reverse mapping (pre-pass already set forward mapping).
    packedEntityToInstanceMap[pe.uuid] = e.GetID();

    return pe;
}

void duin::SceneBuilder::InstantiateEntity(const PackedEntity &pe, Entity e)
{
    packedEntityToInstanceMap[pe.uuid] = e.GetID();

    if (!pe.name.empty())
    {
        std::string name = pe.name;
        // Only append a disambiguation suffix when another entity (not this one) already
        // holds the same name.  world->Lookup returns the pre-pass entity itself which
        // already has this name, so we must exclude it from the duplicate check.
        Entity existing = world->Lookup(pe.name);
        if (existing.IsValid() && existing.GetID() != e.GetID())
        {
            name = name + "#" + static_cast<std::string>(UUID::ToStringHex(e.GetID()));
        }
        e.SetName(name);
    }

    if (!pe.enabled)
    {
        e.Disable();
    }

    for (const PackedComponent &tag : pe.tags)
    {
        InstantiateComponent(tag, e);
    }

    for (const PackedPair &pair : pe.pairs)
    {
        InstantiatePair(pair, e);
    }

    for (const PackedComponent &cmp : pe.components)
    {
        InstantiateComponent(cmp, e);
    }

    for (const PackedEntity &child : pe.children)
    {
        // Look up child entity created by pre-pass; fall back to creating a new one.
        auto it = packedEntityToInstanceMap.find(child.uuid);
        if (it != packedEntityToInstanceMap.end())
        {
            Entity childEntity = world->MakeAlive(it->second);
            InstantiateEntity(child, childEntity);
        }
        else
        {
            Entity childEntity = world->CreateEntity();
            childEntity.ChildOf(e);
            InstantiateEntity(child, childEntity);
        }
    }
}

duin::JSONValue duin::SceneBuilder::SerializeEntity(const PackedEntity &pe)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(PackedEntity::TAG_UUID, UUID::ToStringHex(pe.uuid));
    json.AddMember(PackedEntity::TAG_NAME, pe.name);
    json.AddMember(PackedEntity::TAG_ENABLED, pe.enabled);

    JSONValue tagsArray;
    tagsArray.SetArray();
    for (const auto &tag : pe.tags)
    {
        JSONValue cmpJSON = JSONValue::Parse(tag.jsonData);
        tagsArray.PushBack(cmpJSON);
    }
    json.AddMember(PackedEntity::TAG_TAGS, tagsArray);

    JSONValue pairsArray;
    pairsArray.SetArray();
    for (const auto &pair : pe.pairs)
    {
        JSONValue pairJSON = SerializePair(pair);
        pairsArray.PushBack(pairJSON);
    }
    json.AddMember(PackedEntity::TAG_PAIRS, pairsArray);

    JSONValue componentsArray;
    componentsArray.SetArray();
    for (const auto &cmp : pe.components)
    {
        JSONValue cmpJSON = JSONValue::Parse(cmp.jsonData);
        componentsArray.PushBack(cmpJSON);
    }
    json.AddMember(PackedEntity::TAG_COMPONENTS, componentsArray);

    JSONValue childrenArray;
    childrenArray.SetArray();
    for (const auto &child : pe.children)
    {
        JSONValue eJSON = SerializeEntity(child);
        childrenArray.PushBack(eJSON);
    }
    json.AddMember(PackedEntity::TAG_CHILDREN, childrenArray);

    return json;
}

duin::PackedEntity duin::SceneBuilder::DeserializeEntity(const JSONValue &json)
{
    PackedEntity pe;

    if (json.HasMember(PackedEntity::TAG_UUID))
    {
        pe.uuid = UUID::FromStringHex(json.GetMember(PackedEntity::TAG_UUID).GetString());
    }

    if (json.HasMember(PackedEntity::TAG_NAME))
    {
        pe.name = json.GetMember(PackedEntity::TAG_NAME).GetString();
    }

    if (json.HasMember(PackedEntity::TAG_ENABLED))
    {
        pe.enabled = json.GetMember(PackedEntity::TAG_ENABLED).GetBool();
    }

    if (json.HasMember(PackedEntity::TAG_TAGS))
    {
        JSONValue tagsArray = json.GetMember(PackedEntity::TAG_TAGS);
        if (tagsArray.IsArray())
        {
            for (auto it = tagsArray.Begin(); it != tagsArray.End(); ++it)
            {
                JSONValue cmpJSON(it.GetValue());
                if (!cmpJSON.HasMember("type"))
                {
                    DN_CORE_WARN("SceneBuilder::DeserializeEntity - Skipping tag without 'type' field");
                    continue;
                }
                PackedComponent cmp;
                cmp.componentTypeName = cmpJSON.GetMember("type").GetString();
                cmp.jsonData = cmpJSON.Write();
                if (!cmp.componentTypeName.empty())
                {
                    pe.tags.push_back(cmp);
                }
            }
        }
    }

    if (json.HasMember(PackedEntity::TAG_PAIRS))
    {
        JSONValue pairsArray = json.GetMember(PackedEntity::TAG_PAIRS);
        if (pairsArray.IsArray())
        {
            for (auto it = pairsArray.Begin(); it != pairsArray.End(); ++it)
            {
                JSONValue pairJSON(it.GetValue());
                if (!pairJSON.HasMember(PackedPair::TAG_RELATIONSHIPNAME) ||
                    !pairJSON.HasMember(PackedPair::TAG_TARGETNAME))
                {
                    DN_CORE_WARN(
                        "SceneBuilder::DeserializeEntity - Skipping pair without 'relationship' or 'target' field");
                    continue;
                }
                PackedPair pair = DeserializePair(pairJSON);
                if (!pair.relationshipName.empty() && !pair.targetName.empty())
                {
                    pe.pairs.push_back(pair);
                }
            }
        }
    }

    if (json.HasMember(PackedEntity::TAG_COMPONENTS))
    {
        JSONValue componentsArray = json.GetMember(PackedEntity::TAG_COMPONENTS);
        if (componentsArray.IsArray())
        {
            for (auto it = componentsArray.Begin(); it != componentsArray.End(); ++it)
            {
                JSONValue cmpJSON(it.GetValue());
                if (!cmpJSON.HasMember("type"))
                {
                    DN_CORE_WARN("SceneBuilder::DeserializeEntity - Skipping component without 'type' field");
                    continue;
                }
                PackedComponent cmp;
                cmp.componentTypeName = cmpJSON.GetMember("type").GetString();
                cmp.jsonData = cmpJSON.Write();
                if (!cmp.componentTypeName.empty())
                {
                    pe.components.push_back(cmp);
                }
            }
        }
    }

    if (json.HasMember(PackedEntity::TAG_CHILDREN))
    {
        JSONValue childrenArray = json.GetMember(PackedEntity::TAG_CHILDREN);
        if (childrenArray.IsArray())
        {
            for (auto it = childrenArray.Begin(); it != childrenArray.End(); ++it)
            {
                JSONValue eJSON(it.GetValue());
                pe.children.push_back(DeserializeEntity(eJSON));
            }
        }
    }

    return pe;
}

// ExternalDependency
duin::JSONValue duin::SceneBuilder::SerializeExternalDependency(const PackedExternalDependency &ped)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(PackedExternalDependency::TAG_UUID, UUID::ToStringHex(ped.uuid));
    json.AddMember(PackedExternalDependency::TAG_TYPE, ped.type);

    return json;
}

duin::PackedExternalDependency duin::SceneBuilder::DeserializeExternalDependency(const JSONValue &exdep)
{
    PackedExternalDependency ped;

    if (exdep.HasMember(PackedExternalDependency::TAG_UUID))
    {
        ped.uuid = UUID::FromStringHex(exdep.GetMember(PackedExternalDependency::TAG_UUID).GetString());
    }

    if (exdep.HasMember(PackedExternalDependency::TAG_TYPE))
    {
        ped.type = exdep.GetMember(PackedExternalDependency::TAG_TYPE).GetString();
    }

    return ped;
}

// Metadata
duin::JSONValue duin::SceneBuilder::SerializeMetadata(const PackedSceneMetadata &metadata)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(PackedSceneMetadata::TAG_EDITORVERSION, metadata.editorVersion);
    json.AddMember(PackedSceneMetadata::TAG_ENGINEVERSION, metadata.engineVersion);
    json.AddMember(PackedSceneMetadata::TAG_LASTMODIFIED, metadata.lastModified);
    json.AddMember(PackedSceneMetadata::TAG_AUTHOR, metadata.author);

    return json;
}

duin::PackedSceneMetadata duin::SceneBuilder::DeserializeMetadata(const JSONValue &metadata)
{
    PackedSceneMetadata psm;

    if (metadata.HasMember(PackedSceneMetadata::TAG_EDITORVERSION))
    {
        psm.editorVersion = metadata.GetMember(PackedSceneMetadata::TAG_EDITORVERSION).GetString();
    }

    if (metadata.HasMember(PackedSceneMetadata::TAG_ENGINEVERSION))
    {
        psm.engineVersion = metadata.GetMember(PackedSceneMetadata::TAG_ENGINEVERSION).GetString();
    }

    if (metadata.HasMember(PackedSceneMetadata::TAG_LASTMODIFIED))
    {
        psm.lastModified = metadata.GetMember(PackedSceneMetadata::TAG_LASTMODIFIED).GetString();
    }

    if (metadata.HasMember(PackedSceneMetadata::TAG_AUTHOR))
    {
        psm.author = metadata.GetMember(PackedSceneMetadata::TAG_AUTHOR).GetString();
    }

    return psm;
}

// Scene
duin::JSONValue duin::SceneBuilder::SerializeScene(const PackedScene &pscn)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(PackedScene::TAG_SCENEUUID, UUID::ToStringHex(pscn.uuid));
    json.AddMember(PackedScene::TAG_SCENENAME, pscn.name);

    // Metadata
    {
        JSONValue meta;
        meta.SetObject();
        meta.AddMember(PackedSceneMetadata::TAG_EDITORVERSION, pscn.metadata.editorVersion);
        meta.AddMember(PackedSceneMetadata::TAG_ENGINEVERSION, pscn.metadata.engineVersion);
        meta.AddMember(PackedSceneMetadata::TAG_LASTMODIFIED, pscn.metadata.lastModified);
        meta.AddMember(PackedSceneMetadata::TAG_AUTHOR, pscn.metadata.author);
        json.AddMember(PackedScene::TAG_METADATA, meta);
    }

    // External dependencies
    JSONValue depsArray;
    depsArray.SetArray();
    for (const auto &dep : pscn.externalDependencies)
    {
        JSONValue depJSON;
        depJSON.SetObject();
        depJSON.AddMember(PackedExternalDependency::TAG_UUID, UUID::ToStringHex(dep.uuid));
        depJSON.AddMember(PackedExternalDependency::TAG_TYPE, dep.type);
        depsArray.PushBack(depJSON);
    }
    json.AddMember(PackedScene::TAG_EXTERNALDEPENDENCIES, depsArray);

    // Entities
    JSONValue entitiesArray;
    entitiesArray.SetArray();
    for (const auto &entity : pscn.entities)
    {
        entitiesArray.PushBack(SerializeEntity(entity));
    }
    json.AddMember(PackedScene::TAG_ENTITIES, entitiesArray);

    return json;
}

duin::PackedScene duin::SceneBuilder::DeserializeScene(const JSONValue &scene)
{
    PackedScene ps;

    if (scene.HasMember(PackedScene::TAG_SCENEUUID))
    {
        ps.uuid = UUID::FromStringHex(scene.GetMember(PackedScene::TAG_SCENEUUID).GetString());
    }

    if (scene.HasMember(PackedScene::TAG_SCENENAME))
    {
        ps.name = scene.GetMember(PackedScene::TAG_SCENENAME).GetString();
    }

    if (scene.HasMember(PackedScene::TAG_METADATA))
    {
        JSONValue meta = scene.GetMember(PackedScene::TAG_METADATA);
        if (meta.HasMember(PackedSceneMetadata::TAG_EDITORVERSION))
            ps.metadata.editorVersion = meta.GetMember(PackedSceneMetadata::TAG_EDITORVERSION).GetString();
        if (meta.HasMember(PackedSceneMetadata::TAG_ENGINEVERSION))
            ps.metadata.engineVersion = meta.GetMember(PackedSceneMetadata::TAG_ENGINEVERSION).GetString();
        if (meta.HasMember(PackedSceneMetadata::TAG_LASTMODIFIED))
            ps.metadata.lastModified = meta.GetMember(PackedSceneMetadata::TAG_LASTMODIFIED).GetString();
        if (meta.HasMember(PackedSceneMetadata::TAG_AUTHOR))
            ps.metadata.author = meta.GetMember(PackedSceneMetadata::TAG_AUTHOR).GetString();
    }

    if (scene.HasMember(PackedScene::TAG_EXTERNALDEPENDENCIES))
    {
        JSONValue depsArray = scene.GetMember(PackedScene::TAG_EXTERNALDEPENDENCIES);
        if (depsArray.IsArray())
        {
            for (auto it = depsArray.Begin(); it != depsArray.End(); ++it)
            {
                JSONValue depJSON(it.GetValue());
                PackedExternalDependency dep;
                if (depJSON.HasMember(PackedExternalDependency::TAG_UUID))
                    dep.uuid = UUID::FromStringHex(depJSON.GetMember(PackedExternalDependency::TAG_UUID).GetString());
                if (depJSON.HasMember(PackedExternalDependency::TAG_TYPE))
                    dep.type = depJSON.GetMember(PackedExternalDependency::TAG_TYPE).GetString();
                ps.externalDependencies.push_back(dep);
            }
        }
    }

    if (scene.HasMember(PackedScene::TAG_ENTITIES))
    {
        JSONValue entitiesArray = scene.GetMember(PackedScene::TAG_ENTITIES);
        if (entitiesArray.IsArray())
        {
            for (auto it = entitiesArray.Begin(); it != entitiesArray.End(); ++it)
            {
                JSONValue eJSON(it.GetValue());
                ps.entities.push_back(DeserializeEntity(eJSON));
            }
        }
    }

    return ps;
}

// Pre-pass helpers

void duin::SceneBuilder::PrePassEntity(Entity e)
{
    UUID uuid; // fresh random UUID
    instanceToPackedEntityMap[e.GetID()] = uuid;
    for (Entity child : e.GetChildren())
        PrePassEntity(child);
}

void duin::SceneBuilder::PrePassInstantiate(const PackedEntity &pe, Entity parent)
{
    Entity e = world->CreateEntity();
    if (parent.IsValid())
        e.ChildOf(parent);
    if (!pe.name.empty())
        e.SetName(pe.name);
    packedEntityToInstanceMap[pe.uuid] = e.GetID();
    for (const PackedEntity &child : pe.children)
        PrePassInstantiate(child, e);
}

void duin::SceneBuilder::InstantiateScene(PackedScene &pscn, World *world)
{
    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();

    // Pre-pass: create all entities so pair references can be resolved across root entities.
    for (PackedEntity &pEntity : pscn.entities)
    {
        PrePassInstantiate(pEntity, Entity{});
    }

    // Main pass: apply components and pairs using the populated map.
    for (PackedEntity &pEntity : pscn.entities)
    {
        auto it = packedEntityToInstanceMap.find(pEntity.uuid);
        if (it != packedEntityToInstanceMap.end())
        {
            Entity e = this->world->MakeAlive(it->second);
            InstantiateEntity(pEntity, e);
        }
    }

    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();
}

void duin::SceneBuilder::InstantiateSceneAsChildren(PackedScene &pscn, Entity parent)
{
    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();

    World *w = parent.GetWorld();
    if (!w)
        return;

    // Pre-pass: create all entities as children of parent.
    for (PackedEntity &pEntity : pscn.entities)
    {
        PrePassInstantiate(pEntity, parent);
    }

    // Main pass: apply components and pairs.
    for (PackedEntity &pEntity : pscn.entities)
    {
        auto it = packedEntityToInstanceMap.find(pEntity.uuid);
        if (it != packedEntityToInstanceMap.end())
        {
            Entity e = w->MakeAlive(it->second);
            InstantiateEntity(pEntity, e);
        }
    }

    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();
}

duin::PackedScene duin::SceneBuilder::PackScene(World *world)
{
    std::vector<Entity> children;
    world->IterateChildren([&children](Entity e) {
        // world::children() returns all entities with any ChildOf relationship,
        // including FLECS built-in scope entities that must not be serialized.
        // Filter out known FLECS internal entity categories:
        //   flecs::Component  (struct type)  — component/tag type descriptor entities
        //   flecs::Observer   (entity_t tag) — internal observer entities
        //   flecs::Module     (entity_t tag) — module scope entities
        flecs::entity fe = e.GetFlecsEntity();
        if (fe.has<flecs::Component>())
            return;
        if (fe.has(flecs::Observer))
            return;
        if (fe.has(flecs::Module))
            return;
        children.push_back(e);
    });

    return PackScene(children);
}

duin::PackedScene duin::SceneBuilder::PackScene(std::shared_ptr<World> world)
{
    return PackScene(world.get());
}

duin::PackedScene duin::SceneBuilder::PackScene(const std::vector<Entity> &vecEntities)
{
    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();

    PackedScene packedScene;

    // Pre-pass: assign UUIDs for all entities before packing.
    for (Entity e : vecEntities)
    {
        PrePassEntity(e);
    }

    for (Entity e : vecEntities)
    {
        packedScene.entities.push_back(PackEntity(e));
    }

    instanceToPackedEntityMap.clear();
    packedEntityToInstanceMap.clear();

    return packedScene;
}
