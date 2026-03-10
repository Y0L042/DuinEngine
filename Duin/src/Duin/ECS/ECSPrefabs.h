/**
 * @file ECSPrefabs.h
 * @brief Entity prefab templates.
 * @ingroup ECS_Prefabs
 */

#pragma once

#include "Duin/ECS/DECS/Entity.h"
#include "Duin/ECS/DECS/World.h"

namespace duin
{

/**
 * @namespace ECSPrefab
 * @brief Entity prefab templates.
 * @ingroup ECS_Prefabs
 *
 * Prefabs are pre-configured entity templates. Create instances via
 * `world.entity().is_a(ECSPrefab::Node3D)`.
 */
namespace ECSPrefab
{
extern duin::Entity Node;   ///< Base node prefab.
extern duin::Entity Node2D; ///< 2D node with Position2D, Rotation2D, Scale2D.
extern duin::Entity Node3D; ///< 3D node with Transform3D.

extern duin::Entity PhysicsStaticBody;    ///< Static body prefab.
extern duin::Entity PhysicsKinematicBody; ///< Kinematic body prefab.
extern duin::Entity PhysicsDynamicBody;   ///< Dynamic body prefab.
extern duin::Entity PhysicsCharacterBody; ///< Character body prefab.

extern duin::Entity Camera3D; ///< 3D camera prefab.
extern duin::Entity Cube;     ///< Renderable cube prefab.

extern duin::Entity DebugCapsule; ///< Debug capsule prefab.

void RegisterPrefabs(duin::World &world);
}; // namespace ECSPrefab

} // namespace duin
