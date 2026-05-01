#include "TestConfig.h"
#include "Defines.h"
#include <doctest.h>
#include <Duin/Core/Application.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/Render/Camera.h>
#include <Duin/Render/Renderer.h>

namespace TestGameWorld
{

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------
static duin::Entity MakeNode3D(duin::GameWorld &gw, const char *name, duin::ECSComponent::Transform3D tx = {})
{
    return gw.Entity(name).IsA(duin::ECSPrefab::Node3D).Set<duin::ECSComponent::Transform3D>(tx);
}

static duin::Entity MakeCamera3D(duin::GameWorld &gw, const char *name)
{
    return gw.Entity(name)
        .IsA(duin::ECSPrefab::Camera3D)
        .Add<duin::ECSComponent::Transform3D>();
}

} // namespace TestGameWorld
