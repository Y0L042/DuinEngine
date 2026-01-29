#include "doctest.h"
#include "TestQuery.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/Query.h>
#include <map>
#include <vector>
#include <string>

namespace TestQuery
{
TEST_SUITE("Query")
{
    TEST_CASE("each method iterates over entities with components")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        // Create entities with components
        duin::Entity e1 = w.CreateEntity("Entity1");
        duin::Entity e2 = w.CreateEntity("Entity2");
        duin::Entity e3 = w.CreateEntity("Entity3");
        e1.Set<TestComponent>({10});
        e2.Set<TestComponent>({20});
        e3.Set<TestComponent>({30});

        // Create query
        duin::Query q = w.QueryBuilder<TestComponent>().Cached().Build();

        // Test each method - now uses duin::Entity
        int count = 0;
        int totalValue = 0;
        q.Each([&](duin::Entity entity, TestComponent &comp) {
            count++;
            totalValue += comp.value;
        });

        CHECK(count == 3);
        CHECK(totalValue == 60); // 10 + 20 + 30
    }

    TEST_CASE("each method with multiple components")
    {
        duin::World w;
        struct CompA
        {
            int a = 0;
        };
        struct CompB
        {
            float b = 0.0f;
        };
        w.Component<CompA>();
        w.Component<CompB>();

        // Create entities with both components
        duin::Entity e1 = w.CreateEntity("Entity1");
        duin::Entity e2 = w.CreateEntity("Entity2");
        e1.Set<CompA>({5}).Set<CompB>({1.5f});
        e2.Set<CompA>({10}).Set<CompB>({2.5f});

        // Create entity with only one component (should not be included)
        duin::Entity e3 = w.CreateEntity("Entity3");
        e3.Set<CompA>({15});

        // Create query for both components
        auto flecsQuery = w.GetFlecsWorld().query<CompA, CompB>();
        duin::Query q = w.QueryBuilder<CompA, CompB>().Build();

        // Test each method - now uses duin::Entity
        int count = 0;
        int totalA = 0;
        float totalB = 0.0f;
        q.Each([&](duin::Entity entity, CompA &compA, CompB &compB) {
            count++;
            totalA += compA.a;
            totalB += compB.b;
        });

        CHECK(count == 2);                      // Only e1 and e2 have both components
        CHECK(totalA == 15);                    // 5 + 10
        CHECK(totalB == doctest::Approx(4.0f)); // 1.5 + 2.5
    }

    TEST_CASE("Query with no matching entities")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        // Use QueryBuilder for consistency
        auto q = w.QueryBuilder<TestComponent>().Build();

        // Test each method with no entities
        int count = 0;
        q.Each([&](duin::Entity entity, TestComponent &comp) { count++; });

        CHECK(count == 0);
    }

    TEST_CASE("Query operations are const-correct")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e = w.CreateEntity("TestEntity");
        e.Set<TestComponent>({42});

        // Explicitly specify template parameters
        const duin::Query q = w.QueryBuilder<TestComponent>().Build();

        // All query operations should work on const query
        const auto &underlyingQuery = q.GetFlecsQuery<TestComponent>();
        CHECK(typeid(underlyingQuery) == typeid(flecs::query<TestComponent>));

        int count = 0;
        q.Each([&](duin::Entity entity, TestComponent &comp) {
            count++;
            CHECK(comp.value == 42);
        });
        CHECK(count == 1);
    }

    TEST_CASE("Query with parent-child hierarchy")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        w.Component<Position>();

        // Create parent-child hierarchy
        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity child1 = w.CreateEntity("Child1");
        duin::Entity child2 = w.CreateEntity("Child2");
        duin::Entity grandchild = w.CreateEntity("Grandchild");

        parent.Set<Position>({0.0f, 0.0f});
        child1.Set<Position>({10.0f, 10.0f}).ChildOf(parent);
        child2.Set<Position>({20.0f, 20.0f}).ChildOf(parent);
        grandchild.Set<Position>({5.0f, 5.0f}).ChildOf(child1);

        // Query for all entities with Position
        auto q = w.QueryBuilder<Position>().Build();

        int count = 0;
        q.Each([&](duin::Entity entity, Position &pos) {
            count++;
            CHECK(entity.IsValid());
        });

        // Should find all entities with Position (parent, 2 children, 1 grandchild)
        CHECK(count == 4);
    }

    TEST_CASE("Query with parent component using cascade")
    {
        duin::World w;
        struct Transform
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        w.Component<Transform>();

        // Create parent-child hierarchy
        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity child1 = w.CreateEntity("Child1");
        duin::Entity child2 = w.CreateEntity("Child2");

        parent.Set<Transform>({100.0f, 200.0f});
        child1.Set<Transform>({10.0f, 20.0f}).ChildOf(parent);
        child2.Set<Transform>({30.0f, 40.0f}).ChildOf(parent);

        // Build query with parent component access using cascade
        auto flecsQuery =
            w.GetFlecsWorld().query_builder<Transform, const Transform *>().term_at(1).parent().cascade().build();

        // Test that children can access parent transform
        int childrenWithParent = 0;
        flecsQuery.each([&](flecs::entity e, Transform &childTransform, const Transform *parentTransform) {
            if (parentTransform)
            {
                // This is a child entity with a parent
                childrenWithParent++;
                CHECK(parentTransform->x == 100.0f);
                CHECK(parentTransform->y == 200.0f);
            }
        });

        CHECK(childrenWithParent == 2); // child1 and child2
    }

    TEST_CASE("Query filtering by parent relationship")
    {
        duin::World w;
        struct Tag
        {
        };
        w.Component<Tag>();

        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity child1 = w.CreateEntity("Child1");
        duin::Entity child2 = w.CreateEntity("Child2");
        duin::Entity orphan = w.CreateEntity("Orphan");

        parent.Add<Tag>();
        child1.Add<Tag>().ChildOf(parent);
        child2.Add<Tag>().ChildOf(parent);
        orphan.Add<Tag>();

        // Query all entities with Tag
        auto q = w.QueryBuilder<>().With<Tag>().Build();

        std::vector<std::string> names;
        q.Each([&](duin::Entity entity) { names.push_back(entity.GetName()); });

        // Should find all 4 entities
        CHECK(names.size() == 4);
    }

    TEST_CASE("Query with Tag filter using With")
    {
        duin::World w;
        struct Health
        {
            int hp = 100;
        };
        struct Enemy
        {
        };
        struct Boss
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();
        w.Component<Boss>();

        duin::Entity player = w.CreateEntity("Player");
        duin::Entity minion1 = w.CreateEntity("Minion1");
        duin::Entity minion2 = w.CreateEntity("Minion2");
        duin::Entity bossEnemy = w.CreateEntity("BossEnemy");

        player.Set<Health>({100});
        minion1.Set<Health>({50}).Add<Enemy>();
        minion2.Set<Health>({50}).Add<Enemy>();
        bossEnemy.Set<Health>({500}).Add<Enemy>().Add<Boss>();

        // Query for Health with Enemy tag
        auto q = w.QueryBuilder<Health>().With<Enemy>().Build();

        int enemyCount = 0;
        int totalHp = 0;
        q.Each([&](duin::Entity entity, Health &health) {
            enemyCount++;
            totalHp += health.hp;
        });

        CHECK(enemyCount == 3); // All enemies (2 minions + 1 boss)
        CHECK(totalHp == 600);  // 50 + 50 + 500
    }

    TEST_CASE("Query with multiple Tag filters")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        struct Enemy
        {
        };
        struct Flying
        {
        };
        struct Ranged
        {
        };
        w.Component<Position>();
        w.Component<Enemy>();
        w.Component<Flying>();
        w.Component<Ranged>();

        duin::Entity groundMelee = w.CreateEntity("GroundMelee");
        duin::Entity groundRanged = w.CreateEntity("GroundRanged");
        duin::Entity flyingMelee = w.CreateEntity("FlyingMelee");
        duin::Entity flyingRanged = w.CreateEntity("FlyingRanged");

        groundMelee.Set<Position>({0.0f, 0.0f}).Add<Enemy>();
        groundRanged.Set<Position>({10.0f, 0.0f}).Add<Enemy>().Add<Ranged>();
        flyingMelee.Set<Position>({20.0f, 10.0f}).Add<Enemy>().Add<Flying>();
        flyingRanged.Set<Position>({30.0f, 10.0f}).Add<Enemy>().Add<Flying>().Add<Ranged>();

        // Query for flying ranged enemies
        auto q = w.QueryBuilder<Position>().With<Enemy>().With<Flying>().With<Ranged>().Build();

        int count = 0;
        q.Each([&](duin::Entity entity, Position &pos) {
            count++;
            CHECK(entity.GetName() == "FlyingRanged");
        });

        CHECK(count == 1); // Only flyingRanged has all tags
    }

    TEST_CASE("Query with Without tag filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 100;
        };
        struct Dead
        {
        };
        w.Component<Health>();
        w.Component<Dead>();

        duin::Entity alive1 = w.CreateEntity("Alive1");
        duin::Entity alive2 = w.CreateEntity("Alive2");
        duin::Entity dead1 = w.CreateEntity("Dead1");

        alive1.Set<Health>({100});
        alive2.Set<Health>({50});
        dead1.Set<Health>({0}).Add<Dead>();

        // Query for entities with Health but without Dead tag
        auto q = w.QueryBuilder<Health>().Without<Dead>().Build();

        int aliveCount = 0;
        q.Each([&](duin::Entity entity, Health &health) {
            aliveCount++;
            CHECK(health.hp > 0);
        });

        CHECK(aliveCount == 2); // Only alive1 and alive2
    }

    TEST_CASE("Query with Or tag filter")
    {
        duin::World w;
        struct Damage
        {
            int value = 10;
        };
        struct Fire
        {
        };
        struct Ice
        {
        };
        struct Lightning
        {
        };
        w.Component<Damage>();
        w.Component<Fire>();
        w.Component<Ice>();
        w.Component<Lightning>();

        duin::Entity fireSpell = w.CreateEntity("FireSpell");
        duin::Entity iceSpell = w.CreateEntity("IceSpell");
        duin::Entity lightningSpell = w.CreateEntity("LightningSpell");
        duin::Entity physicalAttack = w.CreateEntity("PhysicalAttack");

        fireSpell.Set<Damage>({20}).Add<Fire>();
        iceSpell.Set<Damage>({15}).Add<Ice>();
        lightningSpell.Set<Damage>({25}).Add<Lightning>();
        physicalAttack.Set<Damage>({10});

        // Query for damage with Fire OR Ice tag
        auto flecsQuery = w.GetFlecsWorld().query_builder<Damage>().with<Fire>().or_().with<Ice>().build();

        int elementalCount = 0;
        int totalDamage = 0;
        flecsQuery.each([&](flecs::entity e, Damage &damage) {
            elementalCount++;
            totalDamage += damage.value;
        });

        CHECK(elementalCount == 2); // Fire and Ice spells
        CHECK(totalDamage == 35);   // 20 + 15
    }

    TEST_CASE("Query with combination of With and Without filters")
    {
        duin::World w;
        struct Unit
        {
            int id = 0;
        };
        struct Friendly
        {
        };
        struct Enemy
        {
        };
        struct Boss
        {
        };
        w.Component<Unit>();
        w.Component<Friendly>();
        w.Component<Enemy>();
        w.Component<Boss>();

        duin::Entity ally1 = w.CreateEntity("Ally1");
        duin::Entity ally2 = w.CreateEntity("Ally2");
        duin::Entity regularEnemy = w.CreateEntity("RegularEnemy");
        duin::Entity bossEnemy = w.CreateEntity("BossEnemy");

        ally1.Set<Unit>({1}).Add<Friendly>();
        ally2.Set<Unit>({2}).Add<Friendly>();
        regularEnemy.Set<Unit>({3}).Add<Enemy>();
        bossEnemy.Set<Unit>({4}).Add<Enemy>().Add<Boss>();

        // Query for regular enemies (Enemy tag but not Boss tag)
        auto q = w.QueryBuilder<Unit>().With<Enemy>().Without<Boss>().Build();

        std::vector<std::string> names;
        q.Each([&](duin::Entity entity, Unit &unit) { names.push_back(entity.GetName()); });

        CHECK(names.size() == 1);
        CHECK(names[0] == "RegularEnemy");
    }

    TEST_CASE("Query with tag-only filter no components")
    {
        duin::World w;
        struct Active
        {
        };
        struct Visible
        {
        };
        w.Component<Active>();
        w.Component<Visible>();

        duin::Entity activeVisible = w.CreateEntity("ActiveVisible");
        duin::Entity activeOnly = w.CreateEntity("ActiveOnly");
        duin::Entity visibleOnly = w.CreateEntity("VisibleOnly");
        duin::Entity neither = w.CreateEntity("Neither");

        activeVisible.Add<Active>().Add<Visible>();
        activeOnly.Add<Active>();
        visibleOnly.Add<Visible>();

        // Query for entities with both Active and Visible tags
        auto q = w.QueryBuilder<>().With<Active>().With<Visible>().Build();

        int count = 0;
        q.Each([&](duin::Entity entity) {
            count++;
            CHECK(entity.GetName() == "ActiveVisible");
        });

        CHECK(count == 1);
    }

    TEST_CASE("Query with tag filters in hierarchy")
    {
        duin::World w;
        struct Transform
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        struct Selectable
        {
        };
        struct Selected
        {
        };
        w.Component<Transform>();
        w.Component<Selectable>();
        w.Component<Selected>();

        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity selectableChild = w.CreateEntity("SelectableChild");
        duin::Entity selectedChild = w.CreateEntity("SelectedChild");
        duin::Entity normalChild = w.CreateEntity("NormalChild");

        parent.Set<Transform>({0.0f, 0.0f});
        selectableChild.Set<Transform>({10.0f, 0.0f}).Add<Selectable>().ChildOf(parent);
        selectedChild.Set<Transform>({20.0f, 0.0f}).Add<Selectable>().Add<Selected>().ChildOf(parent);
        normalChild.Set<Transform>({30.0f, 0.0f}).ChildOf(parent);

        // Query for selected entities (have both Selectable and Selected tags)
        auto q = w.QueryBuilder<Transform>().With<Selectable>().With<Selected>().Build();

        int selectedCount = 0;
        q.Each([&](duin::Entity entity, Transform &transform) {
            selectedCount++;
            CHECK(entity.GetName() == "SelectedChild");
        });

        CHECK(selectedCount == 1);
    }

    TEST_CASE("Query with tag filter and parent cascade")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
            float y = 0.0f;
        };
        struct Enabled
        {
        };
        w.Component<Position>();
        w.Component<Enabled>();

        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity enabledChild = w.CreateEntity("EnabledChild");
        duin::Entity disabledChild = w.CreateEntity("DisabledChild");

        parent.Set<Position>({100.0f, 100.0f}).Add<Enabled>();
        enabledChild.Set<Position>({10.0f, 10.0f}).Add<Enabled>().ChildOf(parent);
        disabledChild.Set<Position>({20.0f, 20.0f}).ChildOf(parent);

        // Query for enabled entities with parent position
        auto flecsQuery = w.GetFlecsWorld()
                              .query_builder<Position, const Position *>()
                              .with<Enabled>()
                              .term_at(1)
                              .parent()
                              .cascade()
                              .build();

        int enabledWithParent = 0;
        flecsQuery.each([&](flecs::entity e, Position &pos, const Position *parentPos) {
            if (parentPos)
            {
                enabledWithParent++;
                // Parent should also be enabled
                CHECK(parentPos->x == 100.0f);
                CHECK(parentPos->y == 100.0f);
            }
        });

        CHECK(enabledWithParent == 1); // Only enabledChild
    }

    TEST_CASE("Query preserves hierarchy relationships after modifications")
    {
        duin::World w;
        struct Transform
        {
            float x = 0.0f;
            float y = 0.0f;
            float z = 0.0f;
        };
        w.Component<Transform>();

        duin::Entity parent = w.CreateEntity("Parent");
        duin::Entity child = w.CreateEntity("Child");

        parent.Set<Transform>({0.0f, 0.0f, 0.0f});
        child.Set<Transform>({10.0f, 10.0f, 10.0f}).ChildOf(parent);

        // Query with parent transform access
        auto flecsQuery =
            w.GetFlecsWorld().query_builder<Transform, const Transform *>().term_at(1).parent().cascade().build();

        flecsQuery.each([&](flecs::entity e, Transform &transform, const Transform *parentTransform) {
            if (parentTransform)
            {
                // Modify child relative to parent
                transform.x = parentTransform->x + 20.0f;
            }
        });

        // Verify hierarchy is still intact and modification worked
        CHECK(child.Get<Transform>().x == doctest::Approx(20.0f));

        // Verify parent is unchanged
        CHECK(parent.Get<Transform>().x == doctest::Approx(0.0f));
    }

    TEST_CASE("Query with deep hierarchy cascade validation")
    {
        duin::World w;
        struct Depth
        {
            int level = 0;
        };
        w.Component<Depth>();

        // Create a deep hierarchy
        duin::Entity root = w.CreateEntity("Root");
        duin::Entity current = root;
        root.Set<Depth>({0});

        const int maxDepth = 5;
        for (int i = 1; i <= maxDepth; ++i)
        {
            std::string name = "Level" + std::to_string(i);
            duin::Entity child = w.CreateEntity(name.c_str());
            child.Set<Depth>({i}).ChildOf(current);
            current = child;
        }

        // Query with cascade to verify parent-child depth relationships
        auto flecsQuery = w.GetFlecsWorld().query_builder<Depth, const Depth *>().term_at(1).parent().cascade().build();

        int validDepthRelations = 0;
        flecsQuery.each([&](flecs::entity e, Depth &depth, const Depth *parentDepth) {
            if (parentDepth)
            {
                // Each child should be exactly one level deeper than parent
                if (depth.level == parentDepth->level + 1)
                {
                    validDepthRelations++;
                }
            }
        });

        CHECK(validDepthRelations == maxDepth); // All 5 parent-child relationships are valid
    }
}
} // namespace TestQuery
