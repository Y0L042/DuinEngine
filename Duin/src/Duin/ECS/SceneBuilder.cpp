#include "dnpch.h"
#include "SceneBuilder.h"
#include "ECSManager.h"
#include "Duin/IO/DataValue.h"
#include <flecs.h>

#define PRETTY_WRITE_JSON


#ifdef PRETTY_WRITE_JSON
#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#endif



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
