#pragma once

#include <Duin.h>

class EntityManager
{
  public:
    EntityManager(std::string i_entityDefsPath);
    ~EntityManager() = default;

    void PreloadEntities();
    duin::Entity CreateEntity(std::string entityName, duin::World *world);

  private:
    std::string entityDefsPath;
};
