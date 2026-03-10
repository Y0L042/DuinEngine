/**
 * @file ECSTag.h
 * @brief Tag components for entity categorization.
 * @ingroup ECS_Tags
 */

#pragma once

#include "Duin/ECS/DECS/World.h"

namespace duin
{

/**
 * @namespace ECSTag
 * @brief Tag components for entity categorization.
 * @ingroup ECS_Tags
 *
 * Tags are empty structs used to mark entities. They enable filtering
 * in queries without storing data.
 */
namespace ECSTag
{
/** @brief Marks transform as local (relative to parent). */
struct Local
{
};
/** @brief Marks transform as global (world space). */
struct Global
{
};

/** @brief Queues entity for external reference creation. */
struct CreateExternalRef
{
};
/** @brief Indicates entity has active external references. */
struct ActiveExternalRef
{
};
/** @brief Queues entity for external reference deletion. */
struct DeleteExternalRef
{
};

/** @brief Marks entity as having a kinematic physics body. */
struct PxKinematic
{
};
/** @brief Marks entity as having a dynamic physics body. */
struct PxDynamic
{
};
/** @brief Marks entity as having a static physics body. */
struct PxStatic
{
};
/** @brief Marks entity as not having a physics body. */
struct NonPx
{
};

/** @brief Request to set this camera as active. */
struct SetCameraAsActive
{
};
/** @brief Indicates camera is currently active. */
struct CameraIsActive
{
};
/** @brief Singleton tag for the active camera entity. */
struct ActiveCamera
{
};

void RegisterTags(duin::World &world);
}; // namespace ECSTag

} // namespace duin
