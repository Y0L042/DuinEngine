#pragma once

#include <flecs.h>
#include <memory>

namespace duin
{
/**
 * @brief Wrapper for flecs::query to provide a unified query API.
 */
template <typename... Components>
class Query
{
  public:
    /**
     * @brief Default constructor.
     */
    Query() = default;

    /**
     * @brief Constructor from flecs query (move).
     * @param other The flecs query to wrap.
     */
    Query(flecs::query<Components...> &&other)
        : rawQuery(std::move(other))
    {
    }

    /**
     * @brief Constructor from flecs query (copy).
     * @param other The flecs query to wrap.
     */
    Query(const flecs::query<Components...> &other)
        : rawQuery(other)
    {
    }

    /**
     * @brief Constructor from query builder.
     * @param builder The query builder to build and wrap.
     */
    Query(flecs::query_builder<Components...> &&builder)
        : rawQuery(builder.build())
    {
    }

    /**
     * @brief Destructor.
     */
    ~Query() = default;

    /**
     * @brief Mark query as cached (for builder pattern compatibility).
     * @return Reference to this query.
     */
    Query &Cached()
    {
        return *this;
    }

    /**
     * @brief Build the query (for builder pattern compatibility).
     * @return Reference to this query.
     */
    Query &Build()
    {
        return *this;
    }

    /**
     * @brief Get the underlying flecs query.
     * @tparam Comps Optional component types (defaults to Components...).
     * @return Reference to the underlying flecs query.
     */
    template <typename... Comps>
    const flecs::query<Comps...> &Get() const
    {
        return reinterpret_cast<const flecs::query<Comps...> &>(rawQuery);
    }

    /**
     * @brief Run the query with a callback for each matching entity.
     * @tparam Comps Component types to query.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each entity.
     */
    template <typename... Comps, typename Func>
    void Each(Func &&func) const
    {
        Get<Comps...>().each(std::forward<Func>(func));
    }

    /**
     * @brief Iterate the query using a flecs iterator and callback.
     * @tparam Comps Component types to query.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each iteration.
     */
    template <typename... Comps, typename Func>
    void Iter(Func &&func) const
    {
        Get<Comps...>().iter(std::forward<Func>(func));
    }

  private:
    friend class World;
    flecs::query<Components...> rawQuery;
};
} // namespace duin