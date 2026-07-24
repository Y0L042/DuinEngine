#pragma once

#include <flecs.h>
#include <memory>
#include <type_traits>
#include "Duin/Core/Debug/DNLog.h"
#include "Iter.h"   // duin::Iter (constructed in Run)
#include "Entity.h" // full Entity type (constructed in Each and Entity())
#include "World.h"  // full World type (SystemHandle holds World by value)

// TODO Add Asserts

namespace duin
{

// Forward declarations
class World;

// Forward declare System before SystemBuilder
class System;

/**
 * @brief Wrapper for flecs::system_builder to provide a unified system builder API.
 *
 * Mirrors QueryBuilder. Unlike QueryBuilder there is no Build(): the terminal
 * methods Each()/Run() register the callback and return the constructed System.
 *
 * IMPORTANT: flecs::system_builder (via node_builder) defines NO copy/move
 * constructor, so its internal desc_/term_ pointers reference its own storage by
 * raw address. Moving or copying it leaves those pointers dangling at the source
 * object, and any term added afterwards (With/Without/Each) writes into freed
 * memory. (flecs::query_builder is safe because its base DOES re-point on
 * move/copy.) We therefore heap-hold the flecs builder in a unique_ptr and never
 * move it after construction, keeping its internal pointers valid for the whole
 * lifetime of the chain.
 */
template <typename... Components>
class SystemBuilder
{
  public:
    SystemBuilder(flecs::world_t *flecsWorld, const char *name)
        : flecsSystemBuilder(std::make_unique<flecs::system_builder<Components...>>(flecsWorld, name))
    {
    }

    // ---- term methods ----
    // Each type becomes its own AND-ed filter term, so With<A, B>() is
    // equivalent to chaining .With<A>().With<B>(). We expand the pack into one
    // with<T>() call per type rather than forwarding the whole pack: flecs'
    // with<First, Second>() is the relationship-pair overload, which would build
    // a single (A, B) pair term instead of two separate terms.
    template <typename... Filters>
    SystemBuilder<Components...> &With()
    {
        (flecsSystemBuilder->template with<Filters>(), ...);
        return *this;
    }

    // Each type becomes its own negated term (excludes entities having any of
    // them), matching .Without<A>().Without<B>(). See With() above for why the
    // pack is expanded per-type rather than forwarded to without<First, Second>().
    template <typename... Filters>
    SystemBuilder<Components...> &Without()
    {
        (flecsSystemBuilder->template without<Filters>(), ...);
        return *this;
    }

    SystemBuilder<Components...> &TermAt(int32_t i)
    {
        flecsSystemBuilder->term_at(i);
        return *this;
    }

    // ---- system config methods (system_builder_i) ----

    /**
     * @brief Specify the phase the system should run in.
     * @param phase The phase entity id (pass 0 to strip the current phase).
     */
    SystemBuilder<Components...> &Kind(uint64_t phase)
    {
        flecsSystemBuilder->kind(phase);
        return *this;
    }

    /**
     * @brief Specify whether the system can run on multiple threads.
     */
    SystemBuilder<Components...> &MultiThreaded(bool value = true)
    {
        flecsSystemBuilder->multi_threaded(value);
        return *this;
    }

    /**
     * @brief Specify whether the system should run in immediate (unstaged) context.
     */
    SystemBuilder<Components...> &Immediate(bool value = true)
    {
        flecsSystemBuilder->immediate(value);
        return *this;
    }

    /**
     * @brief Set the system interval (runs at the specified interval in seconds).
     */
    SystemBuilder<Components...> &Interval(float interval)
    {
        flecsSystemBuilder->interval(interval);
        return *this;
    }

    /**
     * @brief Set the system rate (runs at a multiple of the frame tick).
     */
    SystemBuilder<Components...> &Rate(int32_t rate)
    {
        flecsSystemBuilder->rate(rate);
        return *this;
    }

    /**
     * @brief Set the system context pointer.
     */
    SystemBuilder<Components...> &Ctx(void *ptr)
    {
        flecsSystemBuilder->ctx(ptr);
        return *this;
    }

    /**
     * @brief Build the system with a per-entity callback.
     * Automatically wraps flecs::entity into duin::Entity for the callback.
     * @tparam Func The callback function type.
     * @param func The callback invoked for each matching entity.
     *
     * Supported callback signature:
     *  - func(duin::Entity e, Components& ...)
     *
     * @return Constructed System object.
     */
    template <typename Func>
    System Each(Func &&func)
    {
        // NOTE: a system callback is stored and invoked later (every progress),
        // so the functor must be captured BY VALUE. This differs from
        // Query::Each, which captures by reference because the query callback
        // runs synchronously and immediately. The flecs entity passed to the
        // callback already carries its own world.
        flecs::system sys = flecsSystemBuilder->each(
            [func = std::forward<Func>(func)](
                flecs::entity flecsEntity,
                std::conditional_t<std::is_pointer_v<Components>, Components, Components &>... comps) {
                duin::Entity duinEntity(flecsEntity);
                func(duinEntity, comps...);
            });
        return System(std::move(sys));
    }

    /**
     * @brief Build the system with a per-table run callback.
     * Automatically wraps flecs::iter into duin::Iter for the callback.
     * @tparam Func The callback function type.
     * @param func The callback invoked with (duin::Iter&).
     *
     * @return Constructed System object.
     */
    template <typename Func>
    System Run(Func &&func)
    {
        flecs::system sys = flecsSystemBuilder->run([func = std::forward<Func>(func)](flecs::iter &flecsIter) {
            duin::Iter duinIter(flecsIter);
            func(duinIter);
        });
        return System(std::move(sys));
    }

  private:
    std::unique_ptr<flecs::system_builder<Components...>> flecsSystemBuilder;
};

struct SystemHandle
{
    World world;
    Entity system;
};

/**
 * @brief A flecs system, modelled exactly like flecs::system.
 *
 * Mirrors `flecs::system final : flecs::entity`: a System IS the system entity.
 * It is non-template and stores no extra members — the inherited Entity identity
 * (world + flecs entity) is the system. System-specific methods reconstruct a
 * flecs::system on demand from that entity, exactly as flecs::world::system(e)
 * adopts an existing entity (it does NOT create a new system).
 */
class System : public Entity
{
  public:
    /**
     * @brief Default constructor. Produces an invalid/null system.
     * Use IsValid() before calling Run(), Enable(), etc.
     */
    System() = default;

    // Inherit Entity's constructors (mirrors flecs' `using entity::entity;`).
    using Entity::Entity;

    /**
     * @brief Construct from a built flecs system (move). flecs::system IS-A
     * flecs::entity, so this just adopts its entity (which carries its world).
     */
    System(flecs::system &&other)
        : Entity(static_cast<const flecs::entity &>(other))
    {
    }

    /**
     * @brief Construct from a built flecs system (copy).
     */
    System(const flecs::system &other) : Entity(static_cast<const flecs::entity &>(other))
    {
    }

    /**
     * @brief Construct a System wrapper from an existing system entity.
     *
     * Mirrors flecs::world::system(entity): adopts the entity as a runnable
     * system; it does NOT create a new system. The entity already carries its
     * own world.
     */
    System(const Entity &entity) : Entity(entity)
    {
    }

    ~System() = default;

    /**
     * @brief Get the underlying flecs system (by value).
     *
     * There is no stored flecs::system; this reconstructs one from the entity.
     * Returns an invalid system when this System is invalid.
     */
    flecs::system GetFlecsSystem() const
    {
        DN_CORE_ASSERT(IsValid(), "System entity is invalid!");
        if (!IsValid())
            return flecs::system();
        return AsFlecsSystem();
    }

    /**
     * @brief Return a handle to this system.
     */
    SystemHandle GetHandle()
    {
        return SystemHandle{GetWorld(), *this};
    }
    // NOTE: GetWorld() now returns a World by value (non-owning view); the
    // SystemHandle stores it by value.

    /**
     * @brief Run the system once immediately (outside the pipeline), like ecs_run.
     * @param deltaTime Delta time to pass to the system.
     * @param param Optional parameter forwarded to the system context.
     */
    void Run(float deltaTime = 0.0f, void *param = nullptr) const
    {
        if (!IsValid())
            return;
        // The run() fluent temporary calls ecs_run from its destructor at the ;.
        AsFlecsSystem().run(deltaTime, param);
    }

    /**
     * @brief Set the system context pointer.
     */
    void Ctx(void *ptr)
    {
        if (!IsValid())
            return;
        AsFlecsSystem().ctx(ptr);
    }

    /**
     * @brief Get the system context pointer.
     */
    void *Ctx() const
    {
        if (!IsValid())
            return nullptr;
        return AsFlecsSystem().ctx();
    }

    /**
     * @brief Set the system interval in seconds (requires FLECS_TIMER addon).
     */
    void Interval(float interval)
    {
        if (!IsValid())
            return;
        AsFlecsSystem().interval(interval);
    }

    /**
     * @brief Get the system interval in seconds (requires FLECS_TIMER addon).
     */
    float Interval() const
    {
        if (!IsValid())
            return 0.0f;
        // The reconstructed flecs::system is a local non-const temporary, so the
        // non-const interval() getter is callable without const_cast.
        return AsFlecsSystem().interval();
    }

    /**
     * @brief Get the system entity id.
     */
    uint64_t Id() const
    {
        return GetID();
    }

    ///**
    // * @brief Get the system as a duin::Entity (slices to the Entity base).
    // */
    //duin::Entity Entity() const
    //{
    //    return *this;
    //}

  private:
    /**
     * @brief Reconstruct a flecs::system from this entity (mirrors
     * flecs::world::system(entity)). Only valid when IsValid().
     */
    flecs::system AsFlecsSystem() const
    {
        flecs::entity fe = GetFlecsEntity();
        return flecs::system(fe.world(), fe.id());
    }
};
} // namespace duin
