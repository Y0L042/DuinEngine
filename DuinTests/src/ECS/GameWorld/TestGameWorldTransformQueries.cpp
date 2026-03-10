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

TEST_SUITE("GameWorld - Transform Queries")
{
    // =========================================================================
    // ExecuteQueryTransform3DHierarchicalUpdate
    // =========================================================================

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - no entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQueryTransform3DHierarchicalUpdate(); }());
    }

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - single root entity, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        MakeNode3D(gw, "Root", {{1.0f, 2.0f, 3.0f}});

        CHECK_NOTHROW([&]() { gw.ExecuteQueryTransform3DHierarchicalUpdate(); }());
    }

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - parent-child hierarchy, child cache is invalidated")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity parent = MakeNode3D(gw, "Parent", {{1.0f, 0.0f, 0.0f}});
        duin::Entity child  = MakeNode3D(gw, "Child",  {{2.0f, 0.0f, 0.0f}});
        child.ChildOf(parent);

        // Run one update — must not crash and global position should be
        // reachable after the update.
        CHECK_NOTHROW([&]() { gw.ExecuteQueryTransform3DHierarchicalUpdate(); }());

        auto *tx = child.TryGet<duin::ECSComponent::Transform3D>();
        CHECK(tx != nullptr);
    }

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - three-level hierarchy, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity root = MakeNode3D(gw, "Root",  {{0.0f, 0.0f, 0.0f}});
        duin::Entity mid  = MakeNode3D(gw, "Mid",   {{1.0f, 0.0f, 0.0f}});
        duin::Entity leaf = MakeNode3D(gw, "Leaf",  {{1.0f, 0.0f, 0.0f}});
        mid.ChildOf(root);
        leaf.ChildOf(mid);

        CHECK_NOTHROW([&]() { gw.ExecuteQueryTransform3DHierarchicalUpdate(); }());
    }

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - multiple calls are idempotent")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity parent = MakeNode3D(gw, "P", {{0.0f, 0.0f, 0.0f}});
        duin::Entity child  = MakeNode3D(gw, "C", {{5.0f, 0.0f, 0.0f}});
        child.ChildOf(parent);

        CHECK_NOTHROW([&]() {
            gw.ExecuteQueryTransform3DHierarchicalUpdate();
            gw.ExecuteQueryTransform3DHierarchicalUpdate();
            gw.ExecuteQueryTransform3DHierarchicalUpdate();
        }());
    }

    TEST_CASE("ExecuteQueryTransform3DHierarchicalUpdate - global position reflects parent offset")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::ECSComponent::Transform3D parentTx({2.0f, 0.0f, 0.0f});
        duin::ECSComponent::Transform3D childTx({3.0f, 0.0f, 0.0f});

        duin::Entity parent = MakeNode3D(gw, "P2", parentTx);
        duin::Entity child  = MakeNode3D(gw, "C2", childTx);
        child.ChildOf(parent);

        gw.ExecuteQueryTransform3DHierarchicalUpdate();

        // After update the child's cache should have been refreshed.
        // Global position = parent(2) + child(3) = 5 on X.
        auto *tx = child.TryGet<duin::ECSComponent::Transform3D>();
        REQUIRE(tx != nullptr);
        float gx = gw.GetGlobalPosition(child).x;
        MSG_CHECK(gx, gx == doctest::Approx(5.0f));
    }

    // =========================================================================
    // ExecuteQueryCharacterBody3DUpdateTransform
    // =========================================================================

    TEST_CASE("ExecuteQueryCharacterBody3DUpdateTransform - no entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQueryCharacterBody3DUpdateTransform(); }());
    }

    TEST_CASE("ExecuteQueryCharacterBody3DUpdateTransform - entity without CharacterBodyComponent is ignored")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        // A plain Node3D has no CharacterBodyComponent — should be silently skipped.
        MakeNode3D(gw, "PlainNode");
        CHECK_NOTHROW([&]() { gw.ExecuteQueryCharacterBody3DUpdateTransform(); }());
    }

    TEST_CASE("ExecuteQueryCharacterBody3DUpdateTransform - entity with null body pointer is safely skipped")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        // CharacterBodyComponent with a null body pointer should not crash
        // (the query implementation guards with `if (!cb.body)`).
        duin::Entity e = MakeNode3D(gw, "NullBodyChar");
        e.Set<duin::ECSComponent::CharacterBodyComponent>(duin::ECSComponent::CharacterBodyComponent{nullptr})
         .Set<duin::ECSComponent::Velocity3D>({0.0f, 0.0f, 0.0f})
         .Add<duin::ECSTag::PxKinematic>();

        CHECK_NOTHROW([&]() { gw.ExecuteQueryCharacterBody3DUpdateTransform(); }());
    }

    TEST_CASE("ExecuteQueryCharacterBody3DUpdateTransform - zero velocity with null body does not mutate transform")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = MakeNode3D(gw, "ZeroVelChar");
        e.Set<duin::ECSComponent::CharacterBodyComponent>(duin::ECSComponent::CharacterBodyComponent{nullptr})
         .Set<duin::ECSComponent::Velocity3D>({0.0f, 0.0f, 0.0f})
         .Add<duin::ECSTag::PxKinematic>();

        duin::Vector3 posBefore = e.Get<duin::ECSComponent::Transform3D>().GetPosition();

        gw.ExecuteQueryCharacterBody3DUpdateTransform();

        // With a null body the update is a no-op; position must be unchanged.
        duin::Vector3 posAfter = e.Get<duin::ECSComponent::Transform3D>().GetPosition();
        CHECK(posBefore.x == doctest::Approx(posAfter.x));
        CHECK(posBefore.y == doctest::Approx(posAfter.y));
        CHECK(posBefore.z == doctest::Approx(posAfter.z));
    }

    // =========================================================================
    // ExecuteQuerySyncDynamicBody3DTransform
    // =========================================================================

    TEST_CASE("ExecuteQuerySyncDynamicBody3DTransform - no entities, no crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.ExecuteQuerySyncDynamicBody3DTransform(); }());
    }

    TEST_CASE("ExecuteQuerySyncDynamicBody3DTransform - entity without DynamicBodyComponent is ignored")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        MakeNode3D(gw, "PlainNode2");
        CHECK_NOTHROW([&]() { gw.ExecuteQuerySyncDynamicBody3DTransform(); }());
    }

    // =========================================================================
    // Combined / ordering
    // =========================================================================

    TEST_CASE("All three transform queries run in sequence without crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity root  = MakeNode3D(gw, "SeqRoot",  {{0.0f, 0.0f, 0.0f}});
        duin::Entity child = MakeNode3D(gw, "SeqChild", {{1.0f, 0.0f, 0.0f}});
        child.ChildOf(root);

        CHECK_NOTHROW([&]() {
            gw.ExecuteQueryCharacterBody3DUpdateTransform();
            gw.ExecuteQuerySyncDynamicBody3DTransform();
            gw.ExecuteQueryTransform3DHierarchicalUpdate();
        }());
    }

    TEST_CASE("Transform queries survive Clear and Re-initialize")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        MakeNode3D(gw, "BeforeClear");

        gw.Clear();
        gw.Initialize(false);
        MakeNode3D(gw, "AfterClear", {{10.0f, 0.0f, 0.0f}});

        CHECK_NOTHROW([&]() {
            gw.ExecuteQueryCharacterBody3DUpdateTransform();
            gw.ExecuteQuerySyncDynamicBody3DTransform();
            gw.ExecuteQueryTransform3DHierarchicalUpdate();
        }());
    }
}

} // namespace TestGameWorld
