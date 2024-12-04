#include "EntityManager.h"
#include "Singletons.h"

EntityManager::EntityManager(std::string i_entityDefsPath)
    : entityDefsPath(i_entityDefsPath)
{}

void EntityManager::PreloadEntities()
{}

flecs::entity EntityManager::CreateEntity(std::string entityName, flecs::world *world)
{
    flecs::entity e;

    return e;
}

