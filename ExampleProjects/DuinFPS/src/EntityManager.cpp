#include "EntityManager.h"
#include "Singletons.h"

EntityManager::EntityManager(std::string i_entityDefsPath) : entityDefsPath(i_entityDefsPath)
{
}

void EntityManager::PreloadEntities()
{
}

duin::Entity EntityManager::CreateEntity(std::string entityName, duin::World *world)
{
    duin::Entity e;

    return e;
}
