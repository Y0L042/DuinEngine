#include "dnpch.h"
#include "SceneBuilder.h"
#include "Duin/ECS/GameWorld.h"
#include "Duin/ECS/DECS/Entity.h"
#include "Duin/IO/JSONValue.h"
#include "Duin/IO/FileUtils.h"
#include <flecs.h>
#include "Duin/ECS/ComponentSerializer.h"
#include "Duin/ECS/PrefabRegistry.h"

#define PRETTY_WRITE_JSON

#ifdef PRETTY_WRITE_JSON
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif

// Static TAG definitions
const std::string duin::PackedComponent::TAG_JSON = "json";

const std::string duin::PackedPair::TAG_RELATIONSHIP = "relationship";
const std::string duin::PackedPair::TAG_TARGET = "target";
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

// PackedComponent
duin::PackedComponent duin::PackedComponent::Deserialize(const JSONValue &json)
{
    PackedComponent pc;

    if (!json.HasMember("type"))
    {
        DN_CORE_WARN("PackedComponent::Deserialize - Component missing 'type' field, skipping");
        return pc;
    }

    pc.componentTypeName = json.GetMember("type").GetString();
    pc.jsonData = json.Write();

    return pc;
}

duin::JSONValue duin::PackedComponent::Serialize(const PackedComponent &pComp)
{
    JSONValue json = JSONValue::Parse(pComp.jsonData);

    return json;
}

duin::PackedComponent duin::PackedComponent::Pack(Entity e, Entity cmp)
{
    std::string cmpstr;
    if (cmp.IsTag())
    {
        /* Tag */
        cmpstr = duin::ComponentSerializer::Get().Serialize(cmp.GetName(), nullptr);
    }
    else if (cmp.IsPair())
    {
        /* Pair */
        Entity first = cmp.First();
        Entity second = cmp.Second();
        cmpstr = duin::ComponentSerializer::Get().Serialize(first.GetName(), second.Get(cmp));
    }
    else
    {
        /* Component */
        cmpstr = duin::ComponentSerializer::Get().Serialize(cmp.GetName(), e.Get(cmp));
    }

    if (cmpstr.empty())
    {
        DN_CORE_WARN("PackedComponent::Pack - Failed to serialize component: {}", cmp.GetName());
        return PackedComponent();
    }

    JSONValue v = JSONValue::Parse(cmpstr);

    if (!v.HasMember("type"))
    {
        DN_CORE_WARN("PackedComponent::Pack - Serialized component missing 'type' field: {}", cmp.GetName());
        return PackedComponent();
    }

    PackedComponent pcmp;
    pcmp.componentTypeName = v["type"].GetString();
    pcmp.jsonData = v.Write();

    return pcmp;
}

void duin::PackedComponent::Instantiate(const PackedComponent &p, Entity e)
{
    void *data = nullptr;
    ComponentSerializer::Get().Deserialize(e, p.componentTypeName, data, p.jsonData);
}

// PackedPair
duin::PackedPair duin::PackedPair::Pack(Entity e, Entity::ID pairId)
{
    PackedPair pp;

    if (!pairId.IsPair())
    {
        DN_CORE_WARN("PackedPair::Pack - ID is not a pair: {}", pairId.Str());
        return pp;
    }

    // Get the first element (relationship)
    Entity first = pairId.First();
    pp.relationshipName = first.GetName();

    // Get the second element (target)
    Entity second = pairId.Second();
    pp.targetName = second.GetName();

    // Try to get data from the pair
    // In FLECS, data is stored based on which element is a component
    const void *pairData = nullptr;
    if (e.Has(first))
    {
        pairData = e.Get(first);
    }
    if (pairData != nullptr)
    {
        // Data is stored on the first element (relationship)
        pp.jsonData = ComponentSerializer::Get().Serialize(first.GetName(), pairData);
    }
    else
    {
        // Check if data is stored on the second element
        if (e.Has(second))
        {
            pairData = e.Get(second);
        }
        if (pairData != nullptr)
        {
            pp.jsonData = ComponentSerializer::Get().Serialize(second.GetName(), pairData);
        }
        else
        {
            // No data - this is a tag pair
            pp.jsonData = "";
        }
    }

    DN_CORE_INFO("Packed pair: ({}, {}) with data: {}", pp.relationshipName, pp.targetName,
                 pp.jsonData.empty() ? "none" : "present");

    return pp;
}

duin::JSONValue duin::PackedPair::Serialize(const PackedPair &ppair)
{
    JSONValue json;
    json.SetObject();

    json.AddMember(TAG_RELATIONSHIP, ppair.relationshipName);
    json.AddMember(TAG_TARGET, ppair.targetName);

    if (!ppair.jsonData.empty())
    {
        json.AddMember(TAG_DATA, ppair.jsonData);
    }

    return json;
}

duin::PackedPair duin::PackedPair::Deserialize(const JSONValue &json)
{
    PackedPair pp;

    if (json.HasMember(TAG_RELATIONSHIP))
    {
        pp.relationshipName = json.GetMember(TAG_RELATIONSHIP).GetString();
    }

    if (json.HasMember(TAG_TARGET))
    {
        pp.targetName = json.GetMember(TAG_TARGET).GetString();
    }

    if (json.HasMember(TAG_DATA))
    {
        pp.jsonData = json.GetMember(TAG_DATA).GetString();
    }

    return pp;
}

void duin::PackedPair::Instantiate(const PackedPair &p, Entity e)
{
    if (p.relationshipName.empty() || p.targetName.empty())
    {
        DN_CORE_WARN("PackedPair::Instantiate - Invalid pair (empty relationship or target)");
        return;
    }

    World *world = e.GetWorld();
    if (!world)
    {
        DN_CORE_WARN("PackedPair::Instantiate - Entity has no world");
        return;
    }

    // Apply inheritance pair
    if (p.relationshipName == "IsA")
    {
        Entity targetEntity = PrefabRegistry::Get().Find(p.targetName);
        if (targetEntity.IsValid())
        {
            e.IsA(targetEntity);
        }
    }

    DN_CORE_INFO("Instantiated pair: ({}, {}) on entity {}", p.relationshipName, p.targetName, e.GetName());
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
        JSONValue componentsArray = json.GetMember(pe.TAG_TAGS);
        if (componentsArray.IsArray())
        {
            for (auto it = componentsArray.Begin(); it != componentsArray.End(); ++it)
            {
                JSONValue cmpJSON(it.GetValue());

                if (!cmpJSON.HasMember("type"))
                {
                    DN_CORE_WARN("Skipping component without 'type' field");
                    continue;
                }

                PackedComponent cmp = PackedComponent::Deserialize(cmpJSON);
                if (!cmp.componentTypeName.empty())
                {
                    pe.tags.push_back(cmp);
                }
            }
        }
    }

    if (json.HasMember(pe.TAG_PAIRS))
    {
        JSONValue pairsArray = json.GetMember(pe.TAG_PAIRS);
        if (pairsArray.IsArray())
        {
            for (auto it = pairsArray.Begin(); it != pairsArray.End(); ++it)
            {
                JSONValue pairJSON(it.GetValue());

                if (!pairJSON.HasMember(PackedPair::TAG_RELATIONSHIP) || !pairJSON.HasMember(PackedPair::TAG_TARGET))
                {
                    DN_CORE_WARN("Skipping pair without 'relationship' or 'target' field");
                    continue;
                }

                PackedPair pair = PackedPair::Deserialize(pairJSON);
                if (!pair.relationshipName.empty() && !pair.targetName.empty())
                {
                    pe.pairs.push_back(pair);
                }
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

                if (!cmpJSON.HasMember("type"))
                {
                    DN_CORE_WARN("Skipping component without 'type' field");
                    continue;
                }

                PackedComponent cmp = PackedComponent::Deserialize(cmpJSON);
                if (!cmp.componentTypeName.empty())
                {
                    pe.components.push_back(cmp);
                }
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
    for (const auto &component : entity.tags)
    {
        JSONValue cmpJSON = PackedComponent::Serialize(component);
        tagsArray.PushBack(cmpJSON);
    }
    json.AddMember(entity.TAG_TAGS, tagsArray);

    JSONValue pairsArray;
    pairsArray.SetArray();
    for (const auto &pair : entity.pairs)
    {
        JSONValue pairJSON = PackedPair::Serialize(pair);
        pairsArray.PushBack(pairJSON);
    }
    json.AddMember(entity.TAG_PAIRS, pairsArray);

    JSONValue componentsArray;
    componentsArray.SetArray();
    for (const auto &component : entity.components)
    {
        JSONValue cmpJSON = PackedComponent::Serialize(component);
        componentsArray.PushBack(cmpJSON);
    }
    json.AddMember(entity.TAG_COMPONENTS, componentsArray);

    JSONValue childrenArray;
    childrenArray.SetArray();
    for (const auto &child : entity.children)
    {
        JSONValue eJSON = PackedEntity::Serialize(child);
        childrenArray.PushBack(eJSON);
    }
    json.AddMember(entity.TAG_CHILDREN, childrenArray);

    return json;
}

duin::PackedEntity duin::PackedEntity::Pack(Entity e)
{
    PackedEntity pe;
    pe.name = e.GetName();
    pe.enabled = e.IsAlive();

    e.ForEachComponent([e, &pe](duin::Entity::ID id) {
        if (id.IsPair())
        {
            // Handle FLECS pair (relationship, target)
            duin::PackedPair pp = duin::PackedPair::Pack(e, id);
            if (!pp.relationshipName.empty() && !pp.targetName.empty())
            {
                pe.pairs.push_back(pp);
            }
            else
            {
                DN_CORE_WARN("Skipping invalid pair during pack: {}", id.Str());
            }
        }
        else if (id.IsEntity())
        {
            // Handle regular component or tag
            duin::Entity cmp = id.GetEntity();
            duin::PackedComponent pc = duin::PackedComponent::Pack(e, cmp);

            if (!pc.componentTypeName.empty())
            {
                pe.components.push_back(pc);
            }
            else
            {
                DN_CORE_WARN("Skipping invalid component during pack: {}", cmp.GetName());
            }
        }
        else
        {
            DN_CORE_WARN("Skipping unknown ID type during pack: {}", id.Str());
        }
    });

    for (Entity child : e.GetChildren())
    {
        pe.children.push_back(PackedEntity::Pack(child));
    }

    return pe;
}

void duin::PackedEntity::Instantiate(const PackedEntity &pe, Entity e)
{
    if (!pe.name.empty())
    {
        e.SetName(pe.name);
    }

    if (!pe.enabled)
    {
        e.Disable();
    }

    // Instantiate components
    for (const duin::PackedComponent &cmp : pe.components)
    {
        if (cmp.componentTypeName == "Transform3DImpl")
        {
            DN_INFO("PAUSE");
        }
        duin::PackedComponent::Instantiate(cmp, e);
    }

    // Instantiate pairs
    for (const duin::PackedPair &pair : pe.pairs)
    {
        duin::PackedPair::Instantiate(pair, e);
    }

    // Instantiate children
    for (const PackedEntity &child : pe.children)
    {
        Entity childEntity = e.GetWorld()->CreateEntity();
        childEntity.ChildOf(e);
        PackedEntity::Instantiate(child, childEntity);
    }
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

void duin::PackedScene::Instantiate(PackedScene &pscn, World *world)
{
    for (PackedEntity &pEntity : pscn.entities)
    {
        Entity newEntity = world->CreateEntity();

        PackedEntity::Instantiate(pEntity, newEntity);
    }
}

duin::PackedScene duin::PackedScene::Pack(const std::vector<Entity> &vecEntities)
{
    PackedScene packedScene;

    // Pack the children of the scene root as root-level entities in the scene
    for (Entity e : vecEntities)
    {
        PackedEntity packedEntity = PackedEntity::Pack(e);
        packedScene.entities.push_back(packedEntity);
    }

    return packedScene;
}
