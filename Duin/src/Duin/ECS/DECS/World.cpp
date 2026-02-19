#include "dnpch.h"
#include "DECS.h"
#include "Entity.h"
#include "Query.h"

duin::World::World() 
{
}

duin::World::World(flecs::world w)
    : flecsWorld(w) {

      };

duin::World::~World() {};

duin::Entity duin::World::CreateEntity(const std::string &name)
{
    Entity e;
    flecs::entity flecsEntity = flecsWorld.entity(name.c_str());
    e.SetWorld(this);
    e.SetFlecsEntity(flecsEntity);
    return e;
}

duin::Entity duin::World::CreatePrefab(const std::string &name)
{
    Entity e;
    flecs::entity flecsEntity = flecsWorld.prefab(name.c_str());
    e.SetWorld(this);
    e.SetFlecsEntity(flecsEntity);
    return e;
}

void duin::World::DeleteEntity(uint64_t id)
{
    flecsWorld.entity(id).destruct();
}

void duin::World::DeleteEntity(const Entity &entity)
{
    flecsWorld.entity(entity.GetID()).destruct();
}

void duin::World::DeleteWith(uint64_t id)
{
    flecsWorld.delete_with(id);
}

void duin::World::DeleteWith(uint64_t first, uint64_t second)
{
    flecsWorld.delete_with(first, second);
}

void duin::World::RemoveAll(uint64_t id)
{
    flecsWorld.remove_all(id);
}

void duin::World::RemoveAll(uint64_t first, uint64_t second)
{
    flecsWorld.remove_all(first, second);
}

void duin::World::DeferBegin()
{
    flecsWorld.defer_begin();
}

void duin::World::DeferEnd()
{
    flecsWorld.defer_end();
}

bool duin::World::IsDeferred() const
{
    return flecsWorld.is_deferred();
}

bool duin::World::Progress(float deltaTime)
{
    return flecsWorld.progress(deltaTime);
}

bool duin::World::IsAlive(uint64_t id) const
{
    return flecsWorld.is_alive(id);
}

bool duin::World::IsValid(uint64_t id) const
{
    return flecsWorld.is_valid(id);
}

void duin::World::SetVersion(uint64_t id)
{
    flecsWorld.set_version(id);
}

duin::Entity duin::World::MakeAlive(uint64_t id)
{
    Entity e;
    flecs::entity flecsEntity = flecsWorld.make_alive(id);
    e.SetWorld(this);
    e.SetFlecsEntity(flecsEntity);
    return e;
}

duin::Entity duin::World::GetWorldEntity()
{
    Entity e;
    e.SetWorld(this);
    e.SetFlecsEntity(flecsWorld.entity());

    return e;
}

void duin::World::Release()
{
    flecsWorld.release();
}

void duin::World::Quit()
{
    flecsWorld.quit();
}

duin::Entity duin::World::Lookup(const std::string &name)
{
    Entity e;
    e.SetWorld(this);
    e.SetFlecsEntity(flecsWorld.lookup(name.c_str()));

    return e;
}

flecs::world &duin::World::GetFlecsWorld()
{
    return flecsWorld;
}
