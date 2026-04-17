#pragma once

#define JOLT_PHYSICS

#ifndef JOLT_PHYSICS
#include "physx/CollisionMeta.h"
#include "physx/CollisionShape.h"
#include "physx/CollisionGeometry.h"
#include "physx/PhysicsMaterial.h"
#include "physx/PhysicsStructs.h"
#include "physx/PhysicsServer.h"
#include "physx/CollisionObject.h"
#include "physx/Area.h"
#include "physx/PhysicsObject.h"
#include "physx/StaticBody.h"
#include "physx/KinematicBody.h"
#include "physx/DynamicBody.h"
#include "physx/CharacterBody.h"
#endif

#ifdef JOLT_PHYSICS
#include "jolt/CollisionMeta.h"
#include "jolt/CollisionShape.h"
#include "jolt/CollisionGeometry.h"
#include "jolt/PhysicsMaterial.h"
#include "jolt/PhysicsStructs.h"
#include "jolt/PhysicsServer.h"
#include "jolt/CollisionObject.h"
#include "jolt/Area.h"
#include "jolt/PhysicsObject.h"
#include "jolt/StaticBody.h"
#include "jolt/KinematicBody.h"
#include "jolt/DynamicBody.h"
#include "jolt/CharacterBody.h"
#endif
