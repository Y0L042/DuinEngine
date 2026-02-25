#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

// Component structures for prefab tests
struct Health { float value = 100.0f; bool operator==(const Health& o) const { return value == o.value; } };
struct Armor { float value = 50.0f; bool operator==(const Armor& o) const { return value == o.value; } };
struct Speed { float value = 5.0f; bool operator==(const Speed& o) const { return value == o.value; } };
struct Damage { float value = 10.0f; bool operator==(const Damage& o) const { return value == o.value; } };
struct Prefab {};

static std::string BaseName(const std::string &name)
{
    auto pos = name.find('#');
    return pos != std::string::npos ? name.substr(0, pos) : name;
}

TEST_SUITE("Prefab and Inheritance Tests")
{
    TEST_CASE("Create and instantiate basic prefab")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Armor>();
        world.Component<Prefab>();

        // Create a prefab
        duin::Entity soldierPrefab = world.Entity("SoldierPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f})
            .Set<Armor>({50.0f})
            .Add<Prefab>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({soldierPrefab});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "SoldierPrefab");
        CHECK(ps.entities[0].components.size() >= 3);

        // Instantiate prefab into new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Armor>();
        world2.Component<Prefab>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &instance = worldEntities[0];
        CHECK(BaseName(instance.GetName()) == "SoldierPrefab");
        CHECK(instance.Has<Vec3>());
        CHECK(instance.Has<Health>());
        CHECK(instance.Has<Armor>());
        CHECK(instance.GetMut<Health>() == Health{100.0f});
        CHECK(instance.GetMut<Armor>() == Armor{50.0f});
    }

    TEST_CASE("Prefab inheritance with IsA relationship")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();
        world.Component<Damage>();
        world.Component<Armor>();

        // Create base prefab
        duin::Entity baseSoldier = world.Entity("BaseSoldier")
            .Set<Health>({100.0f})
            .Set<Speed>({5.0f});

        // Create specialized prefab that inherits from base
        duin::Entity heavySoldier = world.Entity("HeavySoldier")
            .IsA(baseSoldier)
            .Set<Health>({150.0f})  // Override health
            .Set<Armor>({75.0f});

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({heavySoldier});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "HeavySoldier");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();
        world2.Component<Damage>();
        world2.Component<Armor>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &instance = worldEntities[0];
        CHECK(instance.Has<Health>());
        CHECK(instance.Has<Armor>());
    }

    TEST_CASE("Pack scene with multiple prefab instances")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();
        world.Component<Prefab>();

        // Create scene root with 5 enemy children
        duin::Entity root = world.Entity("Root");

        for (int i = 0; i < 5; ++i)
        {
            std::string name = "Enemy_" + std::to_string(i);
            world.Entity(name.c_str())
                .ChildOf(root)
                .Set<Vec3>(static_cast<float>(i * 5), 0.0f, 0.0f)
                .Set<Health>({50.0f})
                .Set<Speed>({3.0f});
        }

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene({root});
        scene.name = "EnemySpawnScene";

        // root is the single scene entity with 5 enemy children
        CHECK(scene.entities.size() == 1);
        CHECK(scene.entities[0].children.size() == 5);

        // Serialize and deserialize
        duin::JSONValue json = builder.SerializeScene(scene);
        duin::PackedScene scene2 = builder.DeserializeScene(json);

        CHECK(scene2.entities.size() == 1);
        CHECK(scene2.entities[0].children.size() == 5);
    }

    TEST_CASE("Prefab with child entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Camera>();

        // Create vehicle prefab with nested structure
        duin::Entity vehiclePrefab = world.Entity("VehiclePrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f});

        duin::Entity turret = world.Entity("Turret")
            .ChildOf(vehiclePrefab)
            .Set<Vec3>(0.0f, 2.0f, 0.0f);

        world.Entity("TurretCamera")
            .ChildOf(turret)
            .Set<Vec3>(0.0f, 0.5f, 0.0f)
            .Set<Camera>(60.0f, 0.1f, 500.0f, false);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({vehiclePrefab});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "VehiclePrefab");
        CHECK(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Turret");
        CHECK(ps.entities[0].children[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].children[0].name == "TurretCamera");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Camera>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &instance = worldEntities[0];
        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(BaseName(children[0].GetName()) == "Turret");

            std::vector<duin::Entity> grandchildren = children[0].GetChildren();
            CHECK(grandchildren.size() == 1);
            if (grandchildren.size() >= 1)
            {
                CHECK(BaseName(grandchildren[0].GetName()) == "TurretCamera");
                CHECK(grandchildren[0].Has<Camera>());
            }
        }
    }

    TEST_CASE("Override prefab values in instances")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();

        // Create instances with different values as separate scene root entities
        duin::Entity weakMonster = world.Entity("WeakMonster")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({50.0f})
            .Set<Speed>({5.0f});

        duin::Entity strongMonster = world.Entity("StrongMonster")
            .Set<Vec3>(10.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f})
            .Set<Speed>({3.0f});

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene({weakMonster, strongMonster});

        // Instantiate scene
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();

        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(scene, &world2);

        std::vector<duin::Entity> children = world2.GetChildren();
        CHECK(children.size() == 2);

        // Find and verify overridden values
        for (auto &child : children)
        {
            if (BaseName(child.GetName()) == "WeakMonster")
            {
                CHECK(child.GetMut<Health>() == Health{50.0f});
                CHECK(child.GetMut<Speed>() == Speed{5.0f});
            }
            else if (BaseName(child.GetName()) == "StrongMonster")
            {
                CHECK(child.GetMut<Health>() == Health{200.0f});
                CHECK(child.GetMut<Speed>() == Speed{3.0f});
            }
        }
    }

    TEST_CASE("Nested prefab hierarchies")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Damage>();

        // Create character prefab that includes weapon as child
        duin::Entity characterPrefab = world.Entity("CharacterPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f});

        world.Entity("Weapon")
            .ChildOf(characterPrefab)
            .Set<Vec3>(0.5f, 1.0f, 0.0f)
            .Set<Damage>({10.0f});

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({characterPrefab});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "CharacterPrefab");
        CHECK(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Weapon");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Damage>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &instance = worldEntities[0];
        CHECK(instance.Has<Vec3>());
        CHECK(instance.Has<Health>());

        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(BaseName(children[0].GetName()) == "Weapon");
            CHECK(children[0].Has<Damage>());
            CHECK(children[0].GetMut<Damage>() == Damage{10.0f});
        }
    }
}

} // namespace TestSceneBuilder
