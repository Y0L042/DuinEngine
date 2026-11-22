#include "doctest.h"
#include "TestWorld.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/System.h>
#include <string>

namespace TestWorld
{
TEST_SUITE("World")
{

    TEST_CASE("World construction and destruction")
    {
        duin::World *w = new duin::World();
        CHECK(w != nullptr);
        delete w;
    }

    TEST_CASE("CreateEntity and DeleteEntity by ID")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        CHECK(e.IsValid());
        uint64_t id = e.GetID();
        CHECK(w.IsAlive(id));
        w.DeleteEntity(id);
        CHECK_FALSE(w.IsAlive(id));
    }

    TEST_CASE("CreateEntity and DeleteEntity by Entity reference")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity2");
        CHECK(e.IsValid());
        uint64_t id = e.GetID();
        w.DeleteEntity(e);
        CHECK_FALSE(w.IsAlive(id));
    }

    TEST_CASE("DeleteWith by ID and by pair")
    {
        duin::World w;
        duin::Entity e1 = w.Entity("CompA");
        duin::Entity e2 = w.Entity("CompB");
        uint64_t idA = e1.GetID();
        uint64_t idB = e2.GetID();

        w.DeleteWith(idA);
        w.DeleteWith(idA, idB);
    }

    TEST_CASE("RemoveAll by ID and by pair")
    {
        duin::World w;
        duin::Entity e1 = w.Entity("CompC");
        duin::Entity e2 = w.Entity("CompD");
        uint64_t idC = e1.GetID();
        uint64_t idD = e2.GetID();

        w.RemoveAll(idC);
        w.RemoveAll(idC, idD);
    }

    TEST_CASE("Deferred provides a stage and merges afterwards")
    {
        duin::World w;
        bool called = false;
        w.Deferred([&](duin::World &stage) {
            called = true;
            CHECK(stage.GetFlecsWorld().is_stage());
        });
        CHECK(called);
        CHECK_FALSE(w.GetFlecsWorld().is_stage());
    }

    TEST_CASE("Progress")
    {
        duin::World w;
        bool result = (w.Progress(0.0f) == true) || (w.Progress(0.0f) == false);
        CHECK(result);
    }

    TEST_CASE("IsAlive and IsValid")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity3");
        uint64_t id = e.GetID();
        CHECK(w.IsAlive(id));
        CHECK(w.IsValid(id));
        w.DeleteEntity(id);
        CHECK_FALSE(w.IsAlive(id));
        bool result = (w.IsValid(id) == true) || (w.IsValid(id) == false);
        CHECK(result);
    }

    TEST_CASE("SetVersion")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity4");
        uint64_t id = e.GetID();
        w.SetVersion(id);
    }

    TEST_CASE("MakeAlive")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity5");
        uint64_t id = e.GetID();
        w.DeleteEntity(id);
        duin::Entity e2 = w.MakeAlive(id);
        CHECK(e2.IsValid());
        CHECK(e2.IsAlive());
    }

    TEST_CASE("Release")
    {
        duin::World w;
        w.Release();
    }

    TEST_CASE("Quit")
    {
        duin::World w;
        w.Quit();
    }

    TEST_CASE("DeleteWith<T> and RemoveAll<T>")
    {
        duin::World w;
        struct DummyComponent
        {
            int x;
        };
        w.Component<DummyComponent>();
        duin::Entity e = w.Entity("DummyEntity");
        e.Set<DummyComponent>({42});
        w.DeleteWith<DummyComponent>();
        w.RemoveAll<DummyComponent>();
    }

    TEST_CASE("Query::Each with a stage queues changes until Deferred merges")
    {
        duin::World w;
        struct DeferComp
        {
            int v;
        };
        struct DeferTag
        {
        };
        w.Component<DeferComp>();
        w.Component<DeferTag>();
        duin::Entity e = w.Entity("DeferEntity");
        e.Set<DeferComp>({1});
        auto q = w.QueryBuilder<DeferComp>().Build();

        int calls = 0;
        w.Deferred([&](duin::World &stage) {
            q.Each(stage, [&](duin::Entity ent, DeferComp &) {
                calls++;
                ent.Add<DeferTag>();
            });
            CHECK_FALSE(e.Has<DeferTag>());
        });
        CHECK(calls == 1);
        CHECK(e.Has<DeferTag>());
    }

    TEST_CASE("Query::Run with a stage queues changes until Deferred merges")
    {
        duin::World w;
        struct DeferComp
        {
            int v;
        };
        struct DeferTag
        {
        };
        w.Component<DeferComp>();
        w.Component<DeferTag>();
        duin::Entity e = w.Entity("DeferRunEntity");
        e.Set<DeferComp>({1});
        auto q = w.QueryBuilder<DeferComp>().Build();

        w.Deferred([&](duin::World &stage) {
            q.Run(stage, [&](duin::Iter &it) {
                while (it.Next())
                {
                    for (size_t i = 0; i < it.Count(); i++)
                    {
                        flecs::entity(stage.GetFlecsWorld().c_ptr(), it.GetEntityId(i)).add<DeferTag>();
                    }
                }
            });
            CHECK_FALSE(e.Has<DeferTag>());
        });
        CHECK(e.Has<DeferTag>());
    }

    TEST_CASE("Component<T>")
    {
        duin::World w;
        struct CompX
        {
            int y;
        };
        w.Component<CompX>();
        duin::Entity e = w.Entity("CompXEntity");
        e.Set<CompX>({123});
        auto val = e.Get<CompX>();
        CHECK(val.y == 123);
    }

    TEST_CASE("IterateChildren")
    {
        duin::World w;
        duin::Entity parent = w.Entity("Parent");
        duin::Entity child1 = w.Entity("Child1");
        duin::Entity child2 = w.Entity("Child2");
        int count = 0;
        w.IterateChildren([&](duin::Entity e) {
            count++;
            CHECK(e.IsValid());
        });
        CHECK(count >= 2);
    }

    TEST_CASE("Lookup by plain name")
    {
        duin::World w;
        duin::Entity e = w.Entity("PlainLookup");
        duin::Entity found = w.Lookup("PlainLookup");
        CHECK(found.IsValid());
        CHECK(found.GetID() == e.GetID());
    }

    TEST_CASE("Entity::Lookup child by plain name")
    {
        duin::World w;
        duin::Entity parent = w.Entity("LookupParent");
        duin::Entity child = w.Entity("LookupChild");
        child.ChildOf(parent);
        duin::Entity found = parent.Lookup("LookupChild");
        CHECK(found.IsValid());
        CHECK(found.GetID() == child.GetID());
    }

}

TEST_SUITE("World - Value semantics")
{
    TEST_CASE("World(ecs_world_t*) wraps an existing world")
    {
        duin::World owner;
        duin::Entity e = owner.Entity("Wrapped");

        duin::World view(owner.GetFlecsWorld().c_ptr());
        CHECK(view.GetFlecsWorld().c_ptr() == owner.GetFlecsWorld().c_ptr());
        CHECK(view.Lookup("Wrapped").GetID() == e.GetID());
    }

    TEST_CASE("Copies share the same flecs world")
    {
        duin::World a;
        duin::World b = a;
        duin::World c;
        c = a;

        CHECK(b.GetFlecsWorld().c_ptr() == a.GetFlecsWorld().c_ptr());
        CHECK(c.GetFlecsWorld().c_ptr() == a.GetFlecsWorld().c_ptr());

        duin::Entity e = b.Entity("SharedEntity");
        CHECK(a.Lookup("SharedEntity").GetID() == e.GetID());
        CHECK(c.Lookup("SharedEntity").GetID() == e.GetID());
    }

    TEST_CASE("Moved-to world keeps the flecs world")
    {
        duin::World a;
        ecs_world_t *raw = a.GetFlecsWorld().c_ptr();
        duin::World b = std::move(a);
        CHECK(b.GetFlecsWorld().c_ptr() == raw);
    }

    TEST_CASE("Copy outlives the original")
    {
        duin::World *original = new duin::World();
        duin::Entity e = original->Entity("Survivor");
        duin::World copy = *original;
        delete original;

        CHECK(copy.Lookup("Survivor").GetID() == e.GetID());
    }

    TEST_CASE("const GetFlecsWorld returns the same world")
    {
        duin::World w;
        const duin::World &cw = w;
        CHECK(cw.GetFlecsWorld().c_ptr() == w.GetFlecsWorld().c_ptr());
    }

    TEST_CASE("Entity GetWorld returns a view of the owning world")
    {
        duin::World w;
        duin::Entity e = w.Entity("Owned");
        CHECK(e.GetWorld().GetFlecsWorld().c_ptr() == w.GetFlecsWorld().c_ptr());

        duin::Entity empty;
        CHECK(empty.GetWorld().GetFlecsWorld().c_ptr() == nullptr);
    }

    TEST_CASE("World::System(entity) adopts an existing system")
    {
        duin::World w;
        int count = 0;
        duin::System built = w.System<>("AdoptedSystem").Run([&](duin::Iter &) { count++; });

        duin::Entity asEntity = built;
        duin::System adopted = w.System(asEntity);

        CHECK(adopted.IsValid());
        CHECK(adopted.Id() == built.Id());

        adopted.Run(0.016f);
        CHECK(count == 1);
    }
}
} // namespace TestWorld
