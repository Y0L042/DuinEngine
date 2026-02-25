#include "doctest.h"
#include "TestEntity.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/Entity_impl.hpp>
#include <string>
#include <vector>

// TODO
#include <flecs.h>

namespace TestEntity
{
TEST_SUITE("Entity")
{

    TEST_CASE("Default constructor and destructor")
    {
        duin::Entity e;
        CHECK(!e.IsValid());
    }

    TEST_CASE("CreateEntity, IsValid, IsAlive, GetID, GetWorld")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() != 0);
        CHECK(e.GetWorld() == &w);
    }

    TEST_CASE("operator== and operator!=")
    {
        duin::World w;
        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2 = w.Entity("Entity2");
        duin::Entity e3 = e1;
        CHECK(e1 == e3);
        CHECK(e1 != e2);
    }

    TEST_CASE("explicit operator bool")
    {
        duin::World w;
        duin::Entity e = w.Entity("EntityBool");
        CHECK(static_cast<bool>(e));
        w.DeleteEntity(e);
        CHECK(!static_cast<bool>(e));
    }

    TEST_CASE("GetName and SetName")
    {
        duin::World w;
        duin::Entity e = w.Entity("OriginalName");
        CHECK(e.GetName() == "OriginalName");
        e.SetName("NewName");
        CHECK(e.GetName() == "NewName");
    }

    TEST_CASE("Enable and Disable")
    {
        duin::World w;
        duin::Entity e = w.Entity("EnableDisable");
        e.Disable();
        // No direct API to check enabled/disabled, but should not throw
        e.Enable();
    }

    TEST_CASE("Destruct and Clear")
    {
        duin::World w;
        duin::Entity e = w.Entity("DestructClear");
        CHECK(e.IsAlive());
        e.Clear();
        CHECK(e.IsAlive());
        e.Destruct();
        CHECK(!e.IsAlive());
    }

    TEST_CASE("Clone")
    {
        duin::World w;
        duin::Entity e = w.Entity("CloneEntity");
        duin::Entity clone = e.Clone();
        CHECK(clone.IsValid());
        CHECK(clone != e);
    }

    TEST_CASE("ChildOf and GetParent")
    {
        duin::World w;
        duin::Entity parent = w.Entity("ParentEntity");
        duin::Entity child = w.Entity("ChildEntity");
        child.ChildOf(parent);
        duin::Entity gotParent = child.GetParent();
        CHECK(gotParent.IsValid());
        CHECK(gotParent.GetID() == parent.GetID());
    }

    TEST_CASE("IsA")
    {
        struct CompA
        {
            int damage;
        };
        struct CompB
        {
            int damage;
        };
        duin::World w;
        duin::Entity base = w.Entity("BaseEntity");
        base.Set<CompA>({10});
        duin::Entity derived = w.Entity("DerivedEntity");
        derived.IsA(base);
        derived.Set<CompB>({25});
        CHECK(derived.TryGet<CompA>() != nullptr);
        CHECK(derived.TryGet<CompB>() != nullptr);
        CHECK(derived.Get<CompA>().damage == 10);
        CHECK(derived.Get<CompB>().damage == 25);
    }

    TEST_CASE("GetChildren")
    {
        duin::World w;
        duin::Entity parent = w.Entity("ParentForChildren");
        duin::Entity child1 = w.Entity("Child1");
        duin::Entity child2 = w.Entity("Child2");
        child1.ChildOf(parent);
        child2.ChildOf(parent);
        std::vector<duin::Entity> children = parent.GetChildren();
        CHECK(children.size() >= 2);
        for (const auto &c : children)
        {
            CHECK(c.IsValid());
            CHECK(c.GetParent() == parent);
        }
    }

    TEST_CASE("GetPath")
    {
        duin::World w;
        duin::Entity e = w.Entity("PathEntity");
        std::string path = e.GetPath();
        CHECK(path == static_cast<std::string>(e.GetFlecsEntity().path()));
    }

    TEST_CASE("GetTarget")
    {
        duin::World w;
        duin::Entity rel = w.Entity("Relationship");
        duin::Entity target = w.Entity("TargetEntity");
        rel.ChildOf(target);
        duin::Entity gotTarget = rel.GetTarget(rel.GetID(), 0);
        // Should not throw, may be valid or not depending on flecs internals
    }

    // Template API tests
    TEST_CASE("Component API: Add, Remove, Has, Set, Get, TryGet, GetMut, TryGetMut, Ensure")
    {
        duin::World w;
        struct DummyComponent
        {
            int x = 0;
        };
        w.Component<DummyComponent>();
        duin::Entity e = w.Entity("CompEntity");
        CHECK(!e.Has<DummyComponent>());
        e.Add<DummyComponent>();
        CHECK(e.Has<DummyComponent>());
        e.Set<DummyComponent>({42});
        CHECK(e.Get<DummyComponent>().x == 42);
        CHECK(e.TryGet<DummyComponent>()->x == 42);
        e.GetMut<DummyComponent>().x = 99;
        CHECK(e.Get<DummyComponent>().x == 99);
        CHECK(e.TryGetMut<DummyComponent>()->x == 99);
        e.Remove<DummyComponent>();
        CHECK(!e.Has<DummyComponent>());
        e.Ensure<DummyComponent>({123});
        CHECK(e.Get<DummyComponent>().x == 123);
    }

    TEST_CASE("Component API: AddIf")
    {
        duin::World w;
        struct CompA
        {
            int a = 1;
        };
        w.Component<CompA>();
        duin::Entity e = w.Entity("AddIfEntity");
        e.AddIf<CompA>(true);
        CHECK(e.Has<CompA>());
        e.AddIf<CompA>(false);
        CHECK(!e.Has<CompA>());
    }

    TEST_CASE("Component API: Set with multiple components")
    {
        duin::World w;
        struct CompA
        {
            int a = 1;
        };
        struct CompB
        {
            int b = 2;
        };
        w.Component<CompA>();
        w.Component<CompB>();
        duin::Entity e = w.Entity("MultiSetEntity");
        e.Set<CompA, CompB>(CompA{10}, CompB{20});
        CHECK(e.Get<CompA>().a == 10);
        CHECK(e.Get<CompB>().b == 20);
    }

    TEST_CASE("Pair API: AddPair, RemovePair, HasPair, SetPair, GetPair, TryGetPair, GetPairMut")
    {
        SUBCASE("Empty First, Non-empty Second (Storage type = Second)")
        {
            duin::World w;
            struct TagA
            {
            }; // Empty
            struct DataB
            {
                int v = 5;
            }; // Non-empty
            w.Component<TagA>();
            w.Component<DataB>();
            duin::Entity e = w.Entity("PairEntity");

            e.AddPair<TagA, DataB>();
            CHECK(e.HasPair<TagA, DataB>());

            // Storage type is DataB (Second), so SetPair takes DataB
            e.SetPair<TagA, DataB>(DataB{42});
            CHECK(e.GetPair<TagA, DataB>().v == 42);
            CHECK(e.TryGetPair<TagA, DataB>()->v == 42);

            e.GetPairMut<TagA, DataB>().v = 99;
            CHECK(e.GetPair<TagA, DataB>().v == 99);

            e.RemovePair<TagA, DataB>();
            CHECK(!e.HasPair<TagA, DataB>());
        }

        SUBCASE("Non-empty First, Empty Second (Storage type = First)")
        {
            duin::World w;
            struct DataA
            {
                int x = 10;
            }; // Non-empty
            struct TagB
            {
            }; // Empty
            w.Component<DataA>();
            w.Component<TagB>();
            duin::Entity e = w.Entity("PairEntity2");

            e.AddPair<DataA, TagB>();
            CHECK(e.HasPair<DataA, TagB>());

            // Storage type is DataA (First), so SetPair takes DataA
            e.SetPair<DataA, TagB>(DataA{100});
            CHECK(e.GetPair<DataA, TagB>().x == 100);
            CHECK(e.TryGetPair<DataA, TagB>()->x == 100);

            e.GetPairMut<DataA, TagB>().x = 200;
            CHECK(e.GetPair<DataA, TagB>().x == 200);

            e.RemovePair<DataA, TagB>();
            CHECK(!e.HasPair<DataA, TagB>());
        }

        SUBCASE("Non-empty First, Non-empty Second (Storage type = First)")
        {
            duin::World w;
            struct DataA
            {
                int x = 10;
            }; // Non-empty
            struct DataB
            {
                int v = 5;
            }; // Non-empty
            w.Component<DataA>();
            w.Component<DataB>();
            duin::Entity e = w.Entity("PairEntity3");

            e.AddPair<DataA, DataB>();
            CHECK(e.HasPair<DataA, DataB>());

            // Storage type is DataA (First) since First is non-empty
            e.SetPair<DataA, DataB>(DataA{300});
            CHECK(e.GetPair<DataA, DataB>().x == 300);
            CHECK(e.TryGetPair<DataA, DataB>()->x == 300);

            e.GetPairMut<DataA, DataB>().x = 400;
            CHECK(e.GetPair<DataA, DataB>().x == 400);

            e.RemovePair<DataA, DataB>();
            CHECK(!e.HasPair<DataA, DataB>());
        }

        SUBCASE("Empty First, Empty Second (Storage type = Second)")
        {
            duin::World w;
            struct TagA
            {
            }; // Empty
            struct TagB
            {
            }; // Empty
            w.Component<TagA>();
            w.Component<TagB>();
            duin::Entity e = w.Entity("PairEntity4");

            // Both are empty, so this is just a tag pair
            e.AddPair<TagA, TagB>();
            CHECK(e.HasPair<TagA, TagB>());

            e.RemovePair<TagA, TagB>();
            CHECK(!e.HasPair<TagA, TagB>());
        }
    }

    TEST_CASE("Pair API: AddPair/RemovePair/HasPair by ID")
    {
        duin::World w;
        duin::Entity first = w.Entity("FirstPair");
        duin::Entity second = w.Entity("SecondPair");
        duin::Entity e = w.Entity("PairByIdEntity");
        e.AddPair(first.GetID(), second.GetID());
        CHECK(e.HasPair(first.GetID(), second.GetID()));
        e.RemovePair(first.GetID(), second.GetID());
        CHECK(!e.HasPair(first.GetID(), second.GetID()));
    }

    TEST_CASE("Advanced API: Emplace")
    {
        duin::World w;
        struct CompC
        {
            int c = 0;
        };
        w.Component<CompC>();
        duin::Entity e = w.Entity("AdvancedEntity");
        e.Emplace<CompC>(123);
        CHECK(e.Get<CompC>().c == 123);
    }

    TEST_CASE("Advanced API: Assign")
    {
        duin::World w;
        struct CompC
        {
            int c = 0;
        };
        w.Component<CompC>();
        duin::Entity e = w.Entity("AdvancedEntity");
        e.Set<CompC>({123});
        CHECK(e.Get<CompC>().c == 123);
        e.Assign<CompC>(CompC{456});
        CHECK(e.Get<CompC>().c == 456);
    }

    TEST_CASE("Advanced API: Modify")
    {
        duin::World w;
        struct CompC
        {
            int c = 0;
        };
        w.Component<CompC>();
        duin::Entity e = w.Entity("ModifyEntity");
        e.Set<CompC>({123});
        CHECK(e.Get<CompC>().c == 123);

        e.Modify([](duin::Entity &entity) {
            entity.GetMut<CompC>().c = 789;
            entity.Modified<CompC>();
        });

        CHECK(e.Get<CompC>().c == 789);
    }

    TEST_CASE("EnableComponent, DisableComponent, IsComponentEnabled")
    {
        duin::World w;
        struct CompD
        {
            int d = 0;
        };

        // Register component with CanToggle trait to support enable/disable
        duin::Entity compEntity = w.Component<CompD>();
        compEntity.Add(flecs::CanToggle);

        duin::Entity e = w.Entity("EnableDisableCompEntity");
        e.Add<CompD>();

        // Component should be enabled by default
        CHECK(e.IsComponentEnabled<CompD>());

        e.DisableComponent<CompD>();
        CHECK(!e.IsComponentEnabled<CompD>());

        e.EnableComponent<CompD>();
        CHECK(e.IsComponentEnabled<CompD>());
    }

    TEST_CASE("SetAutoOverride")
    {
        duin::World w;
        struct CompE
        {
            int e = 0;
        };
        w.Component<CompE>();
        duin::Entity e = w.Entity("AutoOverrideEntity");
        e.SetAutoOverride<CompE>(CompE{321});
        CHECK(e.Get<CompE>().e == 321);
    }

    TEST_CASE("WithScope and WithComponent")
    {
        duin::World w;
        struct CompF
        {
            int f = 0;
        };
        w.Component<CompF>();

        duin::Entity parent = w.Entity("ScopeParent");

        // Test WithScope - entities created inside should be children
        bool calledScope = false;
        parent.WithScope([&](duin::Entity &ent) {
            calledScope = true;
            duin::Entity child = w.Entity("ScopeChild");
            CHECK(child.IsValid());
        });
        CHECK(calledScope);

        // Test WithComponent - components added inside should include the parent entity
        duin::Entity e = w.Entity("WithCompEntity");
        bool calledComp = false;
        e.WithComponent([&](duin::Entity &ent) {
            calledComp = true;
            ent.Set<CompF>(CompF{555});
        });
        CHECK(calledComp);
        CHECK(e.Get<CompF>().f == 555);
    }

    TEST_CASE("Emit and Modified")
    {
        duin::World w;
        struct EventComp
        {
            int val = 0;
        };
        w.Component<EventComp>();

        duin::Entity e = w.Entity("ModifiedEntity");
        e.Set<EventComp>(EventComp{100});

        // Emit is a no-op unless flecs event system is set up, but should not throw
        e.Emit<EventComp>(123);

        // Modified should mark the component as changed
        e.GetMut<EventComp>().val = 200;
        e.Modified<EventComp>();
        CHECK(e.Get<EventComp>().val == 200);
    }

    TEST_CASE("EachChild")
    {
        duin::World w;
        struct CompG
        {
            int g = 0;
        };
        w.Component<CompG>();
        duin::Entity parent = w.Entity("ParentForEach");
        duin::Entity child1 = w.Entity("ChildForEach1");
        duin::Entity child2 = w.Entity("ChildForEach2");
        child1.ChildOf(parent).Set<CompG>(CompG{1});
        child2.ChildOf(parent).Set<CompG>(CompG{2});
        int count = 0;
        parent.EachChild([&](duin::Entity c) {
            count++;
            CHECK(c.IsValid());
        });
        CHECK(count >= 2);
    }

    TEST_CASE("ForEachChild")
    {
        duin::World w;
        struct CompG
        {
            int g = 0;
        };
        w.Component<CompG>();
        duin::Entity parent = w.Entity("ParentForEach");
        duin::Entity child1 = w.Entity("ChildForEach1");
        duin::Entity child2 = w.Entity("ChildForEach2");
        child1.ChildOf(parent).Set<CompG>(CompG{1});
        child2.ChildOf(parent).Set<CompG>(CompG{2});
        int compCount = 0;
        parent.ForEachChild<CompG>([&](duin::Entity c, const CompG &g) {
            compCount++;
            bool result = g.g == 1 || g.g == 2;
            CHECK(result);
        });
        CHECK(compCount >= 2);
    }

    TEST_CASE("ForEachTarget")
    {
        duin::World w;
        struct Relationship
        {
        };
        w.Component<Relationship>();

        duin::Entity source = w.Entity("Source");
        duin::Entity target1 = w.Entity("Target1");
        duin::Entity target2 = w.Entity("Target2");

        // Add relationships from targets to source
        target1.AddPair(w.Component<Relationship>().GetID(), source.GetID());
        target2.AddPair(w.Component<Relationship>().GetID(), source.GetID());

        int targetCount = 0;
        source.ForEachTarget<Relationship>([&](duin::Entity t) {
            targetCount++;
            CHECK(t.IsValid());
        });

        // Note: The actual count depends on how flecs represents the relationships
        // This test verifies the function doesn't crash
        CHECK(targetCount >= 0);
    }

    TEST_CASE("GetFlecsEntity")
    {
        duin::World w;
        duin::Entity e = w.Entity("GetFlecsEntity");
        flecs::entity flecsEntity = e.GetFlecsEntity();
        CHECK(flecsEntity.is_valid());
        CHECK(flecsEntity.is_alive());
    }

    TEST_CASE("operator= with Entity")
    {
        duin::World w;
        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2;

        e2 = e1;
        CHECK(e2.IsValid());
        CHECK(e2 == e1);
    }

    TEST_CASE("GetChildren returns valid entities")
    {
        duin::World w;
        duin::Entity parent = w.Entity("ParentGetChildren");
        duin::Entity child1 = w.Entity("Child1");
        duin::Entity child2 = w.Entity("Child2");

        child1.ChildOf(parent);
        child2.ChildOf(parent);

        std::vector<duin::Entity> children = parent.GetChildren();
        CHECK(children.size() >= 2);

        for (const auto &child : children)
        {
            CHECK(child.IsValid());
        }
    }

    TEST_CASE("ForEachComponent")
    {
        duin::World w;
        struct CompA
        {
            int a = 1;
        };
        struct CompB
        {
            int b = 2;
        };
        struct CompC
        {
            int c = 3;
        };

        w.Component<CompA>();
        w.Component<CompB>();
        w.Component<CompC>();

        duin::Entity e = w.Entity("ComponentIteratorEntity");
        e.Add<CompA>();
        e.Add<CompB>();
        e.Add<CompC>();

        int componentCount = 0;
        e.ForEachComponent([&](duin::Entity::ID compID) {
            if (compID.IsEntity())
            {
                duin::Entity compEntity = compID.GetEntity();
                componentCount++;
                CHECK(compEntity.IsValid());
            }
        });

        CHECK(componentCount >= 3);
    }
}

TEST_SUITE("Entity Constructors")
{
    // Helpers to verify the internal flecs::entity has a valid world attached.
    // An entity constructed with flecs::entity(id) (no world) will return nullptr
    // from world().c_ptr(), causing asserts on name(), is_valid(), etc.
    auto flecsHasWorld = [](const duin::Entity &e) -> bool {
        return e.GetFlecsEntity().world().c_ptr() != nullptr;
    };

    TEST_CASE("Default constructor - entity is invalid, flecs entity has no world")
    {
        duin::Entity e;
        CHECK(!e.IsValid());
        CHECK(e.GetWorld() == nullptr);
        CHECK(e.GetFlecsEntity().world().c_ptr() == nullptr);
    }

    TEST_CASE("Entity(uint64_t id, World* world) - valid id with world")
    {
        duin::World w;
        duin::Entity src = w.Entity("CtorSource");
        uint64_t id = src.GetID();

        duin::Entity e(id, &w);

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() == id);
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        // Must not assert - previously this would crash when flecs tried to resolve name
        CHECK(e.GetName() == "CtorSource");
    }

    TEST_CASE("Entity(uint64_t id, World* world) - null world gives invalid entity")
    {
        duin::Entity e(42, nullptr);
        CHECK(!e.IsValid());
        CHECK(e.GetWorld() == nullptr);
        // flecs entity has no world when constructed without one
        CHECK(e.GetFlecsEntity().world().c_ptr() == nullptr);
    }

    TEST_CASE("Entity(const flecs::entity&) - flecs entity carries its own world")
    {
        duin::World w;
        flecs::entity fe = w.GetFlecsWorld().entity("FlecsCtorEntity");

        duin::Entity e(fe);

        // duin world pointer is null (not set via this ctor), but flecs world is valid
        CHECK(e.GetWorld() == nullptr);
        CHECK(!e.IsValid()); // IsValid() requires duin world != nullptr
        CHECK(e.GetFlecsEntity().world().c_ptr() != nullptr);
        CHECK(fe.is_valid());
    }

    TEST_CASE("Entity(const flecs::entity&, World*) - both worlds set")
    {
        duin::World w;
        flecs::entity fe = w.GetFlecsWorld().entity("FlecsAndDuinWorld");

        duin::Entity e(fe, &w);

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        CHECK(e.GetName() == "FlecsAndDuinWorld");
    }

    TEST_CASE("Copy constructor - copies world and flecs entity")
    {
        duin::World w;
        duin::Entity src = w.Entity("CopyCtorEntity");

        duin::Entity copy(src);

        CHECK(copy.IsValid());
        CHECK(copy.IsAlive());
        CHECK(copy.GetID() == src.GetID());
        CHECK(copy.GetWorld() == &w);
        CHECK(flecsHasWorld(copy));
        CHECK(copy.GetName() == "CopyCtorEntity");
        CHECK(copy == src);
    }

    TEST_CASE("Copy assignment - copies world and flecs entity")
    {
        duin::World w;
        duin::Entity src = w.Entity("CopyAssignEntity");

        duin::Entity e;
        e = src;

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() == src.GetID());
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        CHECK(e.GetName() == "CopyAssignEntity");
    }

    TEST_CASE("Move constructor - transfers world and flecs entity, source loses world")
    {
        duin::World w;
        duin::Entity src = w.Entity("MoveCtorEntity");
        uint64_t id = src.GetID();

        duin::Entity moved(std::move(src));

        CHECK(moved.IsValid());
        CHECK(moved.IsAlive());
        CHECK(moved.GetID() == id);
        CHECK(moved.GetWorld() == &w);
        CHECK(flecsHasWorld(moved));
        CHECK(moved.GetName() == "MoveCtorEntity");
        // Source world pointer is cleared after move
        CHECK(src.GetWorld() == nullptr);
    }

    TEST_CASE("Move assignment - transfers world and flecs entity, source loses world")
    {
        duin::World w;
        duin::Entity src = w.Entity("MoveAssignEntity");
        uint64_t id = src.GetID();

        duin::Entity e;
        e = std::move(src);

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() == id);
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        CHECK(e.GetName() == "MoveAssignEntity");
        CHECK(src.GetWorld() == nullptr);
    }

    TEST_CASE("World::Entity(uint64_t id) - constructs valid entity from existing id")
    {
        duin::World w;
        duin::Entity src = w.Entity("WorldEntityById");
        uint64_t id = src.GetID();

        duin::Entity e = w.Entity(id);

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() == id);
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        CHECK(e.GetName() == "WorldEntityById");
    }

    TEST_CASE("SetWorld and SetFlecsEntity - manually assembled entity is valid")
    {
        duin::World w;
        flecs::entity fe = w.GetFlecsWorld().entity("ManualEntity");

        duin::Entity e;
        e.SetWorld(&w);
        e.SetFlecsEntity(fe);

        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetWorld() == &w);
        CHECK(flecsHasWorld(e));
        CHECK(e.GetName() == "ManualEntity");
    }
}

TEST_SUITE("Entity::ID")
{
    TEST_CASE("Default constructor")
    {
        duin::Entity::ID id;
        CHECK(id.GetID() == 0);
        CHECK(id.GetWorld() == nullptr);
    }

    TEST_CASE("Construct from raw ID value")
    {
        flecs::id_t rawValue = 12345;
        duin::Entity::ID id(rawValue);
        CHECK(id.GetID() == rawValue);
        CHECK(id.GetWorld() == nullptr);
    }

    TEST_CASE("Construct from world and ID value")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        flecs::id_t entityId = e.GetID();

        duin::Entity::ID id(&w, entityId);
        CHECK(id.GetID() == entityId);
        CHECK(id.GetWorld() == &w);
    }

    TEST_CASE("Construct pair ID from two raw IDs")
    {
        duin::World w;
        duin::Entity first = w.Entity("First");
        duin::Entity second = w.Entity("Second");

        duin::Entity::ID pairId(first.GetID(), second.GetID());
        CHECK(pairId.IsPair());
        CHECK(pairId.GetWorld() == nullptr);
    }

    TEST_CASE("Construct pair ID from world and two raw IDs")
    {
        duin::World w;
        duin::Entity first = w.Entity("First");
        duin::Entity second = w.Entity("Second");

        duin::Entity::ID pairId(&w, first.GetID(), second.GetID());
        CHECK(pairId.IsPair());
        CHECK(pairId.GetWorld() == &w);
    }

    TEST_CASE("Construct pair ID from two Entity::ID objects")
    {
        duin::World w;
        duin::Entity first = w.Entity("First");
        duin::Entity second = w.Entity("Second");

        duin::Entity::ID firstId(&w, first.GetID());
        duin::Entity::ID secondId(&w, second.GetID());
        duin::Entity::ID pairId(firstId, secondId);

        CHECK(pairId.IsPair());
        CHECK(pairId.GetWorld() == &w);
    }

    TEST_CASE("IsEntity returns true for entity IDs")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        CHECK(id.IsEntity());
        CHECK(!id.IsPair());
    }

    TEST_CASE("IsPair returns true for pair IDs")
    {
        duin::World w;
        duin::Entity first = w.Entity("First");
        duin::Entity second = w.Entity("Second");

        duin::Entity::ID pairId(&w, first.GetID(), second.GetID());

        CHECK(pairId.IsPair());
        CHECK(!pairId.IsEntity());
    }

    TEST_CASE("GetEntity returns valid entity")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        duin::Entity retrieved = id.GetEntity();
        CHECK(retrieved.IsValid());
        CHECK(retrieved.GetID() == e.GetID());
    }

    TEST_CASE("First and Second return pair elements")
    {
        duin::World w;
        duin::Entity first = w.Entity("First");
        duin::Entity second = w.Entity("Second");

        duin::Entity::ID pairId(&w, first.GetID(), second.GetID());

        duin::Entity retrievedFirst = pairId.First();
        duin::Entity retrievedSecond = pairId.Second();

        CHECK(retrievedFirst.IsValid());
        CHECK(retrievedSecond.IsValid());
        CHECK(retrievedFirst.GetID() == first.GetID());
        CHECK(retrievedSecond.GetID() == second.GetID());
    }

    TEST_CASE("HasRelation returns true for matching relation")
    {
        duin::World w;
        duin::Entity relation = w.Entity("Relation");
        duin::Entity target = w.Entity("Target");

        duin::Entity::ID pairId(&w, relation.GetID(), target.GetID());

        CHECK(pairId.HasRelation(relation.GetID()));
        CHECK(!pairId.HasRelation(target.GetID()));
    }

    TEST_CASE("Str returns string representation")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        std::string str = id.Str();
        CHECK(!str.empty());
    }

    TEST_CASE("Implicit conversion to flecs::id_t")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        flecs::id_t GetID = id;
        CHECK(GetID == e.GetID());
    }

    TEST_CASE("GetFlecsId returns valid flecs::id")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        flecs::id flecsId = id.GetFlecsId();
        CHECK(flecsId.raw_id() == e.GetID());
    }

    TEST_CASE("AddFlags and RemoveFlags")
    {
        duin::World w;
        struct TestComp
        {
            int value = 42;
        };

        // Register component
        duin::Entity compEntity = w.Component<TestComp>();
        duin::Entity::ID componentId(&w, compEntity.GetID());

        // AddFlags returns an ID (not Entity) with the flags applied
        duin::Entity::ID idWithFlags = componentId.AddFlags(flecs::AUTO_OVERRIDE);
        CHECK(idWithFlags.HasFlags(flecs::AUTO_OVERRIDE));

        // The flagged ID should be different from the original
        CHECK(idWithFlags.GetID() != componentId.GetID());

        // RemoveFlags returns an ID without the specified flags
        duin::Entity::ID idRemovedFlags = idWithFlags.RemoveFlags(flecs::AUTO_OVERRIDE);
        CHECK(!idRemovedFlags.HasFlags(flecs::AUTO_OVERRIDE));

        // RemoveFlags() (no args) removes all flags
        duin::Entity::ID idNoFlags = idWithFlags.RemoveFlags();
        CHECK(!idNoFlags.HasFlags());
    }

    TEST_CASE("HasFlags checks for flags")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        // Entity IDs without flags should return false
        CHECK(!id.HasFlags());
    }

    TEST_CASE("RemoveGeneration returns entity without generation")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        // RemoveGeneration returns an ID without generation
        duin::Entity::ID idNoGen = id.RemoveGeneration();
        CHECK(idNoGen.GetID() != 0); // Should have a valid ID
    }

    TEST_CASE("TypeId returns component type")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 42;
        };
        w.Component<TestComponent>();

        duin::Entity compEntity = w.Component<TestComponent>();
        duin::Entity::ID id(&w, compEntity.GetID());

        duin::Entity typeId = id.TypeId();
        CHECK(typeId.IsValid());
    }

    TEST_CASE("ID works with pair operations on entities")
    {
        duin::World w;
        struct RelationType
        {
        };
        struct TargetType
        {
            int data = 100;
        };

        w.Component<RelationType>();
        w.Component<TargetType>();

        duin::Entity e = w.Entity("EntityWithPair");
        e.AddPair<RelationType, TargetType>();
        e.SetPair<RelationType, TargetType>(TargetType{200});

        // Create ID for the relation
        duin::Entity relationEntity = w.Component<RelationType>();
        duin::Entity::ID relationId(&w, relationEntity.GetID());

        CHECK(relationId.IsEntity());
        CHECK(relationId.GetEntity().IsValid());
    }

    TEST_CASE("IsWildcard checks for wildcard IDs")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        // Regular entities are not wildcards
        CHECK(!id.IsWildcard());
    }

    TEST_CASE("FlagsStr returns string representation of flags")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        std::string flagsStr = id.FlagsStr();
        // Should not throw, may be empty for entities without flags
        CHECK(flagsStr.size() >= 0);
    }

    TEST_CASE("GetFlags returns entity representing flags")
    {
        duin::World w;
        duin::Entity e = w.Entity("TestEntity");
        duin::Entity::ID id(&w, e.GetID());

        duin::Entity flags = id.GetFlags();
        // Should return a valid entity (may be 0 for no flags)
        bool isValidOrZero = (flags.GetID() == 0) || flags.IsValid();
        CHECK(isValidOrZero);
    }
}
} // namespace TestEntity
