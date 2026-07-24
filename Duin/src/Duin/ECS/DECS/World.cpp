#include "dnpch.h"
#include "DECS.h"
#include "Entity.h"
#include "Query.h"

duin::World::World()
{
}

duin::World::World(flecs::world &&w) : flecsWorld(std::move(w)) {};

duin::World::World(ecs_world_t *world) : flecsWorld(world) {};

duin::World::~World() {};

duin::System duin::World::System(const duin::Entity &e) const
{
    // Adopt the existing system entity; does NOT create a new system.
    // The entity already carries its own world.
    return duin::System(e);
}

duin::Entity duin::World::Entity(const std::string &name)
{
    if (!flecsWorld)
    {
        DN_CORE_FATAL("World is uninitialized!");
        return duin::Entity();
    }
    // The flecs entity carries its own world; no back-pointer needed.
    return duin::Entity(flecsWorld.entity(name.c_str()));
}

duin::Entity duin::World::Entity(uint64_t id)
{
    return duin::Entity(flecsWorld.entity(id));
}

duin::Entity duin::World::Prefab(const std::string &name)
{
    return duin::Entity(flecsWorld.prefab(name.c_str()));
}

void duin::World::DeleteEntity(uint64_t id)
{
    flecsWorld.entity(id).destruct();
}

void duin::World::DeleteEntity(const duin::Entity &entity)
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
    return duin::Entity(flecsWorld.make_alive(id));
}

std::string duin::World::ExportRegisteredComponentMeta()
{
    return std::string();
}

duin::Entity duin::World::GetWorldEntity()
{
    return duin::Entity(flecsWorld.entity());
}

void duin::World::Release()
{
    flecsWorld.release();
}

void duin::World::Quit()
{
    flecsWorld.quit();
}

duin::Entity duin::World::Lookup(
    const std::string &name, const std::string &sep, const std::string &root_sep, bool recursive)
{
    return duin::Entity(flecsWorld.lookup(name.c_str(), sep.c_str(), root_sep.c_str(), recursive));
}

std::vector<duin::Entity> duin::World::GetChildren(bool filterBuiltins)
{
    std::vector<duin::Entity> children;

    flecsWorld.children([&](flecs::entity child) {
        if (filterBuiltins)
        {
            // Filter out known FLECS internal entity categories:
            //   flecs::Component  (struct type)  � component/tag type descriptor entities
            //   flecs::Observer   (entity_t tag) � internal observer entities
            //   flecs::Module     (entity_t tag) � module scope entities
            if (child.has<flecs::Component>())
                return;
            if (child.has(flecs::Observer))
                return;
            if (child.has(flecs::Module))
                return;
        }
        // child already carries its own world.
        children.push_back(duin::Entity(child));
    });

    return children;
}

flecs::world &duin::World::GetFlecsWorld()
{
    return flecsWorld;
}

const flecs::world &duin::World::GetFlecsWorld() const
{
    return flecsWorld;
}
