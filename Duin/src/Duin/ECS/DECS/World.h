#pragma once

#include <flecs.h>
#include "../ComponentSerializer.h"
#include "Query.h"
#include <cstdint>
#include <string>
#include <vector>
#include <flecs/addons/cpp/entity.hpp>
#include <flecs/addons/cpp/world.hpp>

namespace duin
{
class Entity;
class System;
template <typename... Components>
class QueryBuilder;
template <typename... Components>
class SystemBuilder;

class World
{
  public:
    /**
     * @brief Construct a new World object.
     */
    World();

    World(flecs::world &&w);

    World(ecs_world_t *world);

    /**
     * @brief Destroy the World object.
     */
    ~World();

    /**
     * @brief Create a new entity in the world.
     * @param name Optional name for the entity.
     * @return The created Entity object.
     */
    duin::Entity Entity(const std::string &name = "");

    duin::Entity Entity(uint64_t id);

    /**
     * @brief Create a new prefab entity in the world.
     * @param name Optional name for the prefab.
     * @return The created prefab Entity object.
     */
    duin::Entity Prefab(const std::string &name = "");
    /**
     * @brief Delete an entity by its ID.
     * @param id The ID of the entity to delete.
     */
    void DeleteEntity(uint64_t id);
    /**
     * @brief Delete an entity by reference.
     * @param entity The entity to delete.
     */
    void DeleteEntity(const duin::Entity &entity);

    /**
     * @brief Remove all entities with a given component or pair.
     * @tparam T The component type.
     */
    template <typename T>
    void DeleteWith()
    {
        flecsWorld.delete_with<T>();
    }
    /**
     * @brief Remove all entities with a given component by ID.
     * @param id The component ID.
     */
    void DeleteWith(uint64_t id);
    /**
     * @brief Remove all entities with a given pair of components.
     * @param first The first component ID.
     * @param second The second component ID.
     */
    void DeleteWith(uint64_t first, uint64_t second);

    /**
     * @brief Remove all instances of a component or pair from all entities.
     * @tparam T The component type.
     */
    template <typename T>
    void RemoveAll()
    {
        flecsWorld.remove_all<T>();
    }
    /**
     * @brief Remove all instances of a component by ID from all entities.
     * @param id The component ID.
     */
    void RemoveAll(uint64_t id);
    /**
     * @brief Remove all instances of a pair of components from all entities.
     * @param first The first component ID.
     * @param second The second component ID.
     */
    void RemoveAll(uint64_t first, uint64_t second);

    /**
     * @brief Begin deferred execution of commands.
     */
    void DeferBegin();
    /**
     * @brief End deferred execution of commands.
     */
    void DeferEnd();
    /**
     * @brief Check if execution is currently deferred.
     * @return True if deferred, false otherwise.
     */
    bool IsDeferred() const;
    /**
     * @brief Defer a function for later execution.
     * @tparam Func The function type.
     * @param func The function to defer.
     */
    template <typename Func>
    void Defer(Func &&func)
    {
        flecsWorld.defer(std::forward<Func>(func));
    }

    /**
     * @brief Progress the world simulation by a time step.
     * @param deltaTime The time step to progress by.
     * @return True if progress was successful, false otherwise.
     */
    bool Progress(float deltaTime = 0.0f);

    /**
     * @brief Check if an entity is alive.
     * @param id The entity ID.
     * @return True if alive, false otherwise.
     */
    bool IsAlive(uint64_t id) const;
    /**
     * @brief Check if an entity is valid.
     * @param id The entity ID.
     * @return True if valid, false otherwise.
     */
    bool IsValid(uint64_t id) const;

    /**
     * @brief Set the version for an entity.
     * @param id The entity ID.
     */
    void SetVersion(uint64_t id);

    /**
     * @brief Make an entity alive, regenerating if needed.
     * @param id The entity ID.
     * @return The alive Entity object.
     */
    duin::Entity MakeAlive(uint64_t id);

    /**
     * @brief Release the world resources.
     */
    void Release();
    /**
     * @brief Quit and clean up the world.
     */
    void Quit();

    duin::Entity Lookup(const std::string &name,
                        const std::string &sep = "::", const std::string &root_sep = "::", bool recursive = true);

    /**
     * @brief Iterate over the children of the root entity.
     * @tparam Func The function type.
     * @param f The function to apply to each child.
     */
    template <typename Func>
    void IterateChildren(Func &&f, bool filterBuiltins = true) const
    {
        flecsWorld.children([&](flecs::entity child) {
            if (filterBuiltins)
            {
                // Filter out known FLECS internal entity categories:
                //   flecs::Component  (struct type)   component/tag type descriptor entities
                //   flecs::Observer   (entity_t tag)  internal observer entities
                //   flecs::Module     (entity_t tag)  module scope entities
                if (child.has<flecs::Component>())
                    return;
                if (child.has(flecs::Observer))
                    return;
                if (child.has(flecs::Module))
                    return;
            }
            // child already carries its own world.
            duin::Entity e(child);
            f(e);
        });
    }

    std::vector<duin::Entity> GetChildren(bool filterBuiltins = true);

    template <typename Func>
    void Each(Func &&func) const
    {
        flecsWorld.each(std::forward<Func>(func));
    }

    /**
     * @brief Register a component type with the world.
     * @tparam T The component type.
     */
    template <typename T>
    duin::Entity Component()
    {
        flecs::entity typeEntity = flecsWorld.component<T>();

        // typeEntity carries its own world.
        duin::Entity e(typeEntity);

        auto &si = ComponentSerializer::Get();
        si.RegisterComponent<T>(flecsWorld, typeEntity);

        return e;
    }

    template <typename... Comps>
    duin::QueryBuilder<Comps...> QueryBuilder() const
    {
        return duin::QueryBuilder<Comps...>(flecsWorld.query_builder<Comps...>());
    }

    template <typename... Comps>
    duin::SystemBuilder<Comps...> System(const char *name = nullptr) const
    {
        // Pass the raw flecs world + name so SystemBuilder constructs the flecs
        // system_builder in place (heap-held). It must NOT be constructed here
        // and moved in: flecs::system_builder has no move ctor that re-points
        // its internal desc_/term_ pointers, so a move would dangle them.
        return duin::SystemBuilder<Comps...>(flecsWorld.c_ptr(), name);
    }

    /**
     * @brief Adopt an existing system entity as a runnable System.
     *
     * Mirrors flecs::world::system(flecs::entity): does NOT create a new system,
     * it wraps the existing system entity. Defined out-of-line in World.cpp
     * because it returns the complete duin::System type.
     */
    duin::System System(const duin::Entity &e) const;



    template <typename T>
    duin::Entity::ID IDIfRegistered()
    {
        flecs::id_t f_id = flecsWorld.id_if_registered<T>();
        return duin::Entity::ID(flecsWorld, f_id);
    }

    std::string ExportRegisteredComponentMeta();

    /**
     * @brief Get the world as an entity.
     * @return Entity representing the world itself.
     */
    duin::Entity GetWorldEntity();

    flecs::world &GetFlecsWorld();
    const flecs::world &GetFlecsWorld() const;

  private:
    friend class Entity;
    flecs::world flecsWorld;

  public:
    // World is a value type: flecs::world is a refcounted, non-owning handle to
    // the underlying ecs_world_t*, so copying/moving a World is cheap and safe
    // (it never prematurely calls ecs_fini). Many Worlds may view the same world.
    World(const World &) = default;
    World &operator=(const World &) = default;
    World(World &&) = default;
    World &operator=(World &&) = default;
};
} // namespace duin
