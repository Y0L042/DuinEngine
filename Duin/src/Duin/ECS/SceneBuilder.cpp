#include "dnpch.h"
#include "SceneBuilder.h"
#include "ECSManager.h"
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

const char duin::Scene::TAG_EXDEP_EXDEPUUID[] = "EXDEPUUID";
const char duin::Scene::TAG_EXDEP_PARENT[] = "PARENT";
const char duin::Scene::TAG_EXDEP_TYPE[] = "TYPE";

const char duin::Scene::TAG_UUID[] = "UUID";
const char duin::Scene::TAG_NAME[] = "NAME";
const char duin::Scene::TAG_FLECSJSON[] = "FLECSJSON";
const char duin::Scene::TAG_EXT_SCN_DEPS[] = "EXT_SCN_DEPS";

std::string duin::ResourceTypeToString(ResourceType type)
{
    std::string typeStr = "";
    switch (type)
    {
    case ResourceType::RES_SCENE:
		typeStr = "RES_SCENE";
        break;
	case ResourceType::RES_MODEL:
        typeStr = "RES_MODEL";
		break;
    case ResourceType::RES_TEXTURE:
		typeStr = "RES_TEXTURE";
        break;
	case ResourceType::RES_SHADER:
		typeStr = "RES_SHADER";
        break;
	case ResourceType::RES_AUDIO:
        typeStr = "RES_AUDIO";
		break;
	case ResourceType::RES_UNKNOWN:
    default:
        typeStr = "RES_UNKNOWN";
		break;
    }

    return typeStr;
}

duin::ResourceType duin::StringToResourceType(const std::string& string)
{
    ResourceType type = ResourceType::RES_UNKNOWN;
    if (string == "RES_SCENE") {
        type = ResourceType::RES_SCENE;
    }
    else if (string == "RES_MODEL") {
        type = ResourceType::RES_MODEL;
    }
    else if (string == "RES_TEXTURE") {
        type = ResourceType::RES_TEXTURE;
    }
    else if (string == "RES_SHADER") {
        type = ResourceType::RES_SHADER;
    }
    else if (string == "RES_AUDIO") {
        type = ResourceType::RES_AUDIO;
    }
    else /* if (string == "RES_UNKNOWN) */ {
        type = ResourceType::RES_UNKNOWN;
	}

    return type;
}

duin::Scene duin::Scene::ReadFromFile(const std::string& filePath)
{
    // TODO error handling
    std::string sceneStr = "";
    FileUtils::ReadFileIntoString(filePath, sceneStr);
	JSONValue sceneJSON = JSONValue::ParseFromFile(sceneStr);
    if (sceneJSON.IsReadValid()) {
        return ReadJSON(sceneJSON);
	}
    return Scene();
}

void duin::Scene::WriteToFile(const std::string& filePath)
{
    // TODO error handling
    JSONValue sceneJSON = WriteJSON();
    std::string sceneStr = sceneJSON.Write();
	FileUtils::WriteStringIntoFile(filePath, sceneStr);
}

duin::Scene duin::Scene::ReadJSON(JSONValue sceneJSON)
{
    // TODO add asserts and error handling

    Scene newScene;
    std::string uuidString = sceneJSON[TAG_UUID].GetString();
    newScene.uuid = UUID::FromStringHex(uuidString);
    newScene.name = sceneJSON[TAG_NAME].GetString();
    newScene.flecsJSON = sceneJSON[TAG_FLECSJSON].GetString();
    JSONValue exScnDepsArray = sceneJSON[TAG_EXT_SCN_DEPS];
    for (auto exDepJSON : exScnDepsArray)
    {
        ExDep exDep;
        std::string exdepUUIDString = exDepJSON[TAG_EXDEP_EXDEPUUID].GetString();
        exDep.exdepUUID = UUID::FromStringHex(exdepUUIDString);
        exDep.parent = exDepJSON[TAG_EXDEP_PARENT].GetInt();
		exDep.type = StringToResourceType(exDepJSON[TAG_EXDEP_TYPE].GetString());
        newScene.externalSceneDependencies.push_back(exDep);
    }

    return newScene;
}

duin::JSONValue duin::Scene::WriteJSON()
{
    // TODO add asserts and error handling

    JSONValue out;
	out.AddMember(TAG_UUID, UUID::ToStringHex(uuid));
	out.AddMember(TAG_NAME, name);
	out.AddMember(TAG_FLECSJSON, flecsJSON);
	JSONValue exScnDepsArray;
	exScnDepsArray.SetArray();
    for (const auto& exDep : externalSceneDependencies)
    {
		JSONValue exDepJSON;
		exDepJSON.AddMember(TAG_EXDEP_EXDEPUUID, UUID::ToStringHex(exDep.exdepUUID));
		exDepJSON.AddMember(TAG_EXDEP_PARENT, static_cast<int>(exDep.parent));
		exDepJSON.AddMember(TAG_EXDEP_TYPE, ResourceTypeToString(exDep.type));

		exScnDepsArray.PushBack(exDepJSON);
    }
	out.AddMember(TAG_EXT_SCN_DEPS, exScnDepsArray);

    return out;
}

/**
 * Reads an ECS scenetree from file and generates the scenetree.
 */
void duin::SceneBuilder::ReadScene(const std::string& file, ECSManager& ecs)
{
    ecs.world.from_json(file.c_str());
}

void duin::SceneBuilder::WriteScene(std::string& file, ECSManager& ecs)
{
    file = ecs.world.to_json();

    const char* json = file.c_str();
    rapidjson::Document d;
    d.Parse(json);

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    file = buffer.GetString();
}

flecs::entity duin::SceneBuilder::ReadEntity(const std::string& file, ECSManager& ecs)
{
    flecs::entity e = ecs.world.entity();
    e.from_json(file.c_str());
    
    return e;
}

void duin::SceneBuilder::WriteEntity(std::string& file, flecs::entity entity, bool recursive)
{
    if (recursive) {
        RecursiveWriter(entity, file);
    }
    else {
        file = entity.to_json();

        const char* json = file.c_str();
        rapidjson::Document d;
        d.Parse(json);

        rapidjson::StringBuffer buffer;
        rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
        d.Accept(writer);

        file = buffer.GetString();
    }
}

void duin::SceneBuilder::RecursiveWriter(flecs::entity entity, std::string& file)
{
    std::string json = static_cast<std::string>(entity.to_json());
    rapidjson::Document d;
    d.Parse(json.c_str());

    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(buffer);
    d.Accept(writer);

    file = file + buffer.GetString();

    entity.children([&](flecs::entity e) {
        file = file + ",";
        RecursiveWriter(e, file);
        }); 
}


