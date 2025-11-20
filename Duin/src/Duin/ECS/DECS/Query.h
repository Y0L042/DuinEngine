#pragma once

#include <flecs.h>
#include <memory>
#include "Duin/Core/Debug/DNLog.h"

namespace duin
{

// Forward declarations
class World;
class Entity;

/**
 * @brief Wrapper for flecs::iter to provide iterator functionality.
 * This class wraps the flecs iterator and provides safe access to iteration data.
 * All public-facing APIs use duin types rather than exposing flecs types directly.
 */
class Iter
{
  public:
    /**
     * @brief Construct from flecs iterator pointer.
     * @param it Pointer to C-style flecs iterator.
     */
    Iter(ecs_iter_t *it) : iter_(it)
    {
    }

    /**
     * @brief Construct from flecs::iter reference.
     * @param it Reference to flecs C++ iterator.
     */
    Iter(flecs::iter &it) : iter_(const_cast<ecs_iter_t *>(it.c_ptr()))
    {
    }

    /**
     * @brief Get the underlying flecs iterator.
     * @return Reference to the underlying flecs::iter.
     */
    flecs::iter GetFlecsIter() const
    {
        return flecs::iter(const_cast<ecs_iter_t *>(iter_));
    }

    /**
     * @brief Get the number of entities in current iteration.
     * @return Number of entities to iterate.
     */
    size_t Count() const
    {
        return iter_->count;
    }

    /**
     * @brief Get delta time since last frame.
     * @return Delta time in seconds.
     */
    float DeltaTime() const
    {
        return iter_->delta_time;
    }

    /**
     * @brief Get delta time since last system invocation.
     * @return Delta system time in seconds.
     */
    float DeltaSystemTime() const
    {
        return iter_->delta_system_time;
    }

    /**
     * @brief Get entity at specified row.
     * @param row Row index of the entity.
     * @return Entity ID at the specified row.
     */
    uint64_t GetEntityId(size_t row) const
    {
        return iter_->entities[row];
    }

    /**
     * @brief Check if field is matched on self.
     * @param index The field index.
     * @return True if field is matched on self, false otherwise.
     */
    bool IsSelf(int8_t index) const
    {
        return ecs_field_is_self(iter_, index);
    }

    /**
     * @brief Check if field is set.
     * @param index The field index.
     * @return True if field is set, false otherwise.
     */
    bool IsSet(int8_t index) const
    {
        return ecs_field_is_set(iter_, index);
    }

    /**
     * @brief Check if field is readonly.
     * @param index The field index.
     * @return True if field is readonly, false otherwise.
     */
    bool IsReadonly(int8_t index) const
    {
        return ecs_field_is_readonly(iter_, index);
    }

    /**
     * @brief Get number of fields in iterator.
     * @return Number of fields.
     */
    int32_t FieldCount() const
    {
        return iter_->field_count;
    }

    /**
     * @brief Get size of field data type.
     * @param index The field index.
     * @return Size of the field in bytes.
     */
    size_t Size(int8_t index) const
    {
        return ecs_field_size(iter_, index);
    }

    /**
     * @brief Get field source entity ID (0 if This).
     * @param index The field index.
     * @return Entity ID of the field source.
     */
    uint64_t GetSourceId(int8_t index) const
    {
        return ecs_field_src(iter_, index);
    }

    /**
     * @brief Get id matched for field.
     * @param index The field index.
     * @return Entity ID matched for the field.
     */
    uint64_t GetFieldId(int8_t index) const
    {
        return ecs_field_id(iter_, index);
    }

    /**
     * @brief Get column index for field.
     * @param index The field index.
     * @return Column index.
     */
    int32_t GetColumnIndex(int8_t index) const
    {
        return ecs_field_column(iter_, index);
    }

    /**
     * @brief Get term index that triggered an observer.
     * @return Term index.
     */
    int8_t GetTermIndex() const
    {
        return iter_->term_index;
    }

    /**
     * @brief Range-based for loop support - begin iterator.
     * @return Begin iterator for range-based for loops.
     */
    auto begin() const
    {
        return flecs::iter(const_cast<ecs_iter_t *>(iter_)).begin();
    }

    /**
     * @brief Range-based for loop support - end iterator.
     * @return End iterator for range-based for loops.
     */
    auto end() const
    {
        return flecs::iter(const_cast<ecs_iter_t *>(iter_)).end();
    }

    /**
     * @brief Skip current table.
     * This indicates to the query that the data in the current table is not
     * modified. By default, iterating a table will mark components as dirty.
     */
    void Skip()
    {
        ecs_iter_skip(iter_);
    }

    /**
     * @brief Get group id for current table (grouped queries only).
     * @return Group ID.
     */
    uint64_t GetGroupId() const
    {
        return ecs_iter_get_group(iter_);
    }

    /**
     * @brief Get value of variable by id.
     * @param varId Variable ID.
     * @return Entity ID of the variable value.
     */
    uint64_t GetVar(int varId) const
    {
        return ecs_iter_get_var(iter_, varId);
    }

    /**
     * @brief Get value of variable by name.
     * @param name Variable name.
     * @return Entity ID of the variable value, or 0 if not found.
     */
    uint64_t GetVar(const char *name) const
    {
        int varId = ecs_query_find_var(iter_->query, name);
        if (varId == -1)
        {
            return 0;
        }
        return GetVar(varId);
    }

    /**
     * @brief Progress iterator (use in run() callbacks).
     * This operation should only be called from a context where the iterator
     * is not being progressed automatically.
     * @return True if iteration should continue, false if complete.
     */
    bool Next()
    {
        if (iter_->flags & EcsIterIsValid && iter_->table)
        {
            ECS_TABLE_UNLOCK(iter_->world, iter_->table);
        }
        bool result = iter_->next(iter_);
        iter_->flags |= EcsIterIsValid;
        if (result && iter_->table)
        {
            ECS_TABLE_LOCK(iter_->world, iter_->table);
        }
        return result;
    }

    /**
     * @brief Free iterator resources.
     * This operation only needs to be called when the iterator is not iterated
     * until completion (e.g. the last call to Next() did not return false).
     */
    void Fini()
    {
        if (iter_->flags & EcsIterIsValid && iter_->table)
        {
            ECS_TABLE_UNLOCK(iter_->world, iter_->table);
        }
        ecs_iter_fini(iter_);
    }

  private:
    ecs_iter_t *iter_;
};





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
    QueryBuilder(flecs::query_builder<Components...> &&builder) : flecsQueryBuilder(std::move(builder))
    {
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

    /**
     * @brief Build and return the final Query object.
     * @return Constructed Query object.
     */
    Query<Components...> Build()
    {
        return Query<Components...>(flecsQueryBuilder.build());
    }

  private:
    flecs::query_builder<Components...> flecsQueryBuilder;
};





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
    Query() = delete;

    /**
     * @brief Constructor from flecs query (move).
     * @param other The flecs query to wrap.
     */
    Query(flecs::query<Components...> &&other) : rawQuery(std::move(other))
    {
        DN_CORE_INFO("Constructing query using r-value move");
    }

    /**
     * @brief Constructor from flecs query (copy).
     * @param other The flecs query to wrap.
     */
    Query(const flecs::query<Components...> &other) : rawQuery(other)
    {
        DN_CORE_INFO("Constructing query using l-value");
    }

    /**
     * @brief Constructor from query builder.
     * @param builder The query builder to build and wrap.
     */
    Query(flecs::query_builder<Components...> &&builder) : rawQuery(builder.build())
    {
        DN_CORE_INFO("Constructing query using query_builder.build()");
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
        rawQuery.each([&func](flecs::entity flecsEntity, Components &...comps) {
            Entity duinEntity;
            duinEntity.flecsEntity = flecsEntity;
            duinEntity.world = nullptr;
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
        return rawQuery.count();
    }

    /**
     * @brief Check if query has any matches.
     * @return True if at least one entity matches.
     */
    bool IsTrue() const
    {
        return rawQuery.is_true();
    }

    /**
     * @brief Get first matching entity.
     * @return First entity that matches the query.
     */
    flecs::entity First() const
    {
        return rawQuery.first();
    }

  private:
    friend class World;
    friend class Entity;
    flecs::query<Components...> rawQuery;
};
} // namespace duin