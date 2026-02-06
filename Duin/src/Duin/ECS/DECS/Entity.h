#pragma once

#include <flecs.h>
#include <functional>
#include <vector>
#include <string>
#include <type_traits>

namespace duin
{
class World;
class Component;
template <typename... Comps>
class Query;

/**
 * @brief Represents an entity in the ECS world.
 */
class Entity
{
  public:
    /**
     * @brief Default constructor.
     */
    Entity();
    /**
     * @brief Destructor.
     */
    ~Entity();

    /**
     * @brief Equality operator.
     * @param other The other entity to compare.
     * @return True if entities are equal.
     */
    bool operator==(const Entity &other) const;
    /**
     * @brief Inequality operator.
     * @param other The other entity to compare.
     * @return True if entities are not equal.
     */
    bool operator!=(const Entity &other) const;
    /**
     * @brief Checks if the entity is valid.
     * @return True if valid.
     */
    explicit operator bool() const
    {
        return IsValid();
    }

    /**
     * @brief Get the world this entity belongs to.
     * @return Pointer to the World.
     */
    World *GetWorld() const;
    /**
     * @brief Check if the entity is valid.
     * @return True if valid.
     */
    bool IsValid() const;
    /**
     * @brief Check if the entity is alive.
     * @return True if alive.
     */
    bool IsAlive() const;
    /**
     * @brief Get the entity's unique ID.
     * @return The entity ID.
     */
    uint64_t GetID() const;
    /**
     * @brief Get the entity's name.
     * @return The entity name.
     */
    std::string GetName() const;
    /**
     * @brief Get the entity's path in the hierarchy.
     * @param sep Separator between hierarchy levels.
     * @param init_sep Initial separator.
     * @return The entity path.
     */
    std::string GetPath(const std::string &sep = "::", const std::string &init_sep = "::") const;

    /**
     * @brief Set the entity's name.
     * @param name The new name.
     * @return Reference to this entity.
     */
    Entity &SetName(const std::string &name);
    /**
     * @brief Enable the entity.
     * @return Reference to this entity.
     */
    Entity &Enable();
    /**
     * @brief Disable the entity.
     * @return Reference to this entity.
     */
    Entity &Disable();
    /**
     * @brief Destruct the entity.
     * @return Reference to this entity.
     */
    Entity &Destruct();
    /**
     * @brief Clear all components and data from the entity.
     * @return Reference to this entity.
     */
    Entity &Clear();
    /**
     * @brief Clone the entity.
     * @param clone_value Whether to clone component values.
     * @return The cloned entity.
     */
    Entity Clone(bool clone_value = true) const;

    /**
     * @brief Set this entity as a child of another entity.
     * @param parent The parent entity.
     * @return Reference to this entity.
     */
    Entity &ChildOf(const Entity &parent);
    /**
     * @brief Set this entity as an instance of another entity.
     * @param second The entity to inherit from.
     * @return Reference to this entity.
     */
    Entity &IsA(const Entity &second);
    /**
     * @brief Get the parent entity.
     * @return The parent entity.
     */
    Entity GetParent() const;
    /**
     * @brief Get all children of this entity.
     * @return Vector of child entities.
     */
    std::vector<Entity> GetChildren() const;
    /**
     * @brief Iterate over each child entity.
     * @tparam Func The function type.
     * @param func The function to apply to each child.
     */
    template <typename Func>
    void EachChild(Func &&func) const
    {
        flecsEntity.children([&](flecs::entity child) {
            Entity childEntity;
            childEntity.SetWorld(world);
            childEntity.SetFlecsEntity(child);
            func(childEntity);
        });
    }

    // ========== UNIFIED COMPONENT API ==========

    /**
     * @brief Check if the entity has a component.
     * @tparam T The component type.
     * @return True if the entity has the component.
     */
    template <typename T>
    bool Has() const
    {
        return flecsEntity.has<T>();
    }

    /**
     * @brief Check if the entity has all specified components.
     * @tparam Components The component types.
     * @return True if the entity has all components.
     */
    template <typename... Components>
    bool HasAll() const
    {
        return (flecsEntity.has<Components>() && ...);
    }

    /**
     * @brief Check if the entity has any of the specified components.
     * @tparam Components The component types.
     * @return True if the entity has any component.
     */
    template <typename... Components>
    bool HasAny() const
    {
        return (flecsEntity.has<Components>() || ...);
    }

    /**
     * @brief Add a component to the entity.
     * @tparam T The component type.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &Add()
    {
        flecsEntity.add<T>();
        return *this;
    }

    /**
     * @brief Add a component with constructor arguments.
     * @tparam T The component type.
     * @tparam Args Constructor argument types.
     * @param args Arguments to construct the component.
     * @return Reference to this entity.
     */
    template <typename T, typename... Args>
    Entity &Add(Args &&...args)
    {
        if constexpr (sizeof...(args) == 0)
        {
            flecsEntity.add<T>();
        }
        else
        {
            flecsEntity.set<T>(T{std::forward<Args>(args)...});
        }
        return *this;
    }

    /**
     * @brief Conditionally add or remove a component.
     * @tparam T The component type.
     * @param condition If true, add; else, remove.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &AddIf(bool condition)
    {
        if (condition)
        {
            Add<T>();
        }
        else
        {
            Remove<T>();
        }
        return *this;
    }

    /**
     * @brief Remove a component from the entity.
     * @tparam T The component type.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &Remove()
    {
        flecsEntity.remove<T>();
        return *this;
    }

    // ========== UNIFIED SET API ==========
    /**
     * @brief Set the value of a component (adds if doesn't exist).
     * @tparam T The component type.
     * @param value The value to set.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &Set(const T &value)
    {
        flecsEntity.set<T>(value);
        return *this;
    }

    /**
     * @brief Set the value of a component (rvalue overload).
     * @tparam T The component type.
     * @param value The value to set.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &Set(T &&value)
    {
        flecsEntity.set<T>(std::forward<T>(value));
        return *this;
    }

    /**
     * @brief Set a component with constructor arguments.
     * @tparam T The component type.
     * @tparam Args Constructor argument types.
     * @param args Arguments to construct the component.
     * @return Reference to this entity.
     */
    template <typename T, typename... Args>
    Entity &Set(Args &&...args)
    {
        if constexpr (sizeof...(args) == 1 &&
                      std::is_same_v<T, std::decay_t<std::tuple_element_t<0, std::tuple<Args...>>>>)
        {
            // Single argument of same type as T - use direct set
            flecsEntity.set<T>(std::forward<Args>(args)...);
        }
        else
        {
            // Multiple arguments - construct T with them
            flecsEntity.set<T>(T{std::forward<Args>(args)...});
        }
        return *this;
    }

    /**
     * @brief Set multiple components at once.
     * @tparam T1 First component type.
     * @tparam T2 Second component type.
     * @tparam Rest Remaining component types.
     * @param val1 First value.
     * @param val2 Second value.
     * @param rest Remaining values.
     * @return Reference to this entity.
     */
    template <typename T1, typename T2, typename... Rest>
    Entity &Set(const T1 &val1, const T2 &val2, const Rest &...rest)
    {
        Set<T1>(val1);
        Set<T2>(val2);
        if constexpr (sizeof...(rest) > 0)
        {
            SetMultipleInternal(rest...);
        }
        return *this;
    }

    // ========== UNIFIED GET API ==========
    /**
     * @brief Get a const reference to a component.
     * @tparam T The component type.
     * @return Const reference to the component.
     */
    template <typename T>
    const T &Get() const
    {
        return flecsEntity.get<T>();
    }

    const void* Get(Entity e)
    {
        return flecsEntity.get(e.GetID());
    }



    /**
     * @brief Try to get a const pointer to a component.
     * @tparam T The component type.
     * @return Pointer to the component, or nullptr if not present.
     */
    template <typename T>
    const T *TryGet() const
    {
        return flecsEntity.try_get<T>();
    }

    /**
     * @brief Get a mutable reference to a component.
     * @tparam T The component type.
     * @return Mutable reference to the component.
     */
    template <typename T>
    T &GetMut()
    {
        return flecsEntity.get_mut<T>();
    }

    /**
     * @brief Try to get a mutable pointer to a component.
     * @tparam T The component type.
     * @return Pointer to the component, or nullptr if not present.
     */
    template <typename T>
    T *TryGetMut()
    {
        return flecsEntity.try_get_mut<T>();
    }

    /**
     * @brief Ensure a component exists, creating if needed.
     * @tparam T The component type.
     * @tparam Args Constructor argument types.
     * @param args Arguments to construct the component.
     * @return Reference to the component.
     */
    template <typename T>
    T &Ensure(T value)
    {
        flecsEntity.set<T>(std::move(value));
        return flecsEntity.get_mut<T>();
    }

    void IterateEachComponent()
    {
    }

    // ========== UNIFIED PAIR API ==========
    /**
     * @brief Check if the entity has a pair of components.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return True if the entity has the pair.
     */
    template <typename First, typename Second>
    bool HasPair() const
    {
        return flecsEntity.has<First, Second>();
    }

    /**
     * @brief Add a pair of components to the entity.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &AddPair()
    {
        flecsEntity.add<First, Second>();
        return *this;
    }

    /**
     * @brief Remove a pair of components from the entity.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &RemovePair()
    {
        flecsEntity.remove<First, Second>();
        return *this;
    }

    /**
     * @brief Set the value of a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @param value Value to set.
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &SetPair(const Second &value)
    {
        flecsEntity.set<First, Second>(value);
        return *this;
    }

    /**
     * @brief Set the value of a pair component (rvalue overload).
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @param value Value to set.
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &SetPair(Second &&value)
    {
        flecsEntity.set<First, Second>(std::forward<Second>(value));
        return *this;
    }

    /**
     * @brief Get a const reference to a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Const reference to the pair component.
     */
    template <typename First, typename Second>
    const Second &GetPair() const
    {
        return flecsEntity.get<First, Second>();
    }

    /**
     * @brief Try to get a const pointer to a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Pointer to the pair component, or nullptr if not present.
     */
    template <typename First, typename Second>
    const Second *TryGetPair() const
    {
        return flecsEntity.try_get<First, Second>();
    }

    /**
     * @brief Get a mutable reference to a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Mutable reference to the pair component.
     */
    template <typename First, typename Second>
    Second &GetPairMut()
    {
        return flecsEntity.get_mut<First, Second>();
    }

    /**
     * @brief Add a pair of components by entity IDs.
     * @param first First component ID.
     * @param second Second component ID.
     * @return Reference to this entity.
     */
    Entity &AddPair(uint64_t first, uint64_t second)
    {
        flecsEntity.add(first, second);
        return *this;
    }

    /**
     * @brief Remove a pair of components by entity IDs.
     * @param first First component ID.
     * @param second Second component ID.
     * @return Reference to this entity.
     */
    Entity &RemovePair(uint64_t first, uint64_t second)
    {
        flecsEntity.remove(first, second);
        return *this;
    }

    /**
     * @brief Check if the entity has a pair by entity IDs.
     * @param first First component ID.
     * @param second Second component ID.
     * @return True if the entity has the pair.
     */
    bool HasPair(uint64_t first, uint64_t second) const
    {
        return flecsEntity.has(first, second);
    }

    // ========== ADVANCED OPERATIONS ==========
    /**
     * @brief In-place construction of a component.
     * @tparam T The component type.
     * @tparam Args Constructor argument types.
     * @param args Arguments to construct the component.
     * @return Reference to this entity.
     */
    template <typename T, typename... Args>
    Entity &Emplace(Args &&...args)
    {
        flecsEntity.emplace<T>(std::forward<Args>(args)...);
        return *this;
    }

    /**
     * @brief Assign a value to an existing component.
     * @tparam T The component type.
     * @param value Value to assign.
     * @return Reference to this entity.
     * @throws std::runtime_error if the component does not exist.
     */
    template <typename T>
    Entity &Assign(const T &value)
    {
        // Assign only works if component already exists
        // if (!Has<T>()) {
        //	throw std::runtime_error("Cannot assign component that doesn't exist. Use Set() instead.");
        //}
        flecsEntity.set<T>(value);
        return *this;
    }

    /**
     * @brief Assign a value to an existing component (rvalue overload).
     * @tparam T The component type.
     * @param value Value to assign.
     * @return Reference to this entity.
     * @throws std::runtime_error if the component does not exist.
     */
    template <typename T>
    Entity &Assign(T &&value)
    {
        // if (!Has<T>()) {
        //	throw std::runtime_error("Cannot assign component that doesn't exist. Use Set() instead.");
        // }
        flecsEntity.set<T>(std::forward<T>(value));
        return *this;
    }

    /**
     * @brief Modify multiple components in one call.
     * @tparam Func The function type.
     * @param func The function to apply.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &Modify(const Func &func)
    {
        assert(false);
        // flecsEntity.insert(func);
        return *this;
    }

    /**
     * @brief Enable a specific component.
     * @tparam T The component type.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &EnableComponent()
    {
        flecsEntity.enable<T>();
        return *this;
    }

    /**
     * @brief Disable a specific component.
     * @tparam T The component type.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &DisableComponent()
    {
        flecsEntity.disable<T>();
        return *this;
    }

    /**
     * @brief Check if a specific component is enabled.
     * @tparam T The component type.
     * @return True if enabled.
     */
    template <typename T>
    bool IsComponentEnabled() const
    {
        return flecsEntity.enabled<T>();
    }

    // ========== UTILITY OPERATIONS ==========

    /**
     * @brief Set a component value with auto-override (for inheritance).
     * @tparam T The component type.
     * @param value Value to set.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &SetAutoOverride(const T &value)
    {
        flecsEntity.set_auto_override<T>(value);
        return *this;
    }

    /**
     * @brief Set a component value with auto-override (rvalue overload).
     * @tparam T The component type.
     * @param value Value to set.
     * @return Reference to this entity.
     */
    template <typename T>
    Entity &SetAutoOverride(T &&value)
    {
        flecsEntity.set_auto_override<T>(std::forward<T>(value));
        return *this;
    }

    /**
     * @brief Execute a function within the scope of this entity.
     * @tparam Func The function type.
     * @param func The function to execute.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &WithScope(Func &&func) const
    {
        // flecsEntity.scope(std::forward<Func>(func));
        assert(false);
        return const_cast<Entity &>(*this);
    }

    /**
     * @brief Execute a function with a specific component.
     * @tparam Func The function type.
     * @param func The function to execute.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &WithComponent(Func &&func) const
    {
        // flecsEntity.with(std::forward<Func>(func));
        assert(false);
        return const_cast<Entity &>(*this);
    }

    /**
     * @brief Emit an event from this entity.
     * @tparam Event The event type.
     * @tparam Args Event argument types.
     * @param args Arguments for the event.
     */
    template <typename Event, typename... Args>
    void Emit(Args &&...args) const
    {
        /*flecsEntity.emit<Event>(std::forward<Args>(args)...);*/
    }

    /**
     * @brief Mark a component as modified for change detection.
     * @tparam T The component type.
     */
    template <typename T>
    void Modified() const
    {
        flecsEntity.modified<T>();
    }

    /**
     * @brief Mark the entire entity as modified.
     */
    void Modified() const;

    // ========== QUERY HELPERS ==========
    /**
     * @brief For each child entity with specified components, execute a function.
     * @tparam Components The component types.
     * @tparam Func The function type.
     * @param func The function to execute.
     */
    template <typename... Components, typename Func>
    void ForEachChild(Func &&func) const
    {
        flecsEntity.children([&](flecs::entity child) {
            if ((child.has<Components>() && ...))
            {
                Entity childEntity;
                childEntity.SetWorld(world);
                childEntity.SetFlecsEntity(child);
                func(childEntity, child.get<Components>()...);
            }
        });
    }

    /**
     * @brief For each target entity of a relationship, execute a function.
     * @tparam Relationship The relationship type.
     * @tparam Func The function type.
     * @param func The function to execute.
     */
    template <typename Relationship, typename Func>
    void ForEachTarget(Func &&func) const
    {
        // flecsEntity.each<Relationship>([&](flecs::entity target) {
        //	Entity targetEntity;
        //	targetEntity.SetWorld(world);
        //	targetEntity.SetFlecsEntity(target);
        //	func(targetEntity);
        // });
        assert(false);
    }

    template <typename Func>
    void ForEachComponent(Func &&func) const
    {
        flecsEntity.each([&](flecs::id id) {
            // id represents the component type
            // You can get type information and invoke the callback
            flecs::entity fe = id.entity();
            Entity e(fe, world);

            func(e);
        });
    }

    /**
     * @brief Get the target entity for a relationship.
     * @param relationship The relationship ID.
     * @param index The index (default 0).
     * @return The target entity.
     */
    Entity GetTarget(uint64_t relationship, int32_t index = 0) const;

    flecs::entity GetFlecsEntity() const;

  private:
    friend class World;
    template <typename... Components>
    friend class Query;

    World *world;              ///< Pointer to the world this entity belongs to (non-owning).
    flecs::entity flecsEntity; ///< Internal flecs entity handle.

    /**
     * @brief Construct an entity from a flecs entity.
     * @param entity The flecs entity.
     */
    Entity(const flecs::entity &entity);

    /**
     * @brief Construct an entity from a flecs entity, and a world.
     * @param entity The flecs entity.
     * @param world The World.
     */
    Entity(const flecs::entity &entity, World *world);

    /**
     * @brief Set the world pointer for this entity.
     * @param world The world pointer.
     */
    void SetWorld(World *world);

    /**
     * @brief Set the internal flecs entity handle.
     * @param entity The flecs entity.
     */
    void SetFlecsEntity(const flecs::entity &entity);

    /**
     * @brief Internal helper for recursive Set operations.
     * @tparam T The component type.
     * @param value The value to set.
     */
    template <typename T>
    void SetInternal(const T &value)
    {
        Set<T>(value);
    }

    /**
     * @brief Internal helper for recursive Set operations (rvalue overload).
     * @tparam T The component type.
     * @param value The value to set.
     */
    template <typename T>
    void SetInternal(T &&value)
    {
        Set<T>(std::forward<T>(value));
    }

    /**
     * @brief Internal helper for setting multiple components recursively.
     * @tparam T The first component type.
     * @tparam Rest Remaining component types.
     * @param first First value.
     * @param rest Remaining values.
     */
    template <typename T, typename... Rest>
    void SetMultipleInternal(const T &first, const Rest &...rest)
    {
        Set<T>(first);
        if constexpr (sizeof...(rest) > 0)
        {
            SetMultipleInternal(rest...);
        }
    }
};
} // namespace duin
