#include "TestConfig.h"
#include "Defines.h"
#include <doctest.h>
#include <Duin/ECS/GameWorld.h>

namespace TestGameWorld
{

TEST_SUITE("GameWorld - Lifecycle")
{
    // =========================================================================
    // CONSTRUCTION / DESTRUCTION
    // =========================================================================

    TEST_CASE("Default construction and destruction")
    {
        CHECK_NOTHROW([]() {
            duin::GameWorld gw;
        }());
    }

    TEST_CASE("Heap allocation and deletion")
    {
        duin::GameWorld *gw = nullptr;
        CHECK_NOTHROW([&]() { gw = new duin::GameWorld(); }());
        CHECK(gw != nullptr);
        CHECK_NOTHROW([&]() { delete gw; }());
    }

    TEST_CASE("Multiple independent instances do not share state")
    {
        duin::GameWorld a;
        duin::GameWorld b;
        a.Initialize(false);
        b.Initialize(false);

        duin::Entity ea = a.Entity("OnlyInA");
        CHECK(ea.IsValid());
        CHECK_FALSE(b.IsAlive(ea.GetID()));
    }

    // =========================================================================
    // INITIALIZE
    // =========================================================================

    TEST_CASE("Initialize registers ECSTag components")
    {
        duin::GameWorld gw;
        CHECK_NOTHROW([&]() { gw.Initialize(false); }());

        // ECSTag::Local should be a valid component after init
        duin::Entity tagLocal = gw.IDIfRegistered<duin::ECSTag::Local>().GetEntity();
        CHECK(tagLocal.IsValid());
    }

    TEST_CASE("Initialize registers ECSComponent components")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        //duin::Entity comp = gw.Entity("Transform3D");
        duin::Entity comp = gw.IDIfRegistered<duin::ECSComponent::Transform3D>().GetEntity();
        CHECK(comp.IsValid());
    }

    TEST_CASE("Initialize registers ECSPrefab prefabs")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        // ECSPrefab globals are valid entities pointing into this world
        CHECK(duin::ECSPrefab::Node3D.IsValid());
        CHECK(duin::ECSPrefab::Node2D.IsValid());
        CHECK(duin::ECSPrefab::Camera3D.IsValid());
    }

    TEST_CASE("Initialize called twice does not crash")
    {
        duin::GameWorld gw;
        CHECK_NOTHROW([&]() {
            gw.Initialize(false);
            gw.Initialize(false);
        }());
    }

    TEST_CASE("Initialize with connectSignals=false does not crash")
    {
        duin::GameWorld gw;
        CHECK_NOTHROW([&]() { gw.Initialize(false); }());
    }

    // =========================================================================
    // CLEAR
    // =========================================================================

    TEST_CASE("Clear on un-initialized world does not crash")
    {
        duin::GameWorld gw;
        CHECK_NOTHROW([&]() { gw.Clear(); }());
    }

    TEST_CASE("Clear removes all user entities")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = gw.Entity("Temp");
        CHECK(e.IsValid());
        uint64_t id = e.GetID();

        gw.Clear();

        CHECK_FALSE(gw.IsAlive(id));
    }

    TEST_CASE("Clear resets cached queries — further calls on old queries do not crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() { gw.Clear(); }());
    }

    // =========================================================================
    // RESET
    // =========================================================================

    TEST_CASE("Reset re-registers all components")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        gw.Reset(false);

        duin::Entity comp = gw.IDIfRegistered<duin::ECSComponent::Transform3D>().GetEntity();
        CHECK(comp.IsValid());
    }

    TEST_CASE("Reset re-registers all prefabs")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        gw.Reset(false);

        CHECK(duin::ECSPrefab::Node3D.IsValid());
    }

    TEST_CASE("Reset removes entities created before it")
    {
        duin::GameWorld gw;
        gw.Initialize(false);

        duin::Entity e = gw.Entity("WillBeGone");
        uint64_t id = e.GetID();

        gw.Reset(false);

        CHECK_FALSE(gw.IsAlive(id));
    }

    TEST_CASE("Entities created after Reset are valid")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        gw.Reset(false);

        duin::Entity e = gw.Entity("Fresh");
        CHECK(e.IsValid());
        CHECK(gw.IsAlive(e.GetID()));
    }

    TEST_CASE("Multiple Reset cycles do not crash")
    {
        duin::GameWorld gw;
        gw.Initialize(false);
        CHECK_NOTHROW([&]() {
            gw.Reset(false);
            gw.Reset(false);
            gw.Reset(false);
        }());
    }
}

} // namespace TestGameWorld
