#pragma once

#include <flecs.h>
#include <memory>

namespace duin
{
/**
 * @brief Wrapper for flecs::query to provide a unified query API.
 */
template <typename... Components> class Query
{
  public:
    /**
     * @brief Default constructor.
     */
    Query() = default;

    Query(flecs::query&& other)
    {

    }

    /**
     * @brief Destructor.
     */
    ~Query() = default;

    Query& Cached()
    {
        return *this;
    }

    Query& Build()
    {
        return *this;
    }

    /**
     * @brief Run the query with a callback for each matching entity.
     * @tparam Components The component types.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each entity.
     */
    template <typename... Components, typename Func> void Each(Func &&func) const
    {
        Get<Components...>().each(std::forward<Func>(func));
    }

    /**
     * @brief Iterate the query using a flecs iterator and callback.
     * @tparam Components The component types.
     * @tparam Func The callback function type.
     * @param func The callback to invoke for each iteration.
     */
    template <typename... Components, typename Func> void Iter(Func &&func) const
    {
        Get<Components...>().iter(std::forward<Func>(func));
    }

  private:
    friend class World;
};
} // namespace duin