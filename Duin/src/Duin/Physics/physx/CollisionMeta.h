/**
 * @file CollisionMeta.h
 * @brief Collision layer and mask type definitions.
 * @ingroup Physics_Collision
 *
 * Collision filtering uses a layer/mask system:
 * - Layer: Which category this object belongs to (32-bit flags)
 * - Mask: Which categories this object collides with (32-bit flags)
 *
 * Objects A and B collide only if (A.layer & B.mask) && (B.layer & A.mask).
 */

#pragma once

#include <cstdint>

namespace duin
{

/** @brief Collision layer flags (what category an object is). */
typedef uint32_t CollisionLayer;

/** @brief Collision mask flags (what categories to collide with). */
typedef uint32_t CollisionMask;

} // namespace duin
