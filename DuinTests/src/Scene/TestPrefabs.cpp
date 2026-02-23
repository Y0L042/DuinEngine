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
        duin::Entity soldierPrefab = world.CreateEntity("SoldierPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f})
            .Set<Armor>({50.0f})
            .Add<Prefab>();

        // Pack the prefab
        duin::PackedEntity pe = duin::PackedEntity::Pack(soldierPrefab);

        CHECK(pe.name == "SoldierPrefab");
        CHECK(pe.components.size() >= 3);

        // Instantiate prefab
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Armor>();
        world2.Component<Prefab>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

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

        // Create base prefab
        duin::Entity baseSoldier = world.CreateEntity("BaseSoldier")
            .Set<Health>({100.0f})
            .Set<Speed>({5.0f});

        // Create specialized prefab that inherits from base
        duin::Entity heavySoldier = world.CreateEntity("HeavySoldier")
            .IsA(baseSoldier)
            .Set<Health>({150.0f})  // Override health
            .Set<Armor>({75.0f});

        duin::PackedEntity pe = duin::PackedEntity::Pack(heavySoldier);

        CHECK(pe.name == "HeavySoldier");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();
        world2.Component<Damage>();
        world2.Component<Armor>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

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

        // Create enemy prefab
        duin::Entity enemyPrefab = world.CreateEntity("EnemyPrefab")
            .Set<Health>({50.0f})
            .Set<Speed>({3.0f})
            .Add<Prefab>();

        duin::PackedEntity prefabPacked = duin::PackedEntity::Pack(enemyPrefab);

        // Create scene root and instantiate multiple enemies
        duin::Entity root = world.CreateEntity("Root");

        for (int i = 0; i < 5; ++i)
        {
            std::string name = "Enemy_" + std::to_string(i);
            duin::Entity enemy = world.CreateEntity(name.c_str())
                .ChildOf(root)
                .Set<Vec3>(static_cast<float>(i * 5), 0.0f, 0.0f)
                .Set<Health>({50.0f})
                .Set<Speed>({3.0f});
        }

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene({root});
        scene.name = "EnemySpawnScene";

        CHECK(scene.entities.size() == 5);

        // Serialize and deserialize
        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene scene2 = duin::PackedScene::Deserialize(json);

        CHECK(scene2.entities.size() == 5);
    }

    TEST_CASE("Prefab with child entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Camera>();

        // Create vehicle prefab with nested structure
        duin::Entity vehiclePrefab = world.CreateEntity("VehiclePrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f});

        duin::Entity turret = world.CreateEntity("Turret")
            .ChildOf(vehiclePrefab)
            .Set<Vec3>(0.0f, 2.0f, 0.0f);

        duin::Entity turretCamera = world.CreateEntity("TurretCamera")
            .ChildOf(turret)
            .Set<Vec3>(0.0f, 0.5f, 0.0f)
            .Set<Camera>(60.0f, 0.1f, 500.0f, false);

        duin::PackedEntity pe = duin::PackedEntity::Pack(vehiclePrefab);

        CHECK(pe.name == "VehiclePrefab");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Turret");
        CHECK(pe.children[0].children.size() == 1);
        CHECK(pe.children[0].children[0].name == "TurretCamera");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Camera>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].GetName() == "Turret");

            std::vector<duin::Entity> grandchildren = children[0].GetChildren();
            CHECK(grandchildren.size() == 1);
            if (grandchildren.size() >= 1)
            {
                CHECK(grandchildren[0].GetName() == "TurretCamera");
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

        // Create base prefab
        duin::Entity monsterPrefab = world.CreateEntity("MonsterPrefab")
            .Set<Health>({100.0f})
            .Set<Speed>({5.0f});

        duin::PackedEntity prefabPacked = duin::PackedEntity::Pack(monsterPrefab);

        // Create instances with overridden values
        duin::Entity root = world.CreateEntity("Root");

        duin::Entity weakMonster = world.CreateEntity("WeakMonster")
            .ChildOf(root)
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({50.0f})   // Override
            .Set<Speed>({5.0f});

        duin::Entity strongMonster = world.CreateEntity("StrongMonster")
            .ChildOf(root)
            .Set<Vec3>(10.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f})  // Override
            .Set<Speed>({3.0f});    // Override

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene({root});

        // Instantiate scene
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();

        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(scene, &world2);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 2);

        // Find and verify overridden values
        for (auto& child : children)
        {
            if (child.GetName() == "WeakMonster")
            {
                CHECK(child.GetMut<Health>() == Health{50.0f});
                CHECK(child.GetMut<Speed>() == Speed{5.0f});
            }
            else if (child.GetName() == "StrongMonster")
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

        // Create weapon prefab
        duin::Entity weaponPrefab = world.CreateEntity("WeaponPrefab")
            .Set<Damage>({10.0f});

        // Create character prefab that includes weapon
        duin::Entity characterPrefab = world.CreateEntity("CharacterPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f});

        duin::Entity characterWeapon = world.CreateEntity("Weapon")
            .ChildOf(characterPrefab)
            .Set<Vec3>(0.5f, 1.0f, 0.0f)
            .Set<Damage>({10.0f});

        duin::PackedEntity pe = duin::PackedEntity::Pack(characterPrefab);

        CHECK(pe.name == "CharacterPrefab");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Weapon");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Damage>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        CHECK(instance.Has<Vec3>());
        CHECK(instance.Has<Health>());
        
        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].GetName() == "Weapon");
            CHECK(children[0].Has<Damage>());
            CHECK(children[0].GetMut<Damage>() == Damage{10.0f});
        }
    }
}

} // namespace TestSceneBuilder
