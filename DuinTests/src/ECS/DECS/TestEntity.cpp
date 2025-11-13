#include "doctest.h"
#include "TestEntity.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <string>
#include <vector>

TEST_SUITE("Entity") {

    TEST_CASE("Default constructor and destructor") {
        duin::Entity e;
        CHECK(!e.IsValid());
    }

    TEST_CASE("CreateEntity, IsValid, IsAlive, GetID, GetWorld") {
        duin::World w;
        duin::Entity e = w.CreateEntity("TestEntity");
        CHECK(e.IsValid());
        CHECK(e.IsAlive());
        CHECK(e.GetID() != 0);
        CHECK(e.GetWorld() == &w);
    }

    TEST_CASE("operator== and operator!=") {
        duin::World w;
        duin::Entity e1 = w.CreateEntity("Entity1");
        duin::Entity e2 = w.CreateEntity("Entity2");
        duin::Entity e3 = e1;
        CHECK(e1 == e3);
        CHECK(e1 != e2);
    }

    TEST_CASE("explicit operator bool") {
        duin::World w;
        duin::Entity e = w.CreateEntity("EntityBool");
        CHECK(static_cast<bool>(e));
        w.DeleteEntity(e);
        CHECK(!static_cast<bool>(e));
    }

    TEST_CASE("GetName and SetName") {
        duin::World w;
        duin::Entity e = w.CreateEntity("OriginalName");
        CHECK(e.GetName() == "OriginalName");
        e.SetName("NewName");
        CHECK(e.GetName() == "NewName");
    }

    TEST_CASE("Enable and Disable") {
        duin::World w;
        duin::Entity e = w.CreateEntity("EnableDisable");
        e.Disable();
        // No direct API to check enabled/disabled, but should not throw
        e.Enable();
    }

    TEST_CASE("Destruct and Clear") {
        duin::World w;
        duin::Entity e = w.CreateEntity("DestructClear");
        CHECK(e.IsAlive());
        e.Clear();
        CHECK(e.IsAlive());
        e.Destruct();
        CHECK(!e.IsAlive());
    }

    TEST_CASE("Clone") {
        duin::World w;
        duin::Entity e = w.CreateEntity("CloneEntity");
        duin::Entity clone = e.Clone();
        CHECK(clone.IsValid());
        CHECK(clone != e);
    }

    TEST_CASE("ChildOf and GetParent") {
        duin::World w;
        duin::Entity parent = w.CreateEntity("ParentEntity");
        duin::Entity child = w.CreateEntity("ChildEntity");
        child.ChildOf(parent);
        duin::Entity gotParent = child.GetParent();
        CHECK(gotParent.IsValid());
        CHECK(gotParent.GetID() == parent.GetID());
    }

    TEST_CASE("IsA") {
        duin::World w;
        duin::Entity base = w.CreateEntity("BaseEntity");
        duin::Entity derived = w.CreateEntity("DerivedEntity");
        derived.IsA(base);
        // No direct API to check IsA, but should not throw
    }

    TEST_CASE("GetChildren") {
        duin::World w;
        duin::Entity parent = w.CreateEntity("ParentForChildren");
        duin::Entity child1 = w.CreateEntity("Child1");
        duin::Entity child2 = w.CreateEntity("Child2");
        child1.ChildOf(parent);
        child2.ChildOf(parent);
        std::vector<duin::Entity> children = parent.GetChildren();
        CHECK(children.size() >= 2);
        for (const auto& c : children) {
            CHECK(c.IsValid());
            CHECK(c.GetParent() == parent);
        }
    }

    TEST_CASE("GetPath") {
        duin::World w;
        duin::Entity e = w.CreateEntity("PathEntity");
        std::string path = e.GetPath();
        CHECK(path.empty()); // Implementation is TODO, so should be empty
    }

    TEST_CASE("GetTarget") {
        duin::World w;
        duin::Entity rel = w.CreateEntity("Relationship");
        duin::Entity target = w.CreateEntity("TargetEntity");
        rel.ChildOf(target);
        duin::Entity gotTarget = rel.GetTarget(rel.GetID(), 0);
        // Should not throw, may be valid or not depending on flecs internals
    }

    // Template API tests
    TEST_CASE("Component API: Add, Remove, Has, Set, Get, TryGet, GetMut, TryGetMut, Ensure") {
        duin::World w;
        struct DummyComponent { int x = 0; };
        w.Component<DummyComponent>();
        duin::Entity e = w.CreateEntity("CompEntity");
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

    TEST_CASE("Component API: AddIf") {
        duin::World w;
        struct CompA { int a = 1; };
        w.Component<CompA>();
        duin::Entity e = w.CreateEntity("AddIfEntity");
        e.AddIf<CompA>(true);
        CHECK(e.Has<CompA>());
        e.AddIf<CompA>(false);
        CHECK(!e.Has<CompA>());
    }

    TEST_CASE("Component API: Set with multiple components") {
        duin::World w;
        struct CompA { int a = 1; };
        struct CompB { int b = 2; };
        w.Component<CompA>();
        w.Component<CompB>();
        duin::Entity e = w.CreateEntity("MultiSetEntity");
        e.Set<CompA, CompB>(CompA{10}, CompB{20});
        CHECK(e.Get<CompA>().a == 10);
        CHECK(e.Get<CompB>().b == 20);
    }

    TEST_CASE("Pair API: AddPair, RemovePair, HasPair, SetPair, GetPair, TryGetPair, GetPairMut") {
        duin::World w;
        struct TagA {};
        struct DataB { int v = 5; };
        w.Component<TagA>();
        w.Component<DataB>();
        duin::Entity e = w.CreateEntity("PairEntity");
        e.AddPair<TagA, DataB>();
        CHECK(e.HasPair<TagA, DataB>());
        e.SetPair<TagA, DataB>(DataB{42});
        CHECK(e.GetPair<TagA, DataB>().v == 42);
        CHECK(e.TryGetPair<TagA, DataB>()->v == 42);
        e.GetPairMut<TagA, DataB>().v = 99;
        CHECK(e.GetPair<TagA, DataB>().v == 99);
        e.RemovePair<TagA, DataB>();
        CHECK(!e.HasPair<TagA, DataB>());
    }

    TEST_CASE("Pair API: AddPair/RemovePair/HasPair by ID") {
        duin::World w;
        duin::Entity first = w.CreateEntity("FirstPair");
        duin::Entity second = w.CreateEntity("SecondPair");
        duin::Entity e = w.CreateEntity("PairByIdEntity");
        e.AddPair(first.GetID(), second.GetID());
        CHECK(e.HasPair(first.GetID(), second.GetID()));
        e.RemovePair(first.GetID(), second.GetID());
        CHECK(!e.HasPair(first.GetID(), second.GetID()));
    }

    TEST_CASE("Advanced API: Emplace") {
        duin::World w;
        struct CompC { int c = 0; };
        w.Component<CompC>();
        duin::Entity e = w.CreateEntity("AdvancedEntity");
        e.Emplace<CompC>(123);
        CHECK(e.Get<CompC>().c == 123);
    }

    TEST_CASE("Advanced API: Assign") {
        duin::World w;
        struct CompC { int c = 0; };
        w.Component<CompC>();
        duin::Entity e = w.CreateEntity("AdvancedEntity");
        e.Set<CompC>({ 123 });
        CHECK(e.Get<CompC>().c == 123);
        e.Assign<CompC>(CompC{ 456 });
        CHECK(e.Get<CompC>().c == 456);
    }

    TEST_CASE("Advanced API: Modify") {
        // TODO
        //duin::World w;
        //struct CompC { int c = 0; };
        //w.Component<CompC>();
        //duin::Entity e = w.CreateEntity("AdvancedEntity");
        //e.Set<CompC>({ 123 });
        //CHECK(e.Get<CompC>().c == 123);
        //e.Modify([](auto& entity) {
        //    entity.Set<CompC>(CompC{ 789 });
        //});
        //CHECK(e.Get<CompC>().c == 789);
        WARN(false);
	}

    TEST_CASE("EnableComponent, DisableComponent, IsComponentEnabled") {
		// TODO
        //duin::World w;
        //struct CompD { int d = 0; };
        //w.Component<CompD>();
        //duin::Entity e = w.CreateEntity("EnableDisableCompEntity");
        //e.Add<CompD>();
        //e.DisableComponent<CompD>();
        //CHECK(!e.IsComponentEnabled<CompD>());
        //e.EnableComponent<CompD>();
        //CHECK(e.IsComponentEnabled<CompD>());
		WARN(false);
    }

    TEST_CASE("SetAutoOverride") {
        duin::World w;
        struct CompE { int e = 0; };
        w.Component<CompE>();
        duin::Entity e = w.CreateEntity("AutoOverrideEntity");
        e.SetAutoOverride<CompE>(CompE{321});
        CHECK(e.Get<CompE>().e == 321);
    }

    TEST_CASE("WithScope and WithComponent") {
        // TODO
        //duin::World w;
        //struct CompF { int f = 0; };
        //w.Component<CompF>();
        //duin::Entity e = w.CreateEntity("ScopeCompEntity");
        //bool calledScope = false;
        //e.WithScope([&](duin::Entity& ent){
        //    calledScope = true;
        //    ent.Add<CompF>();
        //});
        //CHECK(calledScope);
        //bool calledComp = false;
        //e.WithComponent([&](duin::Entity& ent){
        //    calledComp = true;
        //    ent.Set<CompF>(CompF{555});
        //});
        //CHECK(calledComp);
        //CHECK(e.Get<CompF>().f == 555);
		WARN(false);
    }

    TEST_CASE("Emit and Modified") {
        // TODO
        //duin::World w;
        //struct EventA { int val; };
        //w.Component<EventA>();
        //duin::Entity e = w.CreateEntity("EmitEntity");
        //// Emit is a no-op unless flecs event system is set up, but should not throw
        //e.Emit<EventA>(123);
        //e.Modified<EventA>();
        //e.Modified();
        WARN(false);
    }

    TEST_CASE("EachChild") {
        duin::World w;
        struct CompG { int g = 0; };
        w.Component<CompG>();
        duin::Entity parent = w.CreateEntity("ParentForEach");
        duin::Entity child1 = w.CreateEntity("ChildForEach1");
        duin::Entity child2 = w.CreateEntity("ChildForEach2");
        child1.ChildOf(parent).Set<CompG>(CompG{ 1 });
        child2.ChildOf(parent).Set<CompG>(CompG{ 2 });
        int count = 0;
        parent.EachChild([&](duin::Entity c) {
            count++;
            CHECK(c.IsValid());
        });
        CHECK(count >= 2);
    }

    TEST_CASE("ForEachChild") {
        duin::World w;
        struct CompG { int g = 0; };
        w.Component<CompG>();
        duin::Entity parent = w.CreateEntity("ParentForEach");
        duin::Entity child1 = w.CreateEntity("ChildForEach1");
        duin::Entity child2 = w.CreateEntity("ChildForEach2");
        child1.ChildOf(parent).Set<CompG>(CompG{ 1 });
        child2.ChildOf(parent).Set<CompG>(CompG{ 2 });
        int compCount = 0;
        parent.ForEachChild<CompG>([&](duin::Entity c, const CompG& g) {
            compCount++;
            bool result = g.g == 1 || g.g == 2;
            CHECK(result);
        });
        CHECK(compCount >= 2);
    }

    TEST_CASE("ForEachTarget") {
        // TODO
        //duin::World w;
        //duin::Entity rel = w.CreateEntity("RelForEach");
        //duin::Entity target = w.CreateEntity("TargetForEach");
        //rel.ChildOf(target);
        //int targetCount = 0;
        //rel.ForEachTarget<duin::Entity>([&](duin::Entity t) {
        //    targetCount++;
        //    CHECK(t.IsValid());
        //});
        //CHECK(targetCount >= 1);
		WARN(false);
    }
}