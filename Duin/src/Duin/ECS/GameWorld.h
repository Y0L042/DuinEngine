/**
 * @file GameWorld.h
 * @brief ECS world manager.
 * @ingroup ECS
 *
 * Central hub for the FLECS-based ECS. Contains:
 * - GameWorld class: World management and query execution
 *
 * ECS definitions are in the following files (all included below):
 * - ECSTag.h: Tag components for entity categorization
 * - ECSComponents.h: Spatial data components
 * - ECSPhysics.h: Physics body and debug components
 * - ECSPrefabs.h: Entity prefab templates
 * - ECSObservers.h: ECS observer registration
 */

#pragma once

#include <any>
#include <memory.h>
#include <unordered_map>

#include <flecs.h>
#include <rfl.hpp>
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Signals/Signal.h"
#include "Duin/Physics/PhysicsIncludes.h"
#include "Duin/Render/Camera.h"

#include "ECSTag.h"
#include "ECSComponents.h"
#include "ECSPhysics.h"
#include "ECSPrefabs.h"
#include "ECSObservers.h"

namespace duin
{
/**
 * @class GameWorld
 * @brief Central ECS world manager.
 * @ingroup ECS_Manager
 *
 * Owns the FLECS world and provides methods for entity creation,
 * component registration, and query execution. Each scene/tab in
 * the editor has its own GameWorld instance.
 *
 * @code
 * GameWorld ecs;
 * ecs.Initialize();
 *
 * duin::Entity e = ecs.world.CreateEntity()
 *     .IsA(ECSPrefab::Node3D)
 *     .Set<ECSComponent::Transform3D>({position});
 * @endcode
 */
class GameWorld : public World
{
  public:
    GameWorld();
    GameWorld(bool connectSignals);
    ~GameWorld();

    /** @brief Initializes the ECS world and registers components. */
    void Initialize(bool connectSignals = true);
    void InitializeRemoteExplorer();

    /** @brief Sets the given entity as the active camera. */
    void ActivateCameraEntity(duin::Entity entity);

    /** @brief Runs post-update queries. Called by engine. */
    virtual void PostUpdateQueryExecution(double delta);
    /** @brief Runs post-physics queries. Called by engine. */
    virtual void PostPhysicsUpdateQueryExecution(double delta);
    /** @brief Runs post-draw queries. Called by engine. */
    virtual void PostDrawQueryExecution();
    /** @brief Runs post-draw-UI queries. Called by engine. */
    virtual void PostDrawUIQueryExecution();

    /** @brief Clears all entities from the world. */
    void Clear();
    void Reset(bool connectSignals = true);

    /**
     * @name Transform Queries
     * @{
     */
    void ExecuteQueryTransform3DHierarchicalUpdate();
    void ExecuteQueryCharacterBody3DUpdateTransform();
    void ExecuteQueryVelocity3DUpdateTransform();
    void ExecuteQuerySyncDynamicBody3DTransform();
    /** @} */

    /** @brief Processes camera activation requests. */
    void ExecuteQuerySetCameraAsActive();

    /**
     * @name Render Queries
     * @{
     */
    void ExecuteQueryControlCamera();
    void ExecuteQueryDrawCube();
    void ExecuteQueryDrawDebugCapsule();
    void ExecuteQueryDrawDebugCube();
    /** @} */

    // Global-space transform helpers
    void SetGlobalTransform(duin::Entity e, ECSComponent::Transform3D tx);
    ECSComponent::Transform3D GetGlobalTransform(duin::Entity e);
    void SetGlobalPosition(duin::Entity e, Vector3 position);
    Vector3 GetGlobalPosition(duin::Entity e);
    void SetGlobalScale(duin::Entity e, Vector3 scale);
    Vector3 GetGlobalScale(duin::Entity e);
    void SetGlobalRotation(duin::Entity e, Quaternion rotation);
    Quaternion GetGlobalRotation(duin::Entity e);

  protected:
    // Query cache — queries are built on first call and reused thereafter.
    // Accessible to subclasses so they can inline their own queries.
    template <typename... Comps>
    static std::string MakeQueryKey(const char *tag)
    {
        std::string key = tag;
        key += ":";
        ((key += typeid(Comps).name(), key += "|"), ...);
        return key;
    }

    template <typename... Comps, typename BuilderFn>
    Query<Comps...> &GetOrBuildQuery(const char *tag, BuilderFn &&builderFn)
    {
        std::string key = MakeQueryKey<Comps...>(tag);
        auto it = queryCache_.find(key);
        if (it != queryCache_.end())
        {
            auto *ptr = std::any_cast<Query<Comps...>>(&it->second);
            DN_CORE_ASSERT(ptr, "Query cache type mismatch");
            return *ptr;
        }
        Query<Comps...> q = std::forward<BuilderFn>(builderFn)(*this);
        auto [ins, ok] = queryCache_.emplace(key, std::move(q));
        return *std::any_cast<Query<Comps...>>(&ins->second);
    }

    void ClearQueryCache()
    {
        queryCache_.clear();
    }

  private:
    std::shared_ptr<ScopedConnection> connPostUpdate_;
    std::shared_ptr<ScopedConnection> connPostPhysicsUpdate_;
    std::shared_ptr<ScopedConnection> connPostDraw_;
    std::shared_ptr<ScopedConnection> connPostDrawUI_;

    std::unordered_map<std::string, std::any> queryCache_;
};

} // namespace duin
