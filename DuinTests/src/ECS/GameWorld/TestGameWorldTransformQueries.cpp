#include "TestConfig.h"
#include "Defines.h"
#include <doctest.h>
#include <Duin/ECS/GameWorld.h>

namespace TestGameWorld
{

// ---------------------------------------------------------------------------
// Helper — build a Node3D instance and return its entity.
// ---------------------------------------------------------------------------
static duin::Entity MakeNode3D(duin::GameWorld &gw, const char *name,
                               duin::ECSComponent::Transform3D tx = {})
{
    duin::Entity e = gw.Entity(name).IsA(duin::ECSPrefab::Node3D).Set<duin::ECSComponent::Transform3D>(tx);
    return e;
}

} // namespace TestGameWorld
