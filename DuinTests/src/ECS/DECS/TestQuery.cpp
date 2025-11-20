#include "doctest.h"
#include "TestQuery.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/Query.h>

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
}