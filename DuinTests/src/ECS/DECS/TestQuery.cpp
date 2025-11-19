#include "doctest.h"
#include "TestQuery.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/Query.h>
#include <string>
#include <vector>

TEST_SUITE("Query")
{

    TEST_CASE("Default constructor")
    {
        duin::Query<> q;
        // Query should be constructible but not usable without proper initialization
        CHECK(typeid(q) == typeid(duin::Query<>));
    }

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

        // Test each method
        int count = 0;
        int totalValue = 0;
        q.Each<TestComponent>([&](flecs::entity entity, TestComponent &comp) {
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
        duin::Query<CompA, CompB> q(flecsQuery);

        // Test each method
        int count = 0;
        int totalA = 0;
        float totalB = 0.0f;
        q.Each<CompA, CompB>([&](flecs::entity entity, CompA &compA, CompB &compB) {
            count++;
            totalA += compA.a;
            totalB += compB.b;
        });

        CHECK(count == 2);                      // Only e1 and e2 have both components
        CHECK(totalA == 15);                    // 5 + 10
        CHECK(totalB == doctest::Approx(4.0f)); // 1.5 + 2.5
    }

    TEST_CASE("iter method iterates over entities with components")
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
        e1.Set<TestComponent>({100});
        e2.Set<TestComponent>({200});

        // Create query
        auto flecsQuery = w.GetFlecsWorld().query<TestComponent>();
        duin::Query q(std::move(flecsQuery));

        // Test iter method
        int iterationCount = 0;
        int totalEntities = 0;
        q.Iter<TestComponent>([&](flecs::iter &it, TestComponent *components) {
            iterationCount++;
            totalEntities += it.count();

            // Verify component values
            for (auto i : it)
            {
                CHECK((components[i].value == 100 || components[i].value == 200));
            }
        });

        CHECK(iterationCount >= 1); // At least one iteration
        CHECK(totalEntities == 2);  // Total of 2 entities processed
    }

    TEST_CASE("iter method with multiple components")
    {
        duin::World w;
        struct CompA
        {
            int a = 0;
        };
        struct CompB
        {
            std::string b;
        };
        w.Component<CompA>();
        w.Component<CompB>();

        // Create entities with both components
        duin::Entity e1 = w.CreateEntity("Entity1");
        duin::Entity e2 = w.CreateEntity("Entity2");
        e1.Set<CompA>({1}).Set<CompB>({"first"});
        e2.Set<CompA>({2}).Set<CompB>({"second"});

        // Create query for both components
        auto flecsQuery = w.GetFlecsWorld().query<CompA, CompB>();
        duin::Query q(std::move(flecsQuery));

        // Test iter method
        int iterationCount = 0;
        int totalEntities = 0;
        std::vector<int> aValues;
        std::vector<std::string> bValues;

        q.Iter<CompA, CompB>([&](flecs::iter &it, CompA *compAs, CompB *compBs) {
            iterationCount++;
            totalEntities += it.count();

            for (auto i : it)
            {
                aValues.push_back(compAs[i].a);
                bValues.push_back(compBs[i].b);
            }
        });

        CHECK(iterationCount >= 1);
        CHECK(totalEntities == 2);
        CHECK(aValues.size() == 2);
        CHECK(bValues.size() == 2);

        // Check that we got the expected values (order may vary)
        bool foundFirst = std::find(bValues.begin(), bValues.end(), "first") != bValues.end();
        bool foundSecond = std::find(bValues.begin(), bValues.end(), "second") != bValues.end();
        CHECK(foundFirst);
        CHECK(foundSecond);
    }

    TEST_CASE("Query with no matching entities")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        // Create query but no entities with the component
        auto flecsQuery = w.GetFlecsWorld().query<TestComponent>();
        duin::Query q(std::move(flecsQuery));

        // Test each method with no entities
        int count = 0;
        q.Each<TestComponent>([&](flecs::entity entity, TestComponent &comp) { count++; });

        CHECK(count == 0);

        // Test iter method with no entities
        int iterCount = 0;
        q.Iter<TestComponent>([&](flecs::iter &it, TestComponent *components) { iterCount++; });

        CHECK(iterCount == 0);
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

        auto flecsQuery = w.GetFlecsWorld().query<TestComponent>();
        const duin::Query q(std::move(flecsQuery));

        // All query operations should work on const query
        const auto &underlyingQuery = q.Get<TestComponent>();
        CHECK(typeid(underlyingQuery) == typeid(flecs::query<TestComponent>));

        int count = 0;
        q.Each<TestComponent>([&](flecs::entity entity, TestComponent &comp) {
            count++;
            CHECK(comp.value == 42);
        });
        CHECK(count == 1);

        count = 0;
        q.Iter<TestComponent>([&](flecs::iter &it, TestComponent *components) { count += it.count(); });
        CHECK(count == 1);
    }
}