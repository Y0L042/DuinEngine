#pragma once

#include <flecs.h>
#include <functional>
#include <vector>
#include <string>
#include <type_traits>
#include "Duin/Core/Debug/DNLog.h"

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
     * @brief Wrapper around flecs::id_t that provides entity ID manipulation.
     * A flecs id is an identifier that can be added to entities. Ids can be:
     * - entities (including components, tags)
     * - pair ids
     * - entities with id flags set (like flecs::AUTO_OVERRIDE, flecs::TOGGLE)
     */
    struct ID
    {
        /**
         * @brief Default constructor.
         */
        ID();

        ID(flecs::id id, World *world) : flecsId_(id), world_(world)
        {
        }

        /**
         * @brief Construct from raw ID value.
         * @param value The raw ID value.
         */
        explicit ID(flecs::id_t value);

        /**
         * @brief Construct from world and ID value.
         * @param world Pointer to the world.
         * @param value The raw ID value.
         */
        explicit ID(World *world, flecs::id_t value = 0);

        /**
         * @brief Construct a pair ID from two IDs.
         * @param world Pointer to the world.
         * @param first First ID.
         * @param second Second ID.
         */
        explicit ID(World *world, flecs::id_t first, flecs::id_t second);

        /**
         * @brief Construct from world and string expression.
         * @param world Pointer to the world.
         * @param expr String expression.
         */
        explicit ID(World *world, const char *expr);

        /**
         * @brief Construct a pair ID from two raw IDs.
         * @param first First ID.
         * @param second Second ID.
         */
        explicit ID(flecs::id_t first, flecs::id_t second);

        /**
         * @brief Construct a pair ID from two Entity::ID objects.
         * @param first First ID.
         * @param second Second ID.
         */
        explicit ID(const ID &first, const ID &second);

        /**
         * @brief Test if ID is a pair (has first, second).
         * @return True if the ID is a pair.
         */
        bool IsPair() const;

        /**
         * @brief Test if ID is a wildcard.
         * @return True if the ID is a wildcard.
         */
        bool IsWildcard() const;

        /**
         * @brief Test if ID is an entity.
         * @return True if the ID is an entity.
         */
        bool IsEntity() const;

        /**
         * @brief Return ID as entity (only allowed when ID is valid entity).
         * @return The entity.
         */
        Entity GetEntity() const;

        /**
         * @brief Return ID with flags added.
         * @param flags Flags to add.
         * @return ID with added flags.
         */
        ID AddFlags(flecs::id_t flags) const;

        /**
         * @brief Return ID with specific flags removed.
         * @param flags Flags to remove.
         * @return ID with removed flags.
         */
        ID RemoveFlags(flecs::id_t flags) const;

        /**
         * @brief Return ID without any flags.
         * @return ID without flags.
         */
        ID RemoveFlags() const;

        /**
         * @brief Return ID without generation.
         * @return ID without generation.
         */
        ID RemoveGeneration() const;

        /**
         * @brief Return component type ID.
         * @return The type ID entity.
         */
        Entity TypeId() const;

        /**
         * @brief Test if ID has specified flags.
         * @param flags Flags to test.
         * @return True if all specified flags are set.
         */
        bool HasFlags(flecs::id_t flags) const;

        /**
         * @brief Test if ID has any flags.
         * @return True if any flags are set.
         */
        bool HasFlags() const;

        /**
         * @brief Return flags set on ID.
         * @return Entity representing the flags.
         */
        Entity GetFlags() const;

        /**
         * @brief Test if ID has specified relation as first element.
         * @param first Relation to test.
         * @return True if the relation matches.
         */
        bool HasRelation(flecs::id_t first) const;

        /**
         * @brief Get first element from a pair.
         * If the ID is not a pair, this operation will fail. When the ID has a
         * world, the operation will ensure that the returned ID has the correct
         * generation count.
         * @return The first element entity.
         */
        Entity First() const;

        /**
         * @brief Get second element from a pair.
         * If the ID is not a pair, this operation will fail. When the ID has a
         * world, the operation will ensure that the returned ID has the correct
         * generation count.
         * @return The second element entity.
         */
        Entity Second() const;

        /**
         * @brief Convert ID to string.
         * @return String representation of the ID.
         */
        std::string Str() const;

        /**
         * @brief Convert flags of ID to string.
         * @return String representation of the flags.
         */
        std::string FlagsStr() const;

        /**
         * @brief Get raw flecs::id_t value.
         * @return The raw ID value.
         */
        uint64_t GetID() const;

        /**
         * @brief Implicit conversion to flecs::id_t.
         */
        operator flecs::id_t() const;

        /**
         * @brief Get the world this ID belongs to.
         * @return Pointer to the World.
         */
        World *GetWorld() const;

        /**
         * @brief Get the underlying flecs::id object.
         * @return The flecs::id object.
         */
        flecs::id GetFlecsId() const;

      private:
        friend class Entity;

        World *world_ =
            nullptr; ///< World is optional, but guarantees that entity identifiers extracted from the id are valid
        flecs::id flecsId_; ///< Internal flecs id handle.
    };

    /**
     * @brief Default constructor.
     */
    Entity();

    Entity(uint64_t id, World *world = nullptr);

    /**
     * @brief Copy constructor.
     * @param other The entity to copy from.
     */
    Entity(const Entity &other);

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
     * @brief Copy assignment operator.
     * @param other The entity to copy from.
     * @return Reference to this entity.
     */
    Entity &operator=(const Entity &other);

    /**
     * @brief Move constructor.
     * @param other The entity to move from.
     */
    Entity(Entity &&other) noexcept;

    /**
     * @brief Move assignment operator.
     * @param other The entity to move from.
     * @return Reference to this entity.
     */
    Entity &operator=(Entity &&other) noexcept;

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

    bool IsTag() const;

    bool IsPair() const;

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

    Entity Lookup(const std::string &childName, bool searchPath = false);

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

    Entity First();
    Entity Second();

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

    template <typename First, typename Func>
    Entity &Each(Func &&func)
    {
        flecsEntity.each<First>([&](flecs::id fID) {
            Entity::ID id(fID, world);
            func(id);
        });
    };

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

    template <typename First, typename Second>
    bool Has() const
    {
        return flecsEntity.has<First, Second>();
    }

    bool Has(Entity e) const
    {
        return flecsEntity.has(e.flecsEntity.raw_id());
    }

    bool Has(Entity first, Entity second)
    {
        return flecsEntity.has(first.GetID(), second.GetID());
    }

    bool Has(Entity::ID id) const
    {
        return flecsEntity.has(id.GetID());
    }

    template <typename First>
    bool Has(Entity second) const
    {
        return flecsEntity.has<First>(second.GetID());
    }

    template <typename First>
    bool Has(Entity::ID second) const
    {
        return flecsEntity.has<First>(second.GetID());
    }

    template <typename Second>
    bool HasSecond(Entity first) const
    {
        return flecsEntity.has_second<Second>(first.GetID());
    }

    template <typename Second>
    bool HasSecond(Entity::ID first) const
    {
        return flecsEntity.has_second<Second>(first.GetID());
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

    template <typename T>
    bool Owns() const
    {
        return (flecsEntity.owns<T>());
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

    Entity &Add(const Entity& relationship, const Entity& target);

    /**
     * @brief Add an entity/tag/trait to the entity by ID.
     * This is typically used for adding tags, traits, or relationships.
     * @param component The component/tag/trait ID to add.
     * @return Reference to this entity.
     */
    Entity &Add(flecs::id_t component)
    {
        flecsEntity.add(component);
        return *this;
    }

    // Helper trait to check if Args is a single Entity parameter
    template <typename... Args>
    struct IsSingleEntityArg : std::false_type
    {
    };

    template <typename T>
    struct IsSingleEntityArg<T> : std::is_same<typename std::decay<T>::type, Entity>
    {
    };

    /**
     * @brief Add a component with constructor arguments.
     * @tparam T The component type.
     * @tparam Args Constructor argument types.
     * @param args Arguments to construct the component.
     * @return Reference to this entity.
     *
     * Note: This overload is disabled when a single Entity argument is passed
     * to avoid ambiguity with the relationship pair overload.
     */
    template <typename T, typename... Args,
              typename = typename std::enable_if<!IsSingleEntityArg<Args...>::value>::type>
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
     * @brief Add a relationship pair to the entity.
     * This creates a (Relation, target) pair where Relation is a component type
     * and target is another entity. This is commonly used for relationships like
     * Likes, Targets, Owns, etc.
     *
     * Example:
     * @code
     * Entity player = world.CreateEntity("Player");
     * Entity enemy = world.CreateEntity("Enemy");
     * player.Add<Targets>(enemy);  // Player targets enemy
     * @endcode
     *
     * @tparam Relation The relationship component type (first element of the pair).
     * @param target The target entity (second element of the pair).
     * @return Reference to this entity.
     */
    template <typename Relation>
    Entity &Add(const Entity &target)
    {
        flecsEntity.add<Relation>(target.flecsEntity.raw_id());
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

    // Entity &Set(id, void* data);

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

    const void *Get(Entity e)
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

    // ========== PAIR STORAGE TYPE HELPER ==========
  private:
    /**
     * @brief Helper to determine storage type for pairs (Flecs rules).
     *
     * Storage type is derived using the following rules:
     * - If pair::first is non-empty, the storage type is pair::first
     * - If pair::first is empty and pair::second is non-empty, the storage type is pair::second
     */
    template <typename First, typename Second>
    struct PairStorageType
    {
        using type = std::conditional_t<!std::is_empty_v<First>, First, Second>;
    };

    template <typename First, typename Second>
    using PairStorageType_t = typename PairStorageType<First, Second>::type;

  public:
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

    template <typename First>
    bool HasPair(Entity second) const
    {
        return flecsEntity.has<First>(second.GetID());
    }

    template <typename First>
    bool HasPair(Entity::ID second) const
    {
        return flecsEntity.has<First>(second.GetID());
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
     * Storage type is automatically determined:
     * - If First is non-empty, value type is First
     * - If First is empty and Second is non-empty, value type is Second
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @param value Value to set (of the storage type).
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &SetPair(const PairStorageType_t<First, Second> &value)
    {
        flecsEntity.set<First, Second>(value);
        return *this;
    }

    /**
     * @brief Set the value of a pair component (rvalue overload).
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @param value Value to set (of the storage type).
     * @return Reference to this entity.
     */
    template <typename First, typename Second>
    Entity &SetPair(PairStorageType_t<First, Second> &&value)
    {
        flecsEntity.set<First, Second>(std::forward<PairStorageType_t<First, Second>>(value));
        return *this;
    }

    /**
     * @brief Get a const reference to a pair component.
     * Returns the storage type (First if non-empty, else Second).
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Const reference to the pair component storage type.
     */
    template <typename First, typename Second>
    const PairStorageType_t<First, Second> &GetPair() const
    {
        return flecsEntity.get<First, Second>();
    }

    /**
     * @brief Try to get a const pointer to a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Pointer to the pair component storage type, or nullptr if not present.
     */
    template <typename First, typename Second>
    const PairStorageType_t<First, Second> *TryGetPair() const
    {
        return flecsEntity.try_get<First, Second>();
    }

    /**
     * @brief Get a mutable reference to a pair component.
     * @tparam First First component type.
     * @tparam Second Second component type.
     * @return Mutable reference to the pair component storage type.
     */
    template <typename First, typename Second>
    PairStorageType_t<First, Second> &GetPairMut()
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
     * @brief Modify components using a callback function.
     * The callback receives references to mutable components and can modify them.
     * Components are automatically marked as modified after the callback returns.
     * @tparam Func The function type.
     * @param func The function to apply.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &Modify(const Func &func)
    {
        func(*this);
        return *this;
    }

    /**
     * @brief Enable a specific component.
     * The component must have the CanToggle trait to be enabled/disabled.
     * Add the trait when registering: world.Component<T>().Add<flecs::CanToggle>();
    /**
     * @brief Enable a specific component.
     * The component must have the CanToggle trait to be enabled/disabled.
     * Add the trait when registering: world.Component<T>().Add(flecs::CanToggle);
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
     * The component must have the CanToggle trait to be enabled/disabled.
     * Add the trait when registering: world.Component<T>().Add(flecs::CanToggle);
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
     * The component must have the CanToggle trait to support this operation.
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
     * Creates a scope where entities created within the callback will be children of this entity.
     * @tparam Func The function type.
     * @param func The function to execute, receives a reference to this entity.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &WithScope(Func &&func);

    /**
     * @brief Execute a function with this entity as the "with" context.
     * Creates a with context where components added within the callback will include this entity.
     * @tparam Func The function type.
     * @param func The function to execute, receives a reference to this entity.
     * @return Reference to this entity.
     */
    template <typename Func>
    Entity &WithComponent(Func &&func);

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
     * Iterates over all targets where this entity has the specified relationship.
     * @tparam Relationship The relationship type.
     * @tparam Func The function type.
     * @param func The function to execute, receives each target entity.
     */
    template <typename Relationship, typename Func>
    void ForEachTarget(Func &&func) const;

    // template <typename Func>
    // void ForEachComponent(Func &&func) const
    //{
    //     flecsEntity.each([&](flecs::id &id) {
    //         // id represents the component type
    //         // You can get type information and invoke the callback
    //         ID id_(id, world);
    //         func(id_);
    //         //if (id.is_entity() && id.entity().has<flecs::Component>())
    //         //{
    //         //    flecs::entity fe = id.entity();
    //         //    Entity e(fe, world);

    //        //    func(e);
    //        //}
    //        //else
    //        //{
    //        //    DN_CORE_WARN("Component ID {} is not an Entity!", id.raw_id());
    //        //}
    //    });
    //}

    template <typename Func>
    void ForEachComponent(Func &&func) const
    {
        // Get the entity type (includes inherited components via IsA)
        const ecs_type_t *type = ecs_get_type(flecsEntity.world().c_ptr(), flecsEntity.id());

        if (type && type->array)
        {
            for (int32_t i = 0; i < type->count; i++)
            {
                flecs::id id(flecsEntity.world(), type->array[i]);
                ID id_(id, world);
                func(id_);
            }
        }
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

    World *world = nullptr;    ///< Pointer to the world this entity belongs to (non-owning).
    flecs::entity flecsEntity; ///< Internal flecs entity handle.

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
