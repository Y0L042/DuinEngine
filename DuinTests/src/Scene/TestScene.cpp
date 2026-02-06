#include "doctest.h"
#include <Duin/Scene/Scene.h>
#include <Duin/ECS/DECS/World.h>
#include <Duin/ECS/DECS/Entity.h>
#include <string>
#include <vector>
#include <algorithm>

namespace TestScene
{

static int CountRootEntities(duin::World &world)
{
    int count = 0;
    world.IterateChildren([&](duin::Entity) { count++; });
    return count;
}

TEST_SUITE("Scene")
{
    // ========================================================================
    // CONSTRUCTION
    // ========================================================================

    TEST_CASE("Default construction and destruction")
    {
        duin::Scene scene;
        CHECK(&scene != nullptr);
    }

    TEST_CASE("Multiple independent instances")
    {
        duin::Scene a;
        duin::Scene b;
        CHECK(&a != &b);
    }

    // ========================================================================
    // SERIALIZATION
    // ========================================================================

    TEST_CASE("SceneToSerial - default scene produces non-empty string")
    {
        duin::Scene scene;
        std::string serial = scene.SceneToSerial();
        CHECK_FALSE(serial.empty());
    }

    TEST_CASE("SerialToScene - empty string does not crash")
    {
        duin::Scene scene = duin::Scene::SerialToScene("");
        CHECK(&scene != nullptr);
    }

    TEST_CASE("SerialToScene - invalid input does not crash")
    {
        duin::Scene scene = duin::Scene::SerialToScene("not valid serialized data !@#$");
        CHECK(&scene != nullptr);
    }

    TEST_CASE("Serialization round-trip - empty scene")
    {
        duin::Scene original;
        std::string serial   = original.SceneToSerial();
        duin::Scene restored = duin::Scene::SerialToScene(serial);

        // Both represent empty scenes. Instantiating into fresh worlds
        // should yield the same entity count.
        duin::World worldA;
        duin::World worldB;
        original.SceneToInstance(worldA);
        restored.SceneToInstance(worldB);

        CHECK(CountRootEntities(worldA) == CountRootEntities(worldB));
    }

    TEST_CASE("Serialization round-trip - scene with entities")
    {
        duin::World source;
        source.CreateEntity("Alpha");
        source.CreateEntity("Beta");

        duin::Scene scene  = duin::Scene::InstanceToScene(source);
        std::string serial = scene.SceneToSerial();
        duin::Scene scene2 = duin::Scene::SerialToScene(serial);

        // Both scenes should instantiate identically
        duin::World worldA;
        duin::World worldB;
        scene.SceneToInstance(worldA);
        scene2.SceneToInstance(worldB);

        CHECK(CountRootEntities(worldA) == CountRootEntities(worldB));
    }

    // ========================================================================
    // INSTANCE CAPTURE (World -> Scene)
    // ========================================================================

    TEST_CASE("InstanceToScene - empty world")
    {
        duin::World world;
        duin::Scene scene = duin::Scene::InstanceToScene(world);

        duin::World restored;
        scene.SceneToInstance(restored);

        CHECK(CountRootEntities(restored) == 0);
    }

    TEST_CASE("InstanceToScene - single entity")
    {
        duin::World world;
        world.CreateEntity("Solo");

        duin::Scene scene = duin::Scene::InstanceToScene(world);

        duin::World restored;
        scene.SceneToInstance(restored);

        CHECK(CountRootEntities(restored) >= 1);
    }

    TEST_CASE("InstanceToScene - multiple entities")
    {
        duin::World world;
        world.CreateEntity("A");
        world.CreateEntity("B");
        world.CreateEntity("C");

        duin::Scene scene = duin::Scene::InstanceToScene(world);

        duin::World restored;
        scene.SceneToInstance(restored);

        CHECK(CountRootEntities(restored) >= 3);
    }

    TEST_CASE("InstanceToScene - does not modify source world")
    {
        duin::World world;
        duin::Entity e = world.CreateEntity("Original");
        uint64_t id    = e.GetID();

        duin::Scene::InstanceToScene(world);

        CHECK(world.IsAlive(id));
    }

    // ========================================================================
    // WORLD ROUND-TRIP (World -> Scene -> World)
    // ========================================================================

    TEST_CASE("World round-trip - entity IDs are preserved")
    {
        duin::World original;
        uint64_t id1 = original.CreateEntity("E1").GetID();
        uint64_t id2 = original.CreateEntity("E2").GetID();

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        scene.SceneToInstance(restored);

        CHECK(restored.IsAlive(id1));
        CHECK(restored.IsAlive(id2));
    }

    TEST_CASE("World round-trip - entity names are preserved")
    {
        duin::World original;
        original.CreateEntity("PlayerSpawn");
        original.CreateEntity("EnemyWave");

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        scene.SceneToInstance(restored);

        std::vector<std::string> names;
        restored.IterateChildren([&](duin::Entity e) { names.push_back(e.GetName()); });

        CHECK(std::find(names.begin(), names.end(), "PlayerSpawn") != names.end());
        CHECK(std::find(names.begin(), names.end(), "EnemyWave") != names.end());
    }

    TEST_CASE("World round-trip - component data is preserved")
    {
        struct Position
        {
            float x;
            float y;
            float z;
        };

        duin::World original;
        original.Component<Position>();
        original.CreateEntity("Movable").Set<Position>({1.0f, 2.0f, 3.0f});

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        restored.Component<Position>();
        scene.SceneToInstance(restored);

        bool found = false;
        restored.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Movable")
            {
                const Position *pos = e.TryGet<Position>();
                if (pos)
                {
                    CHECK(pos->x == doctest::Approx(1.0f));
                    CHECK(pos->y == doctest::Approx(2.0f));
                    CHECK(pos->z == doctest::Approx(3.0f));
                    found = true;
                }
            }
        });
        CHECK(found);
    }

    TEST_CASE("World round-trip - multiple components on one entity")
    {
        struct Position
        {
            float x;
            float y;
            float z;
        };
        struct Velocity
        {
            float dx;
            float dy;
            float dz;
        };
        struct Health
        {
            int hp;
        };

        duin::World original;
        original.Component<Position>();
        original.Component<Velocity>();
        original.Component<Health>();
        original.CreateEntity("Player")
            .Set<Position>({10.0f, 0.0f, 5.0f})
            .Set<Velocity>({1.0f, 0.0f, -1.0f})
            .Set<Health>({100});

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        restored.Component<Position>();
        restored.Component<Velocity>();
        restored.Component<Health>();
        scene.SceneToInstance(restored);

        bool found = false;
        restored.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Player")
            {
                CHECK(e.Has<Position>());
                CHECK(e.Has<Velocity>());
                CHECK(e.Has<Health>());
                CHECK(e.Get<Health>().hp == 100);
                found = true;
            }
        });
        CHECK(found);
    }

    // ========================================================================
    // HIERARCHY ROUND-TRIP
    // ========================================================================

    TEST_CASE("World round-trip - parent-child relationship is preserved")
    {
        duin::World original;
        duin::Entity parent = original.CreateEntity("Parent");
        duin::Entity child  = original.CreateEntity("Child");
        child.ChildOf(parent);

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        scene.SceneToInstance(restored);

        bool hierarchyValid = false;
        restored.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Parent")
            {
                std::vector<duin::Entity> children = e.GetChildren();
                if (children.size() == 1 && children[0].GetName() == "Child")
                {
                    hierarchyValid = true;
                }
            }
        });
        CHECK(hierarchyValid);
    }

    TEST_CASE("World round-trip - three-level hierarchy is preserved")
    {
        duin::World original;
        duin::Entity root = original.CreateEntity("Root");
        duin::Entity mid  = original.CreateEntity("Mid");
        duin::Entity leaf = original.CreateEntity("Leaf");
        mid.ChildOf(root);
        leaf.ChildOf(mid);

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        scene.SceneToInstance(restored);

        bool valid = false;
        restored.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Root")
            {
                auto rootChildren = e.GetChildren();
                if (rootChildren.size() == 1 && rootChildren[0].GetName() == "Mid")
                {
                    auto midChildren = rootChildren[0].GetChildren();
                    if (midChildren.size() == 1 && midChildren[0].GetName() == "Leaf")
                    {
                        valid = true;
                    }
                }
            }
        });
        CHECK(valid);
    }

    TEST_CASE("World round-trip - multiple children under one parent")
    {
        duin::World original;
        duin::Entity parent = original.CreateEntity("Room");
        original.CreateEntity("Light").ChildOf(parent);
        original.CreateEntity("Camera").ChildOf(parent);
        original.CreateEntity("Trigger").ChildOf(parent);

        duin::Scene scene = duin::Scene::InstanceToScene(original);

        duin::World restored;
        scene.SceneToInstance(restored);

        bool valid = false;
        restored.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Room")
            {
                std::vector<std::string> childNames;
                for (auto &c : e.GetChildren())
                    childNames.push_back(c.GetName());

                bool hasLight   = std::find(childNames.begin(), childNames.end(), "Light")   != childNames.end();
                bool hasCamera  = std::find(childNames.begin(), childNames.end(), "Camera")  != childNames.end();
                bool hasTrigger = std::find(childNames.begin(), childNames.end(), "Trigger") != childNames.end();
                valid = hasLight && hasCamera && hasTrigger;
            }
        });
        CHECK(valid);
    }

    // ========================================================================
    // FULL PIPELINE (World -> Scene -> Serial -> Scene -> World)
    // ========================================================================

    TEST_CASE("Full pipeline - single entity with component")
    {
        struct Velocity
        {
            float dx;
            float dy;
        };

        duin::World original;
        original.Component<Velocity>();
        original.CreateEntity("Bullet").Set<Velocity>({5.0f, -3.0f});

        duin::Scene captured      = duin::Scene::InstanceToScene(original);
        std::string serial        = captured.SceneToSerial();
        duin::Scene deserialized  = duin::Scene::SerialToScene(serial);

        duin::World final_world;
        final_world.Component<Velocity>();
        deserialized.SceneToInstance(final_world);

        bool found = false;
        final_world.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Bullet")
            {
                const Velocity *vel = e.TryGet<Velocity>();
                if (vel)
                {
                    CHECK(vel->dx == doctest::Approx(5.0f));
                    CHECK(vel->dy == doctest::Approx(-3.0f));
                    found = true;
                }
            }
        });
        CHECK(found);
    }

    TEST_CASE("Full pipeline - hierarchy survives serialization")
    {
        duin::World original;
        duin::Entity parent = original.CreateEntity("Room");
        original.CreateEntity("Light").ChildOf(parent);
        original.CreateEntity("Camera").ChildOf(parent);

        duin::Scene captured      = duin::Scene::InstanceToScene(original);
        std::string serial        = captured.SceneToSerial();
        duin::Scene deserialized  = duin::Scene::SerialToScene(serial);

        duin::World final_world;
        deserialized.SceneToInstance(final_world);

        bool valid = false;
        final_world.IterateChildren([&](duin::Entity e) {
            if (e.GetName() == "Room")
            {
                std::vector<std::string> childNames;
                for (auto &c : e.GetChildren())
                    childNames.push_back(c.GetName());

                bool hasLight  = std::find(childNames.begin(), childNames.end(), "Light")  != childNames.end();
                bool hasCamera = std::find(childNames.begin(), childNames.end(), "Camera") != childNames.end();
                valid          = hasLight && hasCamera;
            }
        });
        CHECK(valid);
    }

    TEST_CASE("Full pipeline - empty scene survives serialization")
    {
        duin::Scene empty;
        std::string serial      = empty.SceneToSerial();
        duin::Scene deserialized = duin::Scene::SerialToScene(serial);

        duin::World final_world;
        deserialized.SceneToInstance(final_world);

        CHECK(CountRootEntities(final_world) == 0);
    }
}
} // namespace TestScene
