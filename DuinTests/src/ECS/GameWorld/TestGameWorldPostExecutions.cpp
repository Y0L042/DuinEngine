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

TEST_SUITE("GameWorld - Post* Query Executions")
{
    // =========================================================================
    // PostUpdateQueryExecution
    // =========================================================================

    TEST_CASE("PostUpdateQueryExecution - empty world, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.PostUpdateQueryExecution(0.016); }());
    }

    TEST_CASE("PostUpdateQueryExecution - arbitrary delta, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() {
            gw.PostUpdateQueryExecution(0.0);
            gw.PostUpdateQueryExecution(1.0);
            gw.PostUpdateQueryExecution(100.0);
        }());
    }

    TEST_CASE("PostUpdateQueryExecution - with entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        MakeNode3D(gw, "UpdateNode");
        CHECK_NOTHROW([&]() { gw.PostUpdateQueryExecution(0.016); }());
    }

    // =========================================================================
    // PostPhysicsUpdateQueryExecution
    // =========================================================================

    TEST_CASE("PostPhysicsUpdateQueryExecution - empty world, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.PostPhysicsUpdateQueryExecution(0.016); }());
    }

    TEST_CASE("PostPhysicsUpdateQueryExecution - with Node3D hierarchy, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity parent = MakeNode3D(gw, "PhysParent", {{0.0f, 0.0f, 0.0f}});
        duin::Entity child = MakeNode3D(gw, "PhysChild", {{1.0f, 0.0f, 0.0f}});
        child.ChildOf(parent);

        CHECK_NOTHROW([&]() { gw.PostPhysicsUpdateQueryExecution(0.016); }());
    }

    TEST_CASE("PostPhysicsUpdateQueryExecution - runs transform update pipeline")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity parent = MakeNode3D(gw, "TxParent", {{2.0f, 0.0f, 0.0f}});
        duin::Entity child = MakeNode3D(gw, "TxChild", {{3.0f, 0.0f, 0.0f}});
        child.ChildOf(parent);

        // After PostPhysicsUpdate, child global X should be parent + child = 5
        gw.PostPhysicsUpdateQueryExecution(0.016);

        float gx = gw.GetGlobalPosition(child).x;
        CHECK(gx == doctest::Approx(5.0f));
    }

    TEST_CASE("PostPhysicsUpdateQueryExecution - runs camera control pipeline")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Vector3 camPos = {4.0f, 0.0f, 0.0f};
        duin::Entity cam = MakeCamera3D(gw, "PhysUpdateCam");
        cam.Set<duin::ECSComponent::Transform3D>({camPos});
        gw.ActivateCameraEntity(cam);

        gw.PostPhysicsUpdateQueryExecution(0.016);

        const duin::Camera *c = cam.TryGet<duin::Camera>();
        REQUIRE(c != nullptr);
        MSG_CHECK(c->position.x, c->position.x == doctest::Approx(camPos.x));
    }

    TEST_CASE("PostPhysicsUpdateQueryExecution - CharacterBodyComponent with null body is safe")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "NullBodyPhys");
        e.Set<duin::ECSComponent::CharacterBodyComponent>(duin::ECSComponent::CharacterBodyComponent{nullptr})
            .Set<duin::ECSComponent::Velocity3D>({0.0f, 0.0f, 0.0f})
            .Add<duin::ECSTag::PxKinematic>();

        CHECK_NOTHROW([&]() { gw.PostPhysicsUpdateQueryExecution(0.016); }());
    }

    TEST_CASE("PostPhysicsUpdateQueryExecution - multiple calls are stable")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        MakeNode3D(gw, "StableNode");

        CHECK_NOTHROW([&]() {
            gw.PostPhysicsUpdateQueryExecution(0.016);
            gw.PostPhysicsUpdateQueryExecution(0.016);
            gw.PostPhysicsUpdateQueryExecution(0.016);
        }());
    }

    // =========================================================================
    // PostDrawQueryExecution
    // =========================================================================

    TEST_CASE("PostDrawQueryExecution - empty world, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.PostDrawQueryExecution(); }());
    }

    TEST_CASE("PostDrawQueryExecution - entities without debug components, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        MakeNode3D(gw, "PlainDraw");
        CHECK_NOTHROW([&]() { gw.PostDrawQueryExecution(); }());
    }

    TEST_CASE("PostDrawQueryExecution - entity with DebugCapsuleComponent does not crash")
    {
        duin::Application app;
        app.SetHeadless(true);
        app.InitBGFX();
        app.InitRenderer();

        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "CapsuleNode");
        duin::ECSComponent::DebugCapsuleComponent caps;
        caps.height = 1.8f;
        caps.radius = 0.3f;
        caps.slices = 8;
        caps.rings = 4;
        e.Set<duin::ECSComponent::DebugCapsuleComponent>(caps);

        CHECK_NOTHROW([&]() {
            app.BeginRenderFrame();
            gw.PostDrawQueryExecution();
            app.EndRenderFrame();
        }());

        app.CleanRenderer();
        app.ShutdownBGFX();
    }

    TEST_CASE("PostDrawQueryExecution - entity with DebugCubeComponent does not crash")
    {
        duin::Application app;
        app.SetHeadless(true);
        app.InitBGFX();
        app.InitRenderer();

        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "CubeNode");
        duin::ECSComponent::DebugCubeComponent cube;
        cube.width = 1.0f;
        cube.height = 1.0f;
        cube.length = 1.0f;
        e.Set<duin::ECSComponent::DebugCubeComponent>(cube);

        CHECK_NOTHROW(app.BeginRenderFrame());
        CHECK_NOTHROW(gw.PostDrawQueryExecution());
        CHECK_NOTHROW(app.EndRenderFrame());

        app.CleanRenderer();
        app.ShutdownBGFX();
    }

    TEST_CASE("PostDrawQueryExecution - multiple calls are safe")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() {
            gw.PostDrawQueryExecution();
            gw.PostDrawQueryExecution();
        }());
    }

    // =========================================================================
    // PostDrawUIQueryExecution
    // =========================================================================

    TEST_CASE("PostDrawUIQueryExecution - empty world, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.PostDrawUIQueryExecution(); }());
    }

    TEST_CASE("PostDrawUIQueryExecution - with entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        MakeNode3D(gw, "UINode");
        CHECK_NOTHROW([&]() { gw.PostDrawUIQueryExecution(); }());
    }

    TEST_CASE("PostDrawUIQueryExecution - multiple calls are safe")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() {
            gw.PostDrawUIQueryExecution();
            gw.PostDrawUIQueryExecution();
        }());
    }

    // =========================================================================
    // ExecuteQueryDrawDebugCapsule / ExecuteQueryDrawDebugCube
    // =========================================================================

    TEST_CASE("ExecuteQueryDrawDebugCapsule - no debug capsule entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQueryDrawDebugCapsule(); }());
    }

    TEST_CASE("ExecuteQueryDrawDebugCapsule - entity with DebugCapsuleComponent is processed safely")
    {
        duin::Application app;
        app.SetHeadless(true);
        app.InitBGFX();
        app.InitRenderer();

        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "DrawCapsule");
        duin::ECSComponent::DebugCapsuleComponent caps;
        caps.height = 2.0f;
        caps.radius = 0.4f;
        caps.slices = 6;
        caps.rings = 4;
        e.Set<duin::ECSComponent::DebugCapsuleComponent>(caps);

        CHECK_NOTHROW([&]() {
            app.BeginRenderFrame();
            gw.ExecuteQueryDrawDebugCapsule();
            app.EndRenderFrame();
        }());

        app.CleanRenderer();
        app.ShutdownBGFX();
    }

    TEST_CASE("ExecuteQueryDrawDebugCube - no debug cube entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQueryDrawDebugCube(); }());
    }

    TEST_CASE("ExecuteQueryDrawDebugCube - entity with DebugCubeComponent is processed safely")
    {
        duin::Application app;
        app.SetHeadless(true);
        app.InitBGFX();
        app.InitRenderer();

        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "DrawCube");
        duin::ECSComponent::DebugCubeComponent cube;
        cube.width = 2.0f;
        cube.height = 2.0f;
        cube.length = 2.0f;
        e.Set<duin::ECSComponent::DebugCubeComponent>(cube);

        CHECK_NOTHROW([&]() {
            app.BeginRenderFrame();
            gw.ExecuteQueryDrawDebugCube();
            app.EndRenderFrame();
        }());

        app.CleanRenderer();
        app.ShutdownBGFX();
    }

    // =========================================================================
    // Full frame simulation
    // =========================================================================

    TEST_CASE("Full frame: PostPhysics -> PostDraw -> PostDrawUI in sequence")
    {
        duin::Application app;
        app.SetHeadless(true);
        app.InitBGFX();
        app.InitRenderer();

        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity cam = MakeCamera3D(gw, "FrameCam");
        gw.ActivateCameraEntity(cam);

        duin::Entity node = MakeNode3D(gw, "FrameNode");
        duin::ECSComponent::DebugCubeComponent cube{1.0f, 1.0f, 1.0f};
        node.Set<duin::ECSComponent::DebugCubeComponent>(cube);

        CHECK_NOTHROW([&]() {
            gw.PostUpdateQueryExecution(0.016);
            gw.PostPhysicsUpdateQueryExecution(0.016);
            app.BeginRenderFrame();
            gw.PostDrawQueryExecution();
            app.EndRenderFrame();
            gw.PostDrawUIQueryExecution();
        }());

        app.CleanRenderer();
        app.ShutdownBGFX();
    }

    TEST_CASE("Simulated frames survive Reset between them")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        MakeNode3D(gw, "Frame1Node");
        gw.PostPhysicsUpdateQueryExecution(0.016);
        gw.PostDrawQueryExecution();

        gw.Reset(false);

        MakeNode3D(gw, "Frame2Node");
        CHECK_NOTHROW([&]() {
            gw.PostPhysicsUpdateQueryExecution(0.016);
            gw.PostDrawQueryExecution();
        }());
    }
}

} // namespace TestGameWorld
