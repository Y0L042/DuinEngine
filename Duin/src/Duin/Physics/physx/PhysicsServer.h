/**
 * @file PhysicsServer.h
 * @brief Central physics world manager.
 * @ingroup Physics_Server
 *
 * Singleton that owns the PhysX scene and handles physics simulation stepping.
 */

#pragma once

#include <PxPhysicsAPI.h>
#include <PxPhysics.h>
#include <characterkinematic/PxController.h>
#include <characterkinematic/PxControllerManager.h>
#include <cooking/PxCooking.h>

namespace duin
{

/**
 * @enum PhysicsServerError
 * @brief Error codes for physics operations.
 * @ingroup Physics_Server
 */
enum PhysicsServerError
{
    SUCCESS = 0,
    INIT_FAILED,
    MATERIAL_CREATION_FAILED
};

class UUID;
class CollisionObject;
class CollisionShape;
class CharacterBody;

/**
 * @class PhysicsServer
 * @brief Singleton managing the PhysX physics world.
 * @ingroup Physics_Server
 *
 * Handles PhysX initialization, scene management, and simulation stepping.
 * Physics bodies (StaticBody, DynamicBody, etc.) use this internally.
 *
 * Connect to PhysX Visual Debugger (PVD) for real-time debugging.
 */
class PhysicsServer
{
  public:
    /** @brief Returns the singleton instance. */
    static PhysicsServer &Get();

    /** @brief Initializes PhysX foundation, physics, and scene. */
    void Initialize();
    /** @brief Releases all PhysX resources. */
    void Clean();

    /** @brief Advances physics simulation. @param delta Time step in seconds. */
    void StepPhysics(double delta);

  protected:
    friend class StaticBody;
    friend class KinematicBody;
    friend class DynamicBody;
    friend class CharacterBody;
    friend class CollisionShape;
    friend class PhysicsMaterial;

    physx::PxDefaultAllocator pxAllocatorCallback;
    physx::PxDefaultErrorCallback pxErrorCallback;
    physx::PxDefaultCpuDispatcher *pxDispatcher = NULL;
    physx::PxTolerancesScale pxToleranceScale;
    physx::PxFoundation *pxFoundation = NULL;
    physx::PxPhysics *pxPhysics = NULL;
    physx::PxScene *pxScene = NULL;
    physx::PxControllerManager *pxControllerManager = NULL;
    physx::PxMaterial *pxMaterial = NULL;
    physx::PxPvd *pxPvd = NULL; ///< PhysX Visual Debugger connection.

    PhysicsServer();
    ~PhysicsServer();
};

} // namespace duin

#undef PHYSICS_SERVER
