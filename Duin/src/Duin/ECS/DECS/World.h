#pragma once

#include <flecs.h>
#include "Query.h"

namespace duin
{
class Entity;
class World
{
  public:
    /**
     * @brief Construct a new World object.
     */
    World();
    /**
     * @brief Destroy the World object.
     */
    ~World();

    /**
     * @brief Create a new entity in the world.
     * @param name Optional name for the entity.
     * @return The created Entity object.
     */
    Entity CreateEntity(const std::string &name);
    /**
     * @brief Delete an entity by its ID.
     * @param id The ID of the entity to delete.
     */
    void DeleteEntity(uint64_t id);
    /**
     * @brief Delete an entity by reference.
     * @param entity The entity to delete.
     */
    void DeleteEntity(const Entity &entity);

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
    Entity MakeAlive(uint64_t id);

    /**
     * @brief Release the world resources.
     */
    void Release();
    /**
     * @brief Quit and clean up the world.
     */
    void Quit();

    /**
     * @brief Iterate over the children of the root entity.
     * @tparam Func The function type.
     * @param f The function to apply to each child.
     */
    template <typename Func>
    void IterateChildren(Func &&f) const
    {
        flecsWorld.children([&](flecs::entity child) {
            Entity e;
            e.SetWorld(const_cast<World *>(this));
            e.SetFlecsEntity(child);
            f(e);
        });
    }

    /**
     * @brief Register a component type with the world.
     * @tparam T The component type.
     */
    template <typename T>
    void Component()
    {
        flecsWorld.component<T>();
    }

    template <typename... Comps>
    Query<Comps...> QueryBuilder() const
    {
        Query<Comps...> q(flecsWorld.query_builder<Comps...>());
        return q;
    }

    flecs::world GetFlecsWorld();

  private:
    friend class Entity;
    flecs::world flecsWorld;

    // Prevent copying
    World(const World &) = delete;
    World &operator=(const World &) = delete;
};
} // namespace duin