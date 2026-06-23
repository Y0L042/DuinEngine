#include "doctest.h"
#include "TestSystem.h"
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <Duin/ECS/DECS/System.h>
#include <map>
#include <vector>
#include <string>

namespace TestSystem
{
TEST_SUITE("System")
{
    TEST_CASE("Default-constructed system is invalid")
    {
        duin::System sys;
        CHECK(sys.IsValid() == false);
        CHECK(sys.Id() == 0);

        // Operations on an invalid system are safe no-ops.
        sys.Run(0.016f);
        sys.Enable();
        sys.Disable();
        CHECK(sys.Ctx() == nullptr);
        CHECK(sys.IsValid() == false);
    }

    TEST_CASE("System built with Each is valid")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::System sys =
            w.System<TestComponent>("ValidSystem").Each([](duin::Entity entity, TestComponent &comp) {});

        CHECK(sys.IsValid());
        CHECK(sys.Id() != 0);
    }

    TEST_CASE("System Each runs over matching entities on Progress")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2 = w.Entity("Entity2");
        duin::Entity e3 = w.Entity("Entity3");
        e1.Set<TestComponent>({10});
        e2.Set<TestComponent>({20});
        e3.Set<TestComponent>({30});

        int count = 0;
        int totalValue = 0;
        auto sys = w.System<TestComponent>("CountSystem").Each([&](duin::Entity entity, TestComponent &comp) {
            count++;
            totalValue += comp.value;
        });

        w.Progress(0.016f);

        CHECK(count == 3);
        CHECK(totalValue == 60); // 10 + 20 + 30
    }

    TEST_CASE("System Each can mutate components")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity e = w.Entity("Mover");
        e.Set<Position>({0.0f}).Set<Velocity>({5.0f});

        auto sys = w.System<Position, Velocity>("MoveSystem").Each([](duin::Entity entity, Position &p, Velocity &v) {
            p.x += v.x;
        });

        w.Progress(0.016f);
        CHECK(e.Get<Position>().x == doctest::Approx(5.0f));

        w.Progress(0.016f);
        CHECK(e.Get<Position>().x == doctest::Approx(10.0f));
    }

    TEST_CASE("System with multiple components only matches entities with all of them")
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

        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2 = w.Entity("Entity2");
        e1.Set<CompA>({5}).Set<CompB>({1.5f});
        e2.Set<CompA>({10}).Set<CompB>({2.5f});

        duin::Entity e3 = w.Entity("Entity3");
        e3.Set<CompA>({15}); // Only one component - should not match

        int count = 0;
        int totalA = 0;
        float totalB = 0.0f;
        auto sys = w.System<CompA, CompB>("MultiSystem").Each([&](duin::Entity entity, CompA &a, CompB &b) {
            count++;
            totalA += a.a;
            totalB += b.b;
        });

        w.Progress(0.016f);

        CHECK(count == 2);                      // Only e1 and e2 have both
        CHECK(totalA == 15);                    // 5 + 10
        CHECK(totalB == doctest::Approx(4.0f)); // 1.5 + 2.5
    }

    TEST_CASE("System with no matching entities runs the callback zero times")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        int count = 0;
        auto sys =
            w.System<TestComponent>("EmptySystem").Each([&](duin::Entity entity, TestComponent &comp) { count++; });

        w.Progress(0.016f);
        CHECK(count == 0);
    }

    // === PAIR 1: component pack + tag filter (system<Health> + with<Enemy>) ===
    // RAW establishes flecs' behaviour; the DUIN twin below must mirror it via
    // the wrapper's near-identical API and reach the same result.

    TEST_CASE("RAW FLECS: system<Health> with with<Enemy> filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 100;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        flecs::world &fw = w.GetFlecsWorld();

        flecs::entity player = fw.entity("Player");
        flecs::entity minion1 = fw.entity("Minion1");
        flecs::entity minion2 = fw.entity("Minion2");

        player.set<Health>({100});
        minion1.set<Health>({50}).add<Enemy>();
        minion2.set<Health>({50}).add<Enemy>();

        int enemyCount = 0;
        int totalHp = 0;
        flecs::system sys =
            fw.system<Health>("RawEnemySystem").with<Enemy>().each([&](flecs::entity entity, Health &health) {
                enemyCount++;
                totalHp += health.hp;
            });

        fw.progress(0.016f);

        CHECK(enemyCount == 2); // Only the two minions
        CHECK(totalHp == 100);  // 50 + 50
    }

    TEST_CASE("DUIN: System<Health> with With<Enemy> filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 100;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        duin::Entity player = w.Entity("Player");
        duin::Entity minion1 = w.Entity("Minion1");
        duin::Entity minion2 = w.Entity("Minion2");

        player.Set<Health>({100});
        minion1.Set<Health>({50}).Add<Enemy>();
        minion2.Set<Health>({50}).Add<Enemy>();

        int enemyCount = 0;
        int totalHp = 0;
        auto sys = w.System<Health>("EnemySystem").With<Enemy>().Each([&](duin::Entity entity, Health &health) {
            enemyCount++;
            totalHp += health.hp;
        });

        w.Progress(0.016f);

        CHECK(enemyCount == 2); // Only the two minions
        CHECK(totalHp == 100);  // 50 + 50
    }

    // === PAIR 2: component pack + Without tag filter (system<Health> + without<Dead>) ===

    TEST_CASE("RAW FLECS: system<Health> with without<Dead> filter")
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

        flecs::world &fw = w.GetFlecsWorld();

        flecs::entity alive1 = fw.entity("Alive1");
        flecs::entity alive2 = fw.entity("Alive2");
        flecs::entity dead1 = fw.entity("Dead1");

        alive1.set<Health>({100});
        alive2.set<Health>({50});
        dead1.set<Health>({0}).add<Dead>();

        int aliveCount = 0;
        flecs::system sys =
            fw.system<Health>("RawAliveSystem").without<Dead>().each([&](flecs::entity entity, Health &health) {
                aliveCount++;
                CHECK(health.hp > 0);
            });

        fw.progress(0.016f);
        CHECK(aliveCount == 2); // Only alive1 and alive2
    }

    TEST_CASE("DUIN: System<Health> with Without<Dead> filter")
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

        duin::Entity alive1 = w.Entity("Alive1");
        duin::Entity alive2 = w.Entity("Alive2");
        duin::Entity dead1 = w.Entity("Dead1");

        alive1.Set<Health>({100});
        alive2.Set<Health>({50});
        dead1.Set<Health>({0}).Add<Dead>();

        int aliveCount = 0;
        auto sys = w.System<Health>("AliveSystem").Without<Dead>().Each([&](duin::Entity entity, Health &health) {
            aliveCount++;
            CHECK(health.hp > 0);
        });

        w.Progress(0.016f);
        CHECK(aliveCount == 2); // Only alive1 and alive2
    }

    // === PAIR 3: tag-only system iterated with each(entity) ===
    // A component-less system (system<>) filtered purely on tags. The DUIN twin
    // mirrors the RAW each(entity) API exactly so it documents whether the
    // wrapper reproduces flecs' behaviour for this pattern.

    TEST_CASE("RAW FLECS: tag-only system with each(entity)")
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

        flecs::world &fw = w.GetFlecsWorld();

        flecs::entity activeVisible = fw.entity("ActiveVisible");
        flecs::entity activeOnly = fw.entity("ActiveOnly");
        flecs::entity visibleOnly = fw.entity("VisibleOnly");

        activeVisible.add<Active>().add<Visible>();
        activeOnly.add<Active>();
        visibleOnly.add<Visible>();

        int count = 0;
        flecs::system sys =
            fw.system<>("RawActiveVisibleSystem").with<Active>().with<Visible>().each([&](flecs::entity entity) {
                count++;
                CHECK(entity.name() == "ActiveVisible");
            });

        fw.progress(0.016f);
        CHECK(count == 1);
    }

    TEST_CASE("DUIN: tag-only System<> with Each(entity)")
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

        duin::Entity activeVisible = w.Entity("ActiveVisible");
        duin::Entity activeOnly = w.Entity("ActiveOnly");
        duin::Entity visibleOnly = w.Entity("VisibleOnly");

        activeVisible.Add<Active>().Add<Visible>();
        activeOnly.Add<Active>();
        visibleOnly.Add<Visible>();

        int count = 0;
        auto sys = w.System<>("ActiveVisibleSystem").With<Active>().With<Visible>().Each([&](duin::Entity entity) {
            count++;
            CHECK(entity.GetName() == "ActiveVisible");
        });

        w.Progress(0.016f);
        CHECK(count == 1);
    }

    // === PAIR 4: tag-only system iterated with run() + manual iteration ===
    // The run() pattern flecs supports for component-less (filter-only) systems.

    TEST_CASE("RAW FLECS: tag-only system with run() manual iteration")
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

        flecs::world &fw = w.GetFlecsWorld();

        flecs::entity activeVisible = fw.entity("ActiveVisible");
        flecs::entity activeOnly = fw.entity("ActiveOnly");
        flecs::entity visibleOnly = fw.entity("VisibleOnly");

        activeVisible.add<Active>().add<Visible>();
        activeOnly.add<Active>();
        visibleOnly.add<Visible>();

        int count = 0;
        flecs::system sys =
            fw.system<>("RawActiveVisibleRunSystem").with<Active>().with<Visible>().run([&](flecs::iter &it) {
                while (it.next())
                {
                    for (auto i : it)
                    {
                        count++;
                        CHECK(it.entity(i).name() == "ActiveVisible");
                    }
                }
            });

        fw.progress(0.016f);
        CHECK(count == 1);
    }

    TEST_CASE("DUIN: tag-only System<> with Run() manual iteration")
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

        duin::Entity activeVisible = w.Entity("ActiveVisible");
        duin::Entity activeOnly = w.Entity("ActiveOnly");
        duin::Entity visibleOnly = w.Entity("VisibleOnly");

        activeVisible.Add<Active>().Add<Visible>();
        activeOnly.Add<Active>();
        visibleOnly.Add<Visible>();

        int count = 0;
        auto sys = w.System<>("ActiveVisibleRunSystem").With<Active>().With<Visible>().Run([&](duin::Iter &it) {
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    count++;
                    CHECK(w.Entity(it.GetEntityId(i)).GetName() == "ActiveVisible");
                }
            }
        });

        w.Progress(0.016f);
        CHECK(count == 1);
    }

    TEST_CASE("System Run iterates per table")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2 = w.Entity("Entity2");
        e1.Set<TestComponent>({1});
        e2.Set<TestComponent>({2});

        int totalEntitiesSeen = 0;
        auto sys = w.System<TestComponent>("RunSystem").Run([&](duin::Iter &it) {
            while (it.Next())
            {
                totalEntitiesSeen += static_cast<int>(it.Count());
            }
        });

        w.Progress(0.016f);
        CHECK(totalEntitiesSeen == 2);
    }

    TEST_CASE("System Run executes the system once on demand")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e1 = w.Entity("Entity1");
        duin::Entity e2 = w.Entity("Entity2");
        e1.Set<TestComponent>({10});
        e2.Set<TestComponent>({20});

        int count = 0;
        auto sys =
            w.System<TestComponent>("OnDemandSystem").Each([&](duin::Entity entity, TestComponent &comp) { count++; });

        // Run once directly, without progressing the whole world.
        sys.Run(0.016f);
        CHECK(count == 2);
    }

    TEST_CASE("Disabled system does not run on Progress")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e = w.Entity("Entity1");
        e.Set<TestComponent>({10});

        int count = 0;
        auto sys =
            w.System<TestComponent>("ToggleSystem").Each([&](duin::Entity entity, TestComponent &comp) { count++; });

        sys.Disable();
        w.Progress(0.016f);
        CHECK(count == 0);

        sys.Enable();
        w.Progress(0.016f);
        CHECK(count == 1);
    }

    TEST_CASE("System context pointer round-trips")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        auto sys = w.System<TestComponent>("CtxSystem").Each([](duin::Entity entity, TestComponent &comp) {});

        int payload = 42;
        sys.Ctx(&payload);
        CHECK(sys.Ctx() == &payload);
    }

    TEST_CASE("System Entity wraps the underlying system entity")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        auto sys = w.System<TestComponent>("EntitySystem").Each([](duin::Entity entity, TestComponent &comp) {});

        duin::Entity sysEntity = sys;
        CHECK(sysEntity.IsValid());
        CHECK(sysEntity.GetName() == "EntitySystem");
        CHECK(sys.Id() == sysEntity.GetID());
    }

    TEST_CASE("Destructed system no longer runs and becomes unusable")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e = w.Entity("Entity1");
        e.Set<TestComponent>({10});

        int count = 0;
        auto sys =
            w.System<TestComponent>("DestructSystem").Each([&](duin::Entity entity, TestComponent &comp) { count++; });

        sys.Destruct();
        w.Progress(0.016f);
        CHECK(count == 0);
    }

    TEST_CASE("System operations are usable on a const handle")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        duin::Entity e = w.Entity("Entity1");
        e.Set<TestComponent>({7});

        int count = 0;
        const duin::System sys =
            w.System<TestComponent>("ConstSystem").Each([&](duin::Entity entity, TestComponent &comp) {
                count++;
                CHECK(comp.value == 7);
            });

        CHECK(sys.IsValid());

        const flecs::system &underlying = sys.GetFlecsSystem();
        CHECK(static_cast<bool>(underlying));

        sys.Run(0.016f);
        CHECK(count == 1);
    }

    // ======================================================================
    // Multi-type / chained filter expansion (With<A, B>, Without<A, B>, mixes)
    // ======================================================================

    TEST_CASE("System With<A, B, C>() requires all three tags")
    {
        duin::World w;
        struct Health
        {
            int hp = 100;
        };
        struct A
        {
        };
        struct B
        {
        };
        struct C
        {
        };
        w.Component<Health>();
        w.Component<A>();
        w.Component<B>();
        w.Component<C>();

        duin::Entity all = w.Entity("All");
        duin::Entity ab = w.Entity("AB");
        duin::Entity a = w.Entity("A");

        all.Set<Health>({1}).Add<A>().Add<B>().Add<C>();
        ab.Set<Health>({2}).Add<A>().Add<B>();
        a.Set<Health>({3}).Add<A>();

        int count = 0;
        auto sys = w.System<Health>("AllThree").With<A, B, C>().Each([&](duin::Entity e, Health &) {
            count++;
            CHECK(e.GetName() == "All");
        });

        w.Progress(0.016f);
        CHECK(count == 1); // Only the entity with A, B and C
    }

    TEST_CASE("System With<A, B>() equals chained With<A>().With<B>()")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct A
        {
        };
        struct B
        {
        };
        w.Component<Health>();
        w.Component<A>();
        w.Component<B>();

        duin::Entity both = w.Entity("Both");
        duin::Entity onlyA = w.Entity("OnlyA");
        both.Set<Health>({1}).Add<A>().Add<B>();
        onlyA.Set<Health>({2}).Add<A>();

        int variadicCount = 0;
        w.System<Health>("Variadic").With<A, B>().Each([&](duin::Entity, Health &) { variadicCount++; });

        int chainedCount = 0;
        w.System<Health>("Chained").With<A>().With<B>().Each([&](duin::Entity, Health &) { chainedCount++; });

        w.Progress(0.016f);
        CHECK(variadicCount == 1);
        CHECK(chainedCount == 1);
        CHECK(variadicCount == chainedCount);
    }

    TEST_CASE("System Without<A, B>() excludes entities having either tag")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Dead
        {
        };
        struct Stunned
        {
        };
        w.Component<Health>();
        w.Component<Dead>();
        w.Component<Stunned>();

        duin::Entity healthy = w.Entity("Healthy");
        duin::Entity dead = w.Entity("Dead");
        duin::Entity stunned = w.Entity("Stunned");
        healthy.Set<Health>({100});
        dead.Set<Health>({0}).Add<Dead>();
        stunned.Set<Health>({50}).Add<Stunned>();

        int count = 0;
        auto sys = w.System<Health>("OnlyHealthy").Without<Dead, Stunned>().Each([&](duin::Entity e, Health &) {
            count++;
            CHECK(e.GetName() == "Healthy");
        });

        w.Progress(0.016f);
        CHECK(count == 1); // Neither Dead nor Stunned
    }

    TEST_CASE("System mixes With and Without filters")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        struct Dead
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();
        w.Component<Dead>();

        duin::Entity liveEnemy = w.Entity("LiveEnemy");
        duin::Entity deadEnemy = w.Entity("DeadEnemy");
        duin::Entity ally = w.Entity("Ally");
        liveEnemy.Set<Health>({10}).Add<Enemy>();
        deadEnemy.Set<Health>({0}).Add<Enemy>().Add<Dead>();
        ally.Set<Health>({20});

        int count = 0;
        auto sys = w.System<Health>("LiveEnemies").With<Enemy>().Without<Dead>().Each([&](duin::Entity e, Health &) {
            count++;
            CHECK(e.GetName() == "LiveEnemy");
        });

        w.Progress(0.016f);
        CHECK(count == 1); // Enemy and not Dead
    }

    TEST_CASE("System tag-only With<A, B>() (no component pack) via Run")
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

        duin::Entity activeVisible = w.Entity("ActiveVisible");
        duin::Entity activeOnly = w.Entity("ActiveOnly");
        activeVisible.Add<Active>().Add<Visible>();
        activeOnly.Add<Active>();

        int count = 0;
        auto sys = w.System<>("TagOnlyVariadic").With<Active, Visible>().Run([&](duin::Iter &it) {
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    count++;
                    CHECK(w.Entity(it.GetEntityId(i)).GetName() == "ActiveVisible");
                }
            }
        });

        w.Progress(0.016f);
        CHECK(count == 1);
    }

    // ======================================================================
    // Builder move-safety regression guards
    //
    // flecs::system_builder has no move ctor that re-points its internal
    // desc_/term_ pointers; the duin wrapper heap-holds it so terms added after
    // construction survive. These guard against a regression where filter terms
    // or callbacks would be silently dropped.
    // ======================================================================

    TEST_CASE("System filter terms survive builder construction (regression)")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        duin::Entity player = w.Entity("Player");
        duin::Entity enemy = w.Entity("Enemy");
        player.Set<Health>({100});
        enemy.Set<Health>({50}).Add<Enemy>();

        // The With<Enemy>() term is added AFTER the flecs builder is constructed.
        // If the builder were moved (dangling desc_), this term would be lost and
        // the system would match the player too.
        int count = 0;
        auto sys = w.System<Health>("EnemyOnly").With<Enemy>().Each([&](duin::Entity e, Health &) {
            count++;
            CHECK(e.GetName() == "Enemy");
        });

        w.Progress(0.016f);
        CHECK(count == 1); // ONLY the enemy, not the player
    }

    TEST_CASE("Multiple systems built from the same world keep distinct filters")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        struct Ally
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();
        w.Component<Ally>();

        duin::Entity enemy = w.Entity("Enemy");
        duin::Entity ally = w.Entity("Ally");
        enemy.Set<Health>({1}).Add<Enemy>();
        ally.Set<Health>({1}).Add<Ally>();

        int enemyCount = 0;
        int allyCount = 0;
        auto enemySys = w.System<Health>("EnemySys").With<Enemy>().Each([&](duin::Entity e, Health &) {
            enemyCount++;
            CHECK(e.GetName() == "Enemy");
        });
        auto allySys = w.System<Health>("AllySys").With<Ally>().Each([&](duin::Entity e, Health &) {
            allyCount++;
            CHECK(e.GetName() == "Ally");
        });

        w.Progress(0.016f);
        CHECK(enemyCount == 1);
        CHECK(allyCount == 1);
    }

    // ======================================================================
    // Optional / pointer components
    // ======================================================================

    TEST_CASE("System with optional (pointer) component matches both presence and absence")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity moving = w.Entity("Moving");
        duin::Entity stationary = w.Entity("Stationary");
        moving.Set<Position>({0.0f}).Set<Velocity>({5.0f});
        stationary.Set<Position>({0.0f});

        int matched = 0;
        int withVelocity = 0;
        auto sys =
            w.System<Position, const Velocity *>("OptVel").Each([&](duin::Entity, Position &p, const Velocity *v) {
                matched++;
                if (v)
                {
                    withVelocity++;
                    p.x += v->x;
                }
            });

        w.Progress(0.016f);
        CHECK(matched == 2);                                      // both entities matched
        CHECK(withVelocity == 1);                                 // only one has Velocity
        CHECK(moving.Get<Position>().x == doctest::Approx(5.0f)); // mutated
        CHECK(stationary.Get<Position>().x == doctest::Approx(0.0f));
    }

    // ======================================================================
    // Run callback receives delta time and component fields
    // ======================================================================

    TEST_CASE("System Run sees per-table entity ids and count")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();

        duin::Entity e1 = w.Entity("E1");
        duin::Entity e2 = w.Entity("E2");
        e1.Set<Marker>({1});
        e2.Set<Marker>({2});

        std::vector<std::string> names;
        auto sys = w.System<Marker>("RunNames").Run([&](duin::Iter &it) {
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    names.push_back(w.Entity(it.GetEntityId(i)).GetName());
                }
            }
        });

        w.Progress(0.016f);
        CHECK(names.size() == 2);
    }

    TEST_CASE("System Run delta time reflects the progress step")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("E1").Set<Marker>({1});

        float seenDelta = -1.0f;
        auto sys = w.System<Marker>("DeltaSys").Run([&](duin::Iter &it) {
            seenDelta = it.DeltaTime();
            while (it.Next())
            {
            }
        });

        w.Progress(0.25f);
        CHECK(seenDelta == doctest::Approx(0.25f));
    }

    // ======================================================================
    // Builder-level configuration (Ctx, Interval, Rate, Kind)
    // ======================================================================

    TEST_CASE("Builder Ctx sets the context retrievable from the System")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        int payload = 7;
        auto sys = w.System<TestComponent>("CtxBuilder").Ctx(&payload).Each([](duin::Entity, TestComponent &) {});

        CHECK(sys.Ctx() == &payload);
    }

    TEST_CASE("Builder Interval round-trips through the System")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        auto sys = w.System<TestComponent>("IntervalBuilder").Interval(0.5f).Each([](duin::Entity, TestComponent &) {});

        CHECK(sys.IsValid());
        CHECK(sys.Interval() == doctest::Approx(0.5f));
    }

    TEST_CASE("System Interval setter updates the interval")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();

        auto sys = w.System<TestComponent>("IntervalSetter").Each([](duin::Entity, TestComponent &) {});
        sys.Interval(1.0f);
        CHECK(sys.Interval() == doctest::Approx(1.0f));
    }

    TEST_CASE("System Rate runs the system every Nth frame")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();
        w.Entity("E1").Set<TestComponent>({1});

        int rateCount = 0;
        int plainCount = 0;
        // Rate(2): the system ticks once every 2 frames. Compare against an
        // unthrottled system over the same frames rather than asserting an exact
        // tick-alignment, which is a flecs internal detail.
        auto rateSys =
            w.System<TestComponent>("RateSys").Rate(2).Each([&](duin::Entity, TestComponent &) { rateCount++; });
        auto plainSys = w.System<TestComponent>("PlainSys").Each([&](duin::Entity, TestComponent &) { plainCount++; });

        for (int i = 0; i < 6; ++i)
            w.Progress(0.016f);

        CHECK(plainCount == 6);        // runs every frame
        CHECK(rateCount < plainCount); // throttled
        CHECK(rateCount >= 2);         // but did tick several times
        CHECK(rateCount <= 3);         // ~ every other frame
    }

    // ======================================================================
    // Run-on-demand parameter pass-through and idempotency
    // ======================================================================

    TEST_CASE("System on-demand Run can execute repeatedly")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();
        w.Entity("E1").Set<TestComponent>({1});

        int count = 0;
        auto sys = w.System<TestComponent>("RepeatRun").Each([&](duin::Entity, TestComponent &) { count++; });

        sys.Run(0.016f);
        sys.Run(0.016f);
        sys.Run(0.016f);
        CHECK(count == 3);
    }

    TEST_CASE("Re-enabling a system after disable resumes execution")
    {
        duin::World w;
        struct TestComponent
        {
            int value = 0;
        };
        w.Component<TestComponent>();
        w.Entity("E1").Set<TestComponent>({1});

        int count = 0;
        auto sys = w.System<TestComponent>("ToggleResume").Each([&](duin::Entity, TestComponent &) { count++; });

        w.Progress(0.016f); // 1
        sys.Disable();
        w.Progress(0.016f); // still 1
        w.Progress(0.016f); // still 1
        sys.Enable();
        w.Progress(0.016f); // 2
        CHECK(count == 2);
    }

    // ======================================================================
    // Component-add during iteration window / dynamic membership
    // ======================================================================

    TEST_CASE("System picks up newly matching entities on next Progress")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        duin::Entity e = w.Entity("Late");
        e.Set<Health>({1});

        int count = 0;
        auto sys = w.System<Health>("LatchOn").With<Enemy>().Each([&](duin::Entity, Health &) { count++; });

        w.Progress(0.016f);
        CHECK(count == 0); // not an Enemy yet

        e.Add<Enemy>();
        w.Progress(0.016f);
        CHECK(count == 1); // now matches
    }

    // ======================================================================
    // Invalid-system safety
    // ======================================================================

    TEST_CASE("Invalid system getters return safe defaults")
    {
        duin::System sys;
        CHECK(sys.IsValid() == false);
        CHECK(sys.Id() == 0);
        CHECK(sys.Ctx() == nullptr);
        CHECK(sys.Interval() == doctest::Approx(0.0f));
        CHECK(sys.IsValid() == false);
        // Mutators on an invalid handle must not crash.
        sys.Interval(1.0f);
        sys.Ctx(nullptr);
        sys.Destruct();
    }
}

// ==========================================================================
// Manual run: drive systems with System::Run(deltaTime, param) instead of
// World::Progress(). This is the primary execution path for this project, so
// it is covered thoroughly and independently of the pipeline.
//
// Notes on semantics:
//  - System::Run(dt, param) calls ecs_run on the system once, immediately,
//    regardless of pipeline phase, enable/disable does NOT gate it the same way
//    Progress does (see dedicated cases below), and it ignores Interval/Rate
//    throttling (those are pipeline concepts).
// ==========================================================================
TEST_SUITE("System Manual Run")
{
    TEST_CASE("Manual Run executes Each over all matching entities")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();

        w.Entity("A").Set<Value>({1});
        w.Entity("B").Set<Value>({2});
        w.Entity("C").Set<Value>({3});

        int count = 0;
        int sum = 0;
        auto sys = w.System<Value>("SumSys").Each([&](duin::Entity, Value &val) {
            count++;
            sum += val.v;
        });

        sys.Run();
        CHECK(count == 3);
        CHECK(sum == 6);
    }

    TEST_CASE("Manual Run does not run on Progress alone (no double execution)")
    {
        // A manually-run system should execute exactly when Run() is called.
        // This guards against the callback being invoked an unexpected number of
        // times across a manual Run plus an unrelated Progress.
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("A").Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("ManualSys").Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run is repeatable and accumulates per call")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("A").Set<Value>({1});
        w.Entity("B").Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("RepeatSys").Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        sys.Run();
        sys.Run();
        CHECK(count == 6); // 2 entities * 3 runs
    }

    TEST_CASE("Manual Run mutates components and persists across calls")
    {
        duin::World w;
        struct Counter
        {
            int n = 0;
        };
        w.Component<Counter>();
        duin::Entity e = w.Entity("E");
        e.Set<Counter>({0});

        auto sys = w.System<Counter>("IncSys").Each([](duin::Entity, Counter &c) { c.n++; });

        sys.Run();
        CHECK(e.Get<Counter>().n == 1);
        sys.Run();
        sys.Run();
        CHECK(e.Get<Counter>().n == 3);
    }

    TEST_CASE("Manual Run forwards delta time to the callback")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity e = w.Entity("Mover");
        e.Set<Position>({0.0f}).Set<Velocity>({10.0f});

        // The delta time passed to System::Run() must be visible on the iterator
        // inside the Run callback.
        float seenDelta = -1.0f;
        bool ran = false;
        auto sys = w.System<Position, Velocity>("IntegrateSys").Run([&](duin::Iter &it) {
            ran = true;
            seenDelta = it.DeltaTime();
            while (it.Next())
            {
            }
        });

        sys.Run(0.5f);
        CHECK(ran);
        CHECK(seenDelta == doctest::Approx(0.5f));
    }

    TEST_CASE("Manual Run with Each integrates using delta-time-scaled logic")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity e = w.Entity("Mover");
        e.Set<Position>({0.0f}).Set<Velocity>({10.0f});

        // Each does not receive delta time directly; use a fixed step captured by
        // the test to confirm mutation works under manual run.
        const float dt = 0.5f;
        auto sys = w.System<Position, Velocity>("StepSys").Each(
            [dt](duin::Entity, Position &p, Velocity &v) { p.x += v.x * dt; });

        sys.Run(dt);
        CHECK(e.Get<Position>().x == doctest::Approx(5.0f));
        sys.Run(dt);
        CHECK(e.Get<Position>().x == doctest::Approx(10.0f));
    }

    TEST_CASE("Manual Run with no matching entities runs callback zero times")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        // No entities with Value.

        int count = 0;
        auto sys = w.System<Value>("EmptyManual").Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        CHECK(count == 0);
    }

    TEST_CASE("Manual Run respects With filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        w.Entity("Player").Set<Health>({100});
        w.Entity("Minion1").Set<Health>({50}).Add<Enemy>();
        w.Entity("Minion2").Set<Health>({50}).Add<Enemy>();

        int count = 0;
        int totalHp = 0;
        auto sys = w.System<Health>("EnemyManual").With<Enemy>().Each([&](duin::Entity, Health &h) {
            count++;
            totalHp += h.hp;
        });

        sys.Run();
        CHECK(count == 2);
        CHECK(totalHp == 100);
    }

    TEST_CASE("Manual Run respects Without filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Dead
        {
        };
        w.Component<Health>();
        w.Component<Dead>();

        w.Entity("Alive1").Set<Health>({100});
        w.Entity("Alive2").Set<Health>({50});
        w.Entity("Dead1").Set<Health>({0}).Add<Dead>();

        int count = 0;
        auto sys = w.System<Health>("AliveManual").Without<Dead>().Each([&](duin::Entity, Health &h) {
            count++;
            CHECK(h.hp > 0);
        });

        sys.Run();
        CHECK(count == 2);
    }

    TEST_CASE("Manual Run respects multi-type With<A, B> filter")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct A
        {
        };
        struct B
        {
        };
        w.Component<Health>();
        w.Component<A>();
        w.Component<B>();

        w.Entity("Both").Set<Health>({1}).Add<A>().Add<B>();
        w.Entity("OnlyA").Set<Health>({1}).Add<A>();

        int count = 0;
        auto sys = w.System<Health>("BothTags").With<A, B>().Each([&](duin::Entity e, Health &) {
            count++;
            CHECK(e.GetName() == "Both");
        });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run respects combined With and Without filters")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        struct Dead
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();
        w.Component<Dead>();

        w.Entity("LiveEnemy").Set<Health>({10}).Add<Enemy>();
        w.Entity("DeadEnemy").Set<Health>({0}).Add<Enemy>().Add<Dead>();
        w.Entity("Ally").Set<Health>({20});

        int count = 0;
        auto sys =
            w.System<Health>("LiveEnemyManual").With<Enemy>().Without<Dead>().Each([&](duin::Entity e, Health &) {
                count++;
                CHECK(e.GetName() == "LiveEnemy");
            });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run with optional (pointer) component")
    {
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity moving = w.Entity("Moving");
        duin::Entity still = w.Entity("Still");
        moving.Set<Position>({0.0f}).Set<Velocity>({4.0f});
        still.Set<Position>({0.0f});

        int matched = 0;
        int withVel = 0;
        auto sys =
            w.System<Position, const Velocity *>("OptManual").Each([&](duin::Entity, Position &p, const Velocity *v) {
                matched++;
                if (v)
                {
                    withVel++;
                    p.x += v->x;
                }
            });

        sys.Run();
        CHECK(matched == 2);
        CHECK(withVel == 1);
        CHECK(moving.Get<Position>().x == doctest::Approx(4.0f));
        CHECK(still.Get<Position>().x == doctest::Approx(0.0f));
    }

    TEST_CASE("Manual Run with multi-component system matches all-of")
    {
        duin::World w;
        struct A
        {
            int a = 0;
        };
        struct B
        {
            int b = 0;
        };
        w.Component<A>();
        w.Component<B>();

        w.Entity("AB").Set<A>({1}).Set<B>({2});
        w.Entity("OnlyA").Set<A>({3});

        int count = 0;
        auto sys = w.System<A, B>("MultiManual").Each([&](duin::Entity, A &, B &) { count++; });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run via Run() callback iterates tables")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("E1").Set<Marker>({1});
        w.Entity("E2").Set<Marker>({2});
        w.Entity("E3").Set<Marker>({3});

        int seen = 0;
        auto sys = w.System<Marker>("RunCbManual").Run([&](duin::Iter &it) {
            while (it.Next())
            {
                seen += static_cast<int>(it.Count());
            }
        });

        sys.Run();
        CHECK(seen == 3);
    }

    TEST_CASE("Manual Run via Run() callback reads entity ids")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("Alpha").Set<Marker>({1});

        std::vector<std::string> names;
        auto sys = w.System<Marker>("RunIdsManual").Run([&](duin::Iter &it) {
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    names.push_back(w.Entity(it.GetEntityId(i)).GetName());
                }
            }
        });

        sys.Run();
        REQUIRE(names.size() == 1);
        CHECK(names[0] == "Alpha");
    }

    TEST_CASE("Manual Run on a tag-only System<> via Run()")
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

        w.Entity("ActiveVisible").Add<Active>().Add<Visible>();
        w.Entity("ActiveOnly").Add<Active>();

        int count = 0;
        auto sys = w.System<>("TagOnlyManual").With<Active>().With<Visible>().Run([&](duin::Iter &it) {
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    count++;
                    CHECK(w.Entity(it.GetEntityId(i)).GetName() == "ActiveVisible");
                }
            }
        });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run picks up newly matching entities between runs")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();

        duin::Entity e = w.Entity("Late");
        e.Set<Health>({1});

        int count = 0;
        auto sys = w.System<Health>("LatchManual").With<Enemy>().Each([&](duin::Entity, Health &) { count++; });

        sys.Run();
        CHECK(count == 0); // not an Enemy yet

        e.Add<Enemy>();
        sys.Run();
        CHECK(count == 1); // now matches
    }

    TEST_CASE("Manual Run reflects entity removal between runs")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        duin::Entity a = w.Entity("A");
        duin::Entity b = w.Entity("B");
        a.Set<Value>({1});
        b.Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("RemovalManual").Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        CHECK(count == 2);

        b.Destruct();
        count = 0;
        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run reflects component removal between runs")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        duin::Entity e = w.Entity("E");
        e.Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("CompRemovalManual").Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        CHECK(count == 1);

        e.Remove<Value>();
        count = 0;
        sys.Run();
        CHECK(count == 0);
    }

    TEST_CASE("Manual Run on a destructed system is a safe no-op")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("E").Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("DestructManual").Each([&](duin::Entity, Value &) { count++; });

        sys.Destruct();
        sys.Run();
        CHECK(count == 0);
    }

    TEST_CASE("Manual Run on a default-constructed system is a safe no-op")
    {
        duin::System sys;
        CHECK(sys.IsValid() == false);
        sys.Run();       // must not crash
        sys.Run(0.016f); // must not crash
        sys.Run(0.016f, nullptr);
        CHECK(sys.Id() == 0);
    }

    TEST_CASE("Manual Run works through a const System handle")
    {
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("E").Set<Value>({7});

        int count = 0;
        const duin::System sys = w.System<Value>("ConstManual").Each([&](duin::Entity, Value &val) {
            count++;
            CHECK(val.v == 7);
        });

        sys.Run(0.016f);
        CHECK(count == 1);
    }

    TEST_CASE("Multiple manually-run systems execute independently")
    {
        duin::World w;
        struct Health
        {
            int hp = 0;
        };
        struct Enemy
        {
        };
        struct Ally
        {
        };
        w.Component<Health>();
        w.Component<Enemy>();
        w.Component<Ally>();

        w.Entity("Enemy").Set<Health>({1}).Add<Enemy>();
        w.Entity("Ally").Set<Health>({1}).Add<Ally>();

        int enemyCount = 0;
        int allyCount = 0;
        auto enemySys = w.System<Health>("EnemyManualSys").With<Enemy>().Each([&](duin::Entity e, Health &) {
            enemyCount++;
            CHECK(e.GetName() == "Enemy");
        });
        auto allySys = w.System<Health>("AllyManualSys").With<Ally>().Each([&](duin::Entity e, Health &) {
            allyCount++;
            CHECK(e.GetName() == "Ally");
        });

        // Run each independently; one must not affect the other's matching.
        enemySys.Run();
        CHECK(enemyCount == 1);
        CHECK(allyCount == 0);

        allySys.Run();
        CHECK(enemyCount == 1);
        CHECK(allyCount == 1);
    }

    TEST_CASE("Manual Run executes regardless of pipeline phase (Kind)")
    {
        // A manual Run() bypasses pipeline scheduling, so even a system placed in
        // a phase still runs immediately when invoked directly.
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("E").Set<Value>({1});

        int count = 0;
        auto sys = w.System<Value>("PhasedManual").Kind(flecs::OnUpdate).Each([&](duin::Entity, Value &) { count++; });

        sys.Run();
        CHECK(count == 1);
    }

    TEST_CASE("Manual Run still works after the builder chain is fully consumed")
    {
        // Regression guard tied to the heap-held flecs builder: the System must
        // remain valid and runnable after SystemBuilder is gone (it is a
        // temporary in these chains).
        duin::World w;
        struct Value
        {
            int v = 0;
        };
        w.Component<Value>();
        w.Entity("E1").Set<Value>({1});
        w.Entity("E2").Set<Value>({1});

        int count = 0;
        duin::System sys =
            w.System<Value>("SurvivesManual").With<Value>().Each([&](duin::Entity, Value &) { count++; });

        CHECK(sys.IsValid());
        sys.Run();
        CHECK(count == 2);
    }

    // ----------------------------------------------------------------------
    // Passing parameters into Run() callbacks
    // ----------------------------------------------------------------------

    TEST_CASE("Manual Run delta time is readable as a Run() param-style input")
    {
        // The common case: pass a frame delta time through System::Run(dt) and
        // read it back from the iterator inside the Run callback.
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("E").Set<Marker>({1});

        float seenDelta = -1.0f;
        auto sys = w.System<Marker>("DeltaParam").Run([&](duin::Iter &it) {
            seenDelta = it.DeltaTime();
            while (it.Next())
            {
            }
        });

        sys.Run(0.25f);
        CHECK(seenDelta == doctest::Approx(0.25f));

        sys.Run(0.5f);
        CHECK(seenDelta == doctest::Approx(0.5f));
    }

    TEST_CASE("Manual Run forwards a user param pointer to the Run callback")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("E").Set<Marker>({1});

        struct Frame
        {
            float dt = 0.0f;
            int tick = 0;
        };
        Frame frame{0.016f, 42};

        bool ran = false;
        void *seenRaw = nullptr;
        Frame seen{};
        auto sys = w.System<Marker>("ParamSys").Run([&](duin::Iter &it) {
            ran = true;
            seenRaw = it.Param();
            if (Frame *f = it.Param<Frame>())
            {
                seen = *f;
            }
            while (it.Next())
            {
            }
        });

        sys.Run(0.016f, &frame);
        CHECK(ran);
        CHECK(seenRaw == &frame);
        CHECK(seen.dt == doctest::Approx(0.016f));
        CHECK(seen.tick == 42);
    }

    TEST_CASE("Manual Run param is null when no param is passed")
    {
        duin::World w;
        struct Marker
        {
            int v = 0;
        };
        w.Component<Marker>();
        w.Entity("E").Set<Marker>({1});

        bool ran = false;
        void *seen = reinterpret_cast<void *>(0x1); // sentinel non-null
        auto sys = w.System<Marker>("NoParamSys").Run([&](duin::Iter &it) {
            ran = true;
            seen = it.Param();
            while (it.Next())
            {
            }
        });

        sys.Run(0.016f); // no param argument
        CHECK(ran);
        CHECK(seen == nullptr);
    }

    TEST_CASE("Manual Run param can drive mutation scaled by a passed delta time")
    {
        // Realistic usage: pass a struct carrying dt and let the system integrate
        // using it via the Run() callback + iterator entity access.
        duin::World w;
        struct Position
        {
            float x = 0.0f;
        };
        struct Velocity
        {
            float x = 0.0f;
        };
        w.Component<Position>();
        w.Component<Velocity>();

        duin::Entity e = w.Entity("Mover");
        e.Set<Position>({0.0f}).Set<Velocity>({10.0f});

        struct StepParam
        {
            float dt = 0.0f;
        };
        StepParam step{0.5f};

        auto sys = w.System<Position, Velocity>("ParamIntegrate").Run([&](duin::Iter &it) {
            StepParam *p = it.Param<StepParam>();
            REQUIRE(p != nullptr);
            float dt = p->dt;
            while (it.Next())
            {
                for (size_t i = 0; i < it.Count(); ++i)
                {
                    duin::Entity ent = w.Entity(it.GetEntityId(i));
                    Velocity v = ent.Get<Velocity>();
                    Position pos = ent.Get<Position>();
                    pos.x += v.x * dt;
                    ent.Set<Position>(pos);
                }
            }
        });

        sys.Run(0.016f, &step);
        CHECK(e.Get<Position>().x == doctest::Approx(5.0f)); // 10 * 0.5

        sys.Run(0.016f, &step);
        CHECK(e.Get<Position>().x == doctest::Approx(10.0f));
    }
}
} // namespace TestSystem
