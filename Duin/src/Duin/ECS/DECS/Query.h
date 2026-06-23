#pragma once

#include <flecs.h>
#include <memory>
#include "Duin/Core/Debug/DNLog.h"
#include "Iter.h" // duin::Iter (constructed in Query::Run / QueryBuilder)

namespace duin
{

// Forward declarations
class World;
class Entity;

// Forward declare Query template before QueryBuilder
template <typename... Components>
class Query;

/**
 * @brief Wrapper for flecs::query_builder to provide a unified query builder API.
 */
template <typename... Components>
class QueryBuilder
{
  public:
    QueryBuilder(flecs::query_builder<Components...> &&builder, World *world = nullptr)
        : flecsQueryBuilder(std::move(builder)), world_(world)
    {
    }

    QueryBuilder<Components...> &TermAt(size_t i)
    {
        flecsQueryBuilder.term_at(i);
        return *this;
    }

    // Each type becomes its own AND-ed filter term, so With<A, B>() is
    // equivalent to chaining .With<A>().With<B>(). We expand the pack into one
    // with<T>() call per type rather than forwarding the whole pack: flecs'
    // with<First, Second>() is the relationship-pair overload, which would build
    // a single (A, B) pair term instead of two separate terms.
    template <typename... Comps>
    QueryBuilder<Components...> &With()
    {
        (flecsQueryBuilder.template with<Comps>(), ...);
        return *this;
    }

    // Each type becomes its own negated term (excludes entities having any of
    // them), matching .Without<A>().Without<B>(). See With() above for why the
    // pack is expanded per-type rather than forwarded to without<First, Second>().
    template <typename... Comps>
    QueryBuilder<Components...> &Without()
    {
        (flecsQueryBuilder.template without<Comps>(), ...);
        return *this;
    }

    /**
     * @brief Mark the query as cached.
     * @return Reference to this QueryBuilder for method chaining.
     */
    QueryBuilder<Components...> &Cached()
    {
        flecsQueryBuilder.cached();
        return *this;
    }

    QueryBuilder<Components...> &Parent()
    {
        flecsQueryBuilder.parent();
        return *this;
    }

    QueryBuilder<Components...> &Cascade()
    {
        flecsQueryBuilder.cascade();
        return *this;
    }

    /**
     * @brief Build and return the final Query object.
     * @return Constructed Query object.
     */
    Query<Components...> Build()
    {
        return Query<Components...>(flecsQueryBuilder.build(), world_);
    }

  private:
    flecs::query_builder<Components...> flecsQueryBuilder;
    World *world_ = nullptr;
};

/**
 * @brief Wrapper for flecs::query to provide a unified query API.
 */
template <typename... Components>
class Query
{
  public:
    /**
     * @brief Default constructor. Produces an invalid/null query.
     * Use IsValid() before calling Any(), Each(), Run(), etc.
     */
    Query() = default;

    /**
     * @brief Constructor from flecs query (move).
     * @param other The flecs query to wrap.
     */
    Query(flecs::query<Components...> &&other, World *world = nullptr) : rawQuery(std::move(other)), world_(world)
    {
        //DN_CORE_INFO("Constructing query using r-value move");
    }

    /**
     * @brief Constructor from flecs query (copy).
     * @param other The flecs query to wrap.
     */
    Query(const flecs::query<Components...> &other, World *world = nullptr) : rawQuery(other), world_(world)
    {
        DN_CORE_INFO("Constructing query using l-value");
    }

    /**
     * @brief Destructor.
     */
    ~Query() = default;

    /**
     * @brief Returns true if this query has been built and is safe to iterate.
     */
    bool IsValid() const
    {
        bool res = static_cast<bool>(rawQuery) && world_;
        return res;
    }

    void Destruct()
    {
        if (IsValid())
            rawQuery.destruct();
    }

    /**
     * @brief Get the underlying flecs query.
     * @tparam Comps Optional component types (defaults to Components...).
     * @return Reference to the underlying flecs query.
     */
    template <typename... Comps>
    const flecs::query<Comps...> &GetFlecsQuery() const
    {
        return reinterpret_cast<const flecs::query<Comps...> &>(rawQuery);
    }

    /**
     * @brief Run the query with a callback for each matching entity.
     * Automatically wraps flecs::entity into duin::Entity for the callback.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each entity.
     *
     * Supports these callback signatures:
     *  - func(duin::Entity e, Components& ...)
     *  - func(Components& ...)
     */
    template <typename Func>
    void Each(Func &&func) const
    {
        if (!IsValid())
            return;
        // Use std::conditional so pointer components (optional terms) are passed
        // by value (T*) rather than by reference (T*&), which flecs requires.
        rawQuery.each([&func, this](flecs::entity flecsEntity,
                                    std::conditional_t<std::is_pointer_v<Components>,
                                                       Components,
                                                       Components &>...comps) {
            Entity duinEntity;
            duinEntity.flecsEntity = flecsEntity;
            duinEntity.world = world_;
            func(duinEntity, comps...);
        });
    }

    /**
     * @brief Iterate the query using a duin iterator and callback.
     * Automatically wraps flecs::iter into duin::Iter for the callback.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each iteration.
     *
     * The callback receives (duin::Iter&, Components*...)
     */
    template <typename Func>
    void Iter(Func &&func) const
    {
        assert(false);
    }

    /**
     * @brief Run the query with a single callback invoked once per matching table.
     * Automatically wraps flecs::iter into duin::Iter for the callback.
     * @tparam Func The callback function type.
     * @param func The callback to invoke (receives only duin::Iter&).
     *
     * This is useful for custom iteration logic where you want full control
     * over the iterator.
     */
    template <typename Func>
    void Run(Func &&func) const
    {
        if (!IsValid())
            return;
        // Wrap the user's callback to convert flecs::iter to duin::Iter
        rawQuery.run([&func](flecs::iter &flecsIter) {
            // Create duin::Iter from flecs::iter
            duin::Iter duinIter(flecsIter);

            // Call user's callback with duin::Iter only
            func(duinIter);
        });
    }

    /**
     * @brief Get count of entities matching the query.
     * @return Number of matching entities.
     */
    int32_t Count() const
    {
        if (!IsValid())
            return 0;
        return rawQuery.count();
    }

    /**
     * @brief Check if query has any matches.
     * @return True if at least one entity matches.
     */
    bool IsTrue() const
    {
        if (!IsValid())
            return false;
        return rawQuery.is_true();
    }

    /**
     * @brief Get first matching entity.
     * @return First entity that matches the query.
     */
    flecs::entity First() const
    {
        if (!IsValid())
            return flecs::entity();
        return rawQuery.first();
    }

  private:
    friend class World;
    friend class Entity;
    flecs::query<Components...> rawQuery;
    World *world_ = nullptr;
};
} // namespace duin
