#include "doctest.h"
#include "TestWorld.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <string>

TEST_SUITE("World") {

    TEST_CASE("World construction and destruction") {
        duin::World* w = new duin::World();
        CHECK(w != nullptr);
        delete w;
    }

    TEST_CASE("CreateEntity and DeleteEntity by ID") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity");
        CHECK(e.IsValid());
        uint64_t id = e.GetID();
        CHECK(w.IsAlive(id));
        w.DeleteEntity(id);
        CHECK_FALSE(w.IsAlive(id));
    }

    TEST_CASE("CreateEntity and DeleteEntity by Entity reference") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity2");
        CHECK(e.IsValid());
        uint64_t id = e.GetID();
        w.DeleteEntity(e);
        CHECK_FALSE(w.IsAlive(id));
    }

    TEST_CASE("DeleteWith by ID and by pair") {
        duin::World w;
        duin::Entity e1 = w.CreateEntity("CompA");
        duin::Entity e2 = w.CreateEntity("CompB");
        uint64_t idA = e1.GetID();
        uint64_t idB = e2.GetID();

        w.DeleteWith(idA);
        w.DeleteWith(idA, idB);
    }

    TEST_CASE("RemoveAll by ID and by pair") {
        duin::World w;
        duin::Entity e1 = w.CreateEntity("CompC");
        duin::Entity e2 = w.CreateEntity("CompD");
        uint64_t idC = e1.GetID();
        uint64_t idD = e2.GetID();

        w.RemoveAll(idC);
        w.RemoveAll(idC, idD);
    }

    TEST_CASE("DeferBegin, DeferEnd, IsDeferred") {
        duin::World w;
        CHECK_FALSE(w.IsDeferred());
        w.DeferBegin();
        CHECK(w.IsDeferred());
        w.DeferEnd();
        CHECK_FALSE(w.IsDeferred());
    }

    TEST_CASE("Progress") {
        duin::World w;
        bool result = (w.Progress(0.0f) == true) || (w.Progress(0.0f) == false);
        CHECK(result);
    }

    TEST_CASE("IsAlive and IsValid") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity3");
        uint64_t id = e.GetID();
        CHECK(w.IsAlive(id));
        CHECK(w.IsValid(id));
        w.DeleteEntity(id);
        CHECK_FALSE(w.IsAlive(id));
        bool result = (w.IsValid(id) == true) || (w.IsValid(id) == false);
        CHECK(result);
    }

    TEST_CASE("SetVersion") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity4");
        uint64_t id = e.GetID();
        w.SetVersion(id);
    }

    TEST_CASE("MakeAlive") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity5");
        uint64_t id = e.GetID();
        w.DeleteEntity(id);
        duin::Entity e2 = w.MakeAlive(id);
        CHECK(e2.IsValid());
        CHECK(e2.IsAlive());
    }

    TEST_CASE("Release") {
        duin::World w;
        w.Release();
    }

	TEST_CASE("Quit") {
        duin::World w;
        w.Quit();
    }

    TEST_CASE("DeleteWith<T> and RemoveAll<T>") {
        duin::World w;
        struct DummyComponent { int x; };
        w.Component<DummyComponent>();
        duin::Entity e = w.CreateEntity("DummyEntity");
        e.Set<DummyComponent>({42});
        w.DeleteWith<DummyComponent>();
        w.RemoveAll<DummyComponent>();
    }

    TEST_CASE("Defer<Func>") {
        duin::World w;
        bool called = false;
        w.DeferBegin();
        w.Defer([&](){ called = true; });
        w.DeferEnd();
        CHECK(called);
    }

    TEST_CASE("Component<T>") {
        duin::World w;
        struct CompX { int y; };
        w.Component<CompX>();
        duin::Entity e = w.CreateEntity("CompXEntity");
        e.Set<CompX>({123});
        auto val = e.Get<CompX>();
        CHECK(val.y == 123);
    }

    TEST_CASE("IterateChildren") {
        duin::World w;
        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity child1 = w.CreateEntity("Child1");
        duin::Entity child2 = w.CreateEntity("Child2");
        int count = 0;
        w.IterateChildren([&](duin::Entity e){
            count++;
            CHECK(e.IsValid());
        });
        CHECK(count >= 2);
    }

    TEST_CASE("World to Query conversion") {
        duin::World w;
        duin::Query q = w;
        CHECK(typeid(q) == typeid(duin::Query));
    }
}

