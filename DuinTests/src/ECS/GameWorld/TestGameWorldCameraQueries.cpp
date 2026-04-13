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

    // =========================================================================
    // ExecuteQuerySetCameraAsActive
    // =========================================================================

    TEST_CASE("ExecuteQuerySetCameraAsActive - no camera entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQuerySetCameraAsActive(); }());
    }

    TEST_CASE("ExecuteQuerySetCameraAsActive - camera without ActiveCamera tag is not promoted")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        MakeCamera3D(gw, "InactiveCam");

        // Should not crash; the query only acts on entities with ActiveCamera.
        CHECK_NOTHROW([&]() { gw.ExecuteQuerySetCameraAsActive(); }());
    }

    TEST_CASE("ExecuteQuerySetCameraAsActive - active camera entity results in non-null GetActiveCamera")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "ActiveCam");
        gw.ActivateCameraEntity(cam);

        gw.ExecuteQuerySetCameraAsActive();

        duin::Camera *active = duin::GetActiveCamera();
        CHECK(active != nullptr);
    }

    TEST_CASE("ExecuteQuerySetCameraAsActive - multiple calls are safe")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "RepeatCam");
        gw.ActivateCameraEntity(cam);

        CHECK_NOTHROW([&]() {
            gw.ExecuteQuerySetCameraAsActive();
            gw.ExecuteQuerySetCameraAsActive();
        }());
    }

    // =========================================================================
    // ExecuteQueryControlCamera
    // =========================================================================

    TEST_CASE("ExecuteQueryControlCamera - no camera entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQueryControlCamera(); }());
    }

    TEST_CASE("ExecuteQueryControlCamera - camera position is updated from Transform3D")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Vector3 expectedPos = {3.0f, 5.0f, 7.0f};
        duin::Entity cam = MakeCamera3D(gw, "ControlledCam");
        cam.Set<duin::ECSComponent::Transform3D>(
            duin::ECSComponent::Transform3D{expectedPos});

        gw.ExecuteQueryControlCamera();

        const duin::Camera *c = cam.TryGet<duin::Camera>();
        REQUIRE(c != nullptr);
        CHECK(c->position.x == doctest::Approx(expectedPos.x));
        CHECK(c->position.y == doctest::Approx(expectedPos.y));
        CHECK(c->position.z == doctest::Approx(expectedPos.z));
    }

    TEST_CASE("ExecuteQueryControlCamera - target is offset from position along forward direction")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "ForwardCam");
        // Identity rotation — forward is (0, 0, -1)
        cam.Set<duin::ECSComponent::Transform3D>(
            duin::ECSComponent::Transform3D{{0.0f, 0.0f, 0.0f}});

        gw.ExecuteQueryControlCamera();

        const duin::Camera *c = cam.TryGet<duin::Camera>();
        REQUIRE(c != nullptr);

        // target should be position + forward = (0,0,0) + (0,0,-1) = (0,0,-1)
        CHECK(c->target.x == doctest::Approx(0.0f).epsilon(0.01));
        CHECK(c->target.y == doctest::Approx(0.0f).epsilon(0.01));
        CHECK(c->target.z == doctest::Approx(-1.0f).epsilon(0.01));
    }

    TEST_CASE("ExecuteQueryControlCamera - up vector is always (0,1,0)")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "UpCam");
        cam.Set<duin::ECSComponent::Transform3D>(
            duin::ECSComponent::Transform3D{{0.0f, 0.0f, 0.0f}});

        gw.ExecuteQueryControlCamera();

        const duin::Camera *c = cam.TryGet<duin::Camera>();
        REQUIRE(c != nullptr);
        CHECK(c->cachedLocalUp.x == doctest::Approx(0.0f));
        CHECK(c->cachedLocalUp.y == doctest::Approx(1.0f));
        CHECK(c->cachedLocalUp.z == doctest::Approx(0.0f));
    }

    TEST_CASE("ExecuteQueryControlCamera - multiple cameras all get updated")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam1 = MakeCamera3D(gw, "Multi1");
        duin::Entity cam2 = MakeCamera3D(gw, "Multi2");
        cam1.Set<duin::ECSComponent::Transform3D>(duin::ECSComponent::Transform3D{{1.0f, 0.0f, 0.0f}});
        cam2.Set<duin::ECSComponent::Transform3D>(duin::ECSComponent::Transform3D{{9.0f, 0.0f, 0.0f}});

        gw.ExecuteQueryControlCamera();

        const duin::Camera *c1 = cam1.TryGet<duin::Camera>();
        const duin::Camera *c2 = cam2.TryGet<duin::Camera>();
        REQUIRE(c1 != nullptr);
        REQUIRE(c2 != nullptr);
        CHECK(c1->position.x == doctest::Approx(1.0f));
        CHECK(c2->position.x == doctest::Approx(9.0f));
    }

    // =========================================================================
    // Combined camera pipeline
    // =========================================================================

    TEST_CASE("Full camera pipeline: activate -> control -> setActive")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Vector3 camPos = {0.0f, 10.0f, 20.0f};
        duin::Entity cam = MakeCamera3D(gw, "PipelineCam");
        cam.Set<duin::ECSComponent::Transform3D>(duin::ECSComponent::Transform3D{camPos});

        gw.ActivateCameraEntity(cam);
        gw.ExecuteQueryControlCamera();
        gw.ExecuteQuerySetCameraAsActive();

        const duin::Camera *active = duin::GetActiveCamera();
        REQUIRE(active != nullptr);
        MSG_CHECK(active->position.x, active->position.x == doctest::Approx(camPos.x));
        MSG_CHECK(active->position.y, active->position.y == doctest::Approx(camPos.y));
        MSG_CHECK(active->position.z, active->position.z == doctest::Approx(camPos.z));
    }
}

} // namespace TestGameWorld
