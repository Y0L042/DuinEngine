#include "EntityManager.h"
#include "Singletons.h"

EntityManager::EntityManager(std::string i_entityDefsPath)
    : entityDefsPath(i_entityDefsPath)
{}

void EntityManager::PreloadEntities()
{}

flecs::entity EntityManager::CreateEntity(std::string entityName, flecs::world *world)
{
    rapidjson::Document document;
    ReadJSONFile(entityDefsPath.c_str(), &document);
    
    assert(document.HasMember("entity_defs"));
    //const char* members = document["entity_defs"];
    debugConsole.LogEx(duin::LogLevel::Info, "Entity: ");

    flecs::entity e;
    return e;
}

