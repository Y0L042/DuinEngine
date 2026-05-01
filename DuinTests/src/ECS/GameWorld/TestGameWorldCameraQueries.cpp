#include "TestConfig.h"
#include "Defines.h"
#include <doctest.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/Render/Camera.h>

namespace TestGameWorld
{

// ---------------------------------------------------------------------------
// Helper — spawn a Camera3D prefab instance.
// ---------------------------------------------------------------------------
static duin::Entity MakeCamera3D(duin::GameWorld &gw, const char *name)
{
    return gw.Entity(name)
             .IsA(duin::ECSPrefab::Camera3D)
             .Set<duin::ECSComponent::Transform3D>(duin::ECSComponent::Transform3D{});
}

TEST_SUITE("GameWorld - Camera Queries")
{
    // =========================================================================
    // ActivateCameraEntity
    // =========================================================================

    TEST_CASE("ActivateCameraEntity - no crash when called before Initialize")
    {
        duin::GameWorld gw;
        duin::Entity dummy; // invalid entity
        CHECK_NOTHROW([&]() { gw.ActivateCameraEntity(dummy); }());
    }

    TEST_CASE("ActivateCameraEntity - entity without Camera component is silently ignored")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = gw.Entity("NoCam").IsA(duin::ECSPrefab::Node3D);
        CHECK_NOTHROW([&]() { gw.ActivateCameraEntity(e); }());

        // The entity must NOT have received ActiveCamera from this call.
        CHECK_FALSE(e.Has<duin::ECSTag::ActiveCamera>());
    }

    TEST_CASE("ActivateCameraEntity - adds ActiveCamera tag to a valid camera entity")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "Cam1");
        REQUIRE(cam.Has<duin::Camera>());

        gw.ActivateCameraEntity(cam);

        CHECK(cam.Has<duin::ECSTag::ActiveCamera>());
    }

    TEST_CASE("ActivateCameraEntity - switching active camera removes tag from previous")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam1 = MakeCamera3D(gw, "CamA");
        duin::Entity cam2 = MakeCamera3D(gw, "CamB");

        gw.ActivateCameraEntity(cam1);
        CHECK(cam1.Has<duin::ECSTag::ActiveCamera>());

        gw.ActivateCameraEntity(cam2);
        // cam1 should have had its ActiveCamera tag removed.
        CHECK_FALSE(cam1.Has<duin::ECSTag::ActiveCamera>());
        CHECK(cam2.Has<duin::ECSTag::ActiveCamera>());
    }

    TEST_CASE("ActivateCameraEntity - activating same entity twice is idempotent")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "CamSame");

        gw.ActivateCameraEntity(cam);
        gw.ActivateCameraEntity(cam);

        CHECK(cam.Has<duin::ECSTag::ActiveCamera>());
    }
}

} // namespace TestGameWorld
