/**
 * @file ECSPhysics.h
 * @brief Physics body and debug/visual component structs.
 * @ingroup ECS_Components
 */

#pragma once

#include "ECSComponents.h"
#include "Duin/Physics/PhysicsIncludes.h"

namespace duin
{

namespace ECSComponent
{

/**
 * @name Physics Body Components
 * Wrappers for PhysX body types.
 * @{
 */


/** @brief Static cube physics body (stub). */
struct PhysicsStaticCubeComponent
{
    std::shared_ptr<PhysicsStaticCubeComponent> cube;
};
/** @} */

/**
 * @name Debug Components
 * Visual debugging aids.
 * @{
 */

/** @brief Debug capsule visualization. */
struct DebugCapsuleComponent
{
    float height, radius;
    int slices, rings;
    // TODO REPLACE_RAYLIB
    // ::Color color;
};

/** @brief Debug cube visualization. */
struct DebugCubeComponent
{
    float width, height, length;
};
/** @} */

}; // namespace ECSComponent

} // namespace duin
