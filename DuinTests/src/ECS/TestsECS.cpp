/**
 * @file TestsECS.cpp
 * @brief Unit tests for the Duin ECS system using DocTest.
 * @details This file contains test cases for entity creation, component management, hierarchy, pairs, and queries in the Duin ECS framework.
 */

#include "TestsECS.h"

#include "doctest.h"

#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/Query.h>

/**
 * @brief Test entity creation and deletion in the ECS world.
 */
TEST_CASE("World: Entity Creation and Deletion") {
    duin::World world;
    auto e1 = world.CreateEntity("TestEntity");
    CHECK(e1.IsValid());
    CHECK(e1.GetName() == "TestEntity");
    auto id = e1.GetID();
    world.DeleteEntity(id);
    CHECK(!world.IsAlive(id));
}

/**
 * @brief Test component registration, RemoveAll, and DeleteWith operations.
 */
TEST_CASE("World: Component Registration and RemoveAll/DeleteWith") {
    duin::World world;
    world.Component<Position>();
    auto e1 = world.CreateEntity();
    auto e2 = world.CreateEntity();
    e1.Add<Position>();
    e2.Add<Position>();
    world.DeleteWith<Position>();
    CHECK(!e1.IsAlive());
    CHECK(!e2.IsAlive());

    auto e3 = world.CreateEntity();
    e3.Add<Velocity>();
    world.RemoveAll<Velocity>();
    CHECK(!e3.Has<Velocity>());
}

/**
 * @brief Test deferred execution of ECS commands.
 */
TEST_CASE("World: Deferred Execution") {
    duin::World world;
    auto e1 = world.CreateEntity();
    world.DeferBegin();
    e1.Add<Position>();
    world.DeferEnd();
    CHECK(e1.Has<Position>());
}

/**
 * @brief Test world simulation progress.
 */
TEST_CASE("World: Progress Simulation") {
    duin::World world;
    CHECK(world.Progress(0.0f));
}

/**
 * @brief Test adding, removing, setting, and getting components on an entity.
 */
TEST_CASE("Entity: Component Add/Remove/Set/Get") {
    duin::World world;
    auto e = world.CreateEntity();
    e.Add<Position>();
    CHECK(e.Has<Position>());
    e.Set<Position>({1,2});
    CHECK(e.Get<Position>() == Position{1,2});
    e.Remove<Position>();
    CHECK(!e.Has<Position>());
}

/**
 * @brief Test setting and getting multiple components on an entity.
 */
TEST_CASE("Entity: Set/Get Multiple Components") {
    duin::World world;
    auto e = world.CreateEntity();
    e.Set(Position{3,4}, Velocity{5.0f,6.0f});
    CHECK(e.Get<Position>() == Position{3,4});
    CHECK(e.Get<Velocity>() == Velocity{5.0f,6.0f});
}

/**
 * @brief Test TryGet and Ensure component operations.
 */
TEST_CASE("Entity: TryGet and Ensure") {
    duin::World world;
	world.Component<Health>();
    duin::Entity e = world.CreateEntity();
    CHECK(e.TryGet<Health>() == nullptr);
	e.Set<Health>({ 42 });
	auto& h = e.Get<Health>();
    //auto& h = e.Ensure<Health>({ 42 });
    CHECK(h.value == 42);
    CHECK(e.TryGet<Health>() != nullptr);
	CHECK(e.Get<Health>().value == 42);
}

/**
 * @brief Test pair operations on entities.
 */
TEST_CASE("Entity: Pair Operations") {
    //duin::World world;
    //auto e = world.CreateEntity();
    //e.AddPair<Position, Velocity>();
    //CHECK(e.HasPair<Position, Velocity>());
    //e.SetPair<Position, Velocity>(Velocity{7.0f,8.0f});
    //CHECK(e.GetPair<Position, Velocity>() == Velocity{7.0f,8.0f});
    //e.RemovePair<Position, Velocity>();
    //CHECK(!e.HasPair<Position, Velocity>());
    // TODO
    CHECK(false);
}

/**
 * @brief Test entity hierarchy operations (parent/child).
 */
TEST_CASE("Entity: Hierarchy Operations") {
    duin::World world;
    auto parent = world.CreateEntity("Parent");
    auto child = world.CreateEntity("Child");
    child.ChildOf(parent);
    CHECK(child.GetParent().GetName() == "Parent");
    auto children = parent.GetChildren();
    CHECK(children.size() == 1);
    CHECK(children[0].GetName() == "Child");
}

/**
 * @brief Test enabling and disabling components on an entity.
 */
TEST_CASE("Entity: Enable/Disable Component") {
    duin::World world;
    auto e = world.CreateEntity();
    e.Add<Health>();
    e.DisableComponent<Health>();
    CHECK(!e.IsComponentEnabled<Health>());
    e.EnableComponent<Health>();
    CHECK(e.IsComponentEnabled<Health>());
}

/**
 * @brief Test assign and modify operations on entity components.
 */
TEST_CASE("Entity: Assign and Modify") {
    duin::World world;
    auto e = world.CreateEntity();
    e.Set(Health{50});
    e.Assign(Health{75});
    CHECK(e.Get<Health>().value == 75);
    e.Modify([](duin::Entity& ent){ ent.Set(Health{99}); });
    CHECK(e.Get<Health>().value == 99);
}

/**
 * @brief Test query construction and usage (TODO: implement actual query logic).
 */
TEST_CASE("Query: Construction and Usage") {
    //duin::World world;
    //world.Component<Position>();
    //auto e1 = world.CreateEntity();
    //auto e2 = world.CreateEntity();
    //e1.Set(Position{10,20});
    //e2.Set(Position{30,40});
    //flecs::query<Position> flecsQuery(world);
    //duin::Query query = world.QueryBuilder;
    //int count = 0;
    //query.each<Position>([&](flecs::entity, const Position& pos){
    //    count++;
    //    CHECK((pos == Position{10,20} || pos == Position{30,40}));
    //});
    //CHECK(count == 2);
    // TODO
    CHECK(false);
}

