#pragma once

#include <Duin.h>

class EntityManager
{
public:
    EntityManager(std::string i_entityDefsPath);
    ~EntityManager() = default;

    void PreloadEntities();
    flecs::entity CreateEntity(std::string entityName, flecs::world *world);

private:
    std::string entityDefsPath;
};
