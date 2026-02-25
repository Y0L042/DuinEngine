#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

// Helper: strip the '#uuid' suffix that SceneBuilder::InstantiateEntity appends.
static std::string BaseName(const std::string &name)
{
    auto pos = name.find('#');
    return pos != std::string::npos ? name.substr(0, pos) : name;
}

TEST_SUITE("Complex Hierarchy Tests")
{
    TEST_CASE("Pack and Instantiate entity tree with mixed components at all levels")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Root (Vec3)
        //   ChildA (Camera)
        //     GrandchildA1 (Vec3)
        //     GrandchildA2 (Vec3, Camera)
        //   ChildB (Vec3, Camera)
        //     GrandchildB1 (Camera)
        //       GreatGrandchildB1 (Vec3)

        duin::Entity root = world.Entity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        duin::Entity childA = world.Entity("ChildA").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        childA.ChildOf(root);

        duin::Entity grandchildA1 = world.Entity("GrandchildA1").Set<Vec3>(1.0f, 0.0f, 0.0f);
        grandchildA1.ChildOf(childA);

        duin::Entity grandchildA2 =
            world.Entity("GrandchildA2").Set<Vec3>(2.0f, 0.0f, 0.0f).Set<Camera>(60.0f, 0.2f, 500.0f, false);
        grandchildA2.ChildOf(childA);

        duin::Entity childB =
            world.Entity("ChildB").Set<Vec3>(0.0f, 1.0f, 0.0f).Set<Camera>(75.0f, 0.1f, 800.0f, false);
        childB.ChildOf(root);

        duin::Entity grandchildB1 = world.Entity("GrandchildB1").Set<Camera>(45.0f, 0.5f, 300.0f, true);
        grandchildB1.ChildOf(childB);

        duin::Entity greatGrandchildB1 = world.Entity("GreatGrandchildB1").Set<Vec3>(3.0f, 3.0f, 3.0f);
        greatGrandchildB1.ChildOf(grandchildB1);

        // Pack root as scene entity; root itself is ps.entities[0]
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({root});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Root");
        CHECK(ps.entities[0].children.size() == 2);

        // Instantiate into new world; root entity is created at world root level
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "Root");
        CHECK(restored.Has<Vec3>());
        CHECK(restored.GetMut<Vec3>() == Vec3{0.0f, 0.0f, 0.0f});

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 2);

        // Find ChildA (Camera only) and ChildB (Vec3 + Camera)
        duin::Entity *restoredChildA = nullptr;
        duin::Entity *restoredChildB = nullptr;
        for (auto &child : children)
        {
            if (BaseName(child.GetName()) == "ChildA")
                restoredChildA = &child;
            if (BaseName(child.GetName()) == "ChildB")
                restoredChildB = &child;
        }

        REQUIRE(restoredChildA != nullptr);
        REQUIRE(restoredChildB != nullptr);

        // Verify ChildA
        CHECK(restoredChildA->Has<Camera>());
        CHECK(restoredChildA->GetMut<Camera>() == Camera{90.0f, 0.1f, 1000.0f, true});
        std::vector<duin::Entity> grandchildrenA = restoredChildA->GetChildren();
        CHECK(grandchildrenA.size() == 2);

        // Verify ChildB and its deep hierarchy
        CHECK(restoredChildB->Has<Vec3>());
        CHECK(restoredChildB->Has<Camera>());
        CHECK(restoredChildB->GetMut<Vec3>() == Vec3{0.0f, 1.0f, 0.0f});
        CHECK(restoredChildB->GetMut<Camera>() == Camera{75.0f, 0.1f, 800.0f, false});

        std::vector<duin::Entity> grandchildrenB = restoredChildB->GetChildren();
        CHECK(grandchildrenB.size() == 1);
        if (grandchildrenB.size() >= 1)
        {
            CHECK(BaseName(grandchildrenB[0].GetName()) == "GrandchildB1");
            CHECK(grandchildrenB[0].Has<Camera>());

            std::vector<duin::Entity> greatGrandchildren = grandchildrenB[0].GetChildren();
            CHECK(greatGrandchildren.size() == 1);
            if (greatGrandchildren.size() >= 1)
            {
                CHECK(BaseName(greatGrandchildren[0].GetName()) == "GreatGrandchildB1");
                CHECK(greatGrandchildren[0].Has<Vec3>());
                CHECK(greatGrandchildren[0].GetMut<Vec3>() == Vec3{3.0f, 3.0f, 3.0f});
            }
        }

        // Verify parent-child relationships
        CHECK(restoredChildA->GetParent() == restored);
        CHECK(restoredChildB->GetParent() == restored);
        if (grandchildrenA.size() >= 2)
        {
            CHECK(grandchildrenA[0].GetParent() == *restoredChildA);
            CHECK(grandchildrenA[1].GetParent() == *restoredChildA);
        }
        if (grandchildrenB.size() >= 1)
        {
            CHECK(grandchildrenB[0].GetParent() == *restoredChildB);
            std::vector<duin::Entity> greatGrandchildren = grandchildrenB[0].GetChildren();
            if (greatGrandchildren.size() >= 1)
            {
                CHECK(greatGrandchildren[0].GetParent() == grandchildrenB[0]);
            }
        }

        duin::Entity parentOfChildA = restoredChildA->GetParent();
        CHECK(BaseName(parentOfChildA.GetName()) == "Root");
        CHECK(parentOfChildA == restored);
    }

    TEST_CASE("Scene with multiple complex entity hierarchies")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Create three entity hierarchies as world-root entities (no shared container)
        duin::Entity player =
            world.Entity("Player").Set<Vec3>(10.0f, 0.0f, 0.0f).Set<Camera>(90.0f, 0.1f, 2000.0f, true);

        duin::Entity playerCamera = world.Entity("PlayerCamera").Set<Camera>(75.0f, 0.1f, 1500.0f, false);
        playerCamera.ChildOf(player);

        duin::Entity weapon = world.Entity("Weapon").Set<Vec3>(0.5f, 1.2f, 0.3f);
        weapon.ChildOf(player);

        duin::Entity weaponEffect = world.Entity("WeaponEffect").Set<Vec3>(0.1f, 0.1f, 0.1f);
        weaponEffect.ChildOf(weapon);

        duin::Entity enemy =
            world.Entity("Enemy").Set<Vec3>(-10.0f, 0.0f, 0.0f).Set<Camera>(60.0f, 0.2f, 1000.0f, false);

        duin::Entity enemyWeapon = world.Entity("EnemyWeapon").Set<Vec3>(0.3f, 1.0f, 0.2f);
        enemyWeapon.ChildOf(enemy);

        duin::Entity environment = world.Entity("Environment").Set<Vec3>(0.0f, -1.0f, 0.0f);

        duin::Entity light1 = world.Entity("Light1").Set<Vec3>(5.0f, 10.0f, 5.0f);
        light1.ChildOf(environment);

        duin::Entity light2 = world.Entity("Light2").Set<Vec3>(-5.0f, 10.0f, -5.0f);
        light2.ChildOf(environment);

        // Pack the three root entities as separate scene entities
        duin::SceneBuilder sceneBuilder(&world);
        duin::PackedScene ps = sceneBuilder.PackScene({player, enemy, environment});
        ps.uuid = duin::UUID::FromStringHex("c0ab1e5cee111111");
        ps.name = "ComplexScene";

        CHECK(ps.entities.size() == 3);

        // Serialize and deserialize via SceneBuilder instance
        duin::JSONValue json = sceneBuilder.SerializeScene(ps);
        std::string jsonStr = json.Write();
        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = sceneBuilder.DeserializeScene(json2);

        // Instantiate as children of a new root entity
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        duin::Entity newRoot = world2.Entity("NewSceneRoot");
        sb2.InstantiateSceneAsChildren(ps2, newRoot);

        std::vector<duin::Entity> rootChildren = newRoot.GetChildren();
        CHECK(rootChildren.size() == 3);

        // Find Player, Enemy, Environment by base name
        duin::Entity *restoredPlayer = nullptr;
        duin::Entity *restoredEnemy = nullptr;
        duin::Entity *restoredEnvironment = nullptr;

        for (auto &child : rootChildren)
        {
            std::string bn = BaseName(child.GetName());
            if (bn == "Player")
                restoredPlayer = &child;
            if (bn == "Enemy")
                restoredEnemy = &child;
            if (bn == "Environment")
                restoredEnvironment = &child;
        }

        REQUIRE(restoredPlayer != nullptr);
        REQUIRE(restoredEnemy != nullptr);
        REQUIRE(restoredEnvironment != nullptr);

        // Verify Player
        CHECK(restoredPlayer->Has<Vec3>());
        CHECK(restoredPlayer->Has<Camera>());
        std::vector<duin::Entity> playerChildren = restoredPlayer->GetChildren();
        CHECK(playerChildren.size() == 2); // PlayerCamera and Weapon

        // Verify weapon hierarchy
        bool foundWeapon = false;
        for (auto &child : playerChildren)
        {
            if (BaseName(child.GetName()) == "Weapon")
            {
                foundWeapon = true;
                CHECK(child.Has<Vec3>());
                std::vector<duin::Entity> weaponChildren = child.GetChildren();
                CHECK(weaponChildren.size() == 1);
                if (weaponChildren.size() >= 1)
                {
                    CHECK(BaseName(weaponChildren[0].GetName()) == "WeaponEffect");
                }
            }
        }
        CHECK(foundWeapon);

        // Verify Enemy
        CHECK(restoredEnemy->Has<Vec3>());
        CHECK(restoredEnemy->Has<Camera>());
        std::vector<duin::Entity> enemyChildren = restoredEnemy->GetChildren();
        CHECK(enemyChildren.size() == 1);
        if (enemyChildren.size() >= 1)
        {
            CHECK(BaseName(enemyChildren[0].GetName()) == "EnemyWeapon");
        }

        // Verify Environment
        CHECK(restoredEnvironment->Has<Vec3>());
        std::vector<duin::Entity> envChildren = restoredEnvironment->GetChildren();
        CHECK(envChildren.size() == 2);

        // Verify parent-child relationships
        CHECK(restoredPlayer->GetParent() == newRoot);
        CHECK(restoredEnemy->GetParent() == newRoot);
        CHECK(restoredEnvironment->GetParent() == newRoot);

        for (auto &child : playerChildren)
            CHECK(child.GetParent() == *restoredPlayer);

        for (auto &child : enemyChildren)
            CHECK(child.GetParent() == *restoredEnemy);

        for (auto &child : envChildren)
            CHECK(child.GetParent() == *restoredEnvironment);

        if (foundWeapon)
        {
            for (auto &child : playerChildren)
            {
                if (BaseName(child.GetName()) == "Weapon")
                {
                    std::vector<duin::Entity> weaponChildren = child.GetChildren();
                    if (!weaponChildren.empty())
                        CHECK(weaponChildren[0].GetParent() == child);
                }
            }
        }
    }

    TEST_CASE("Wide hierarchy - entity with many children each having components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 10; ++i)
        {
            std::string name = "Child" + std::to_string(i);
            duin::Entity child = world.Entity(name.c_str());

            if (i % 3 == 0)
                child.Set<Vec3>(static_cast<float>(i), 0.0f, 0.0f);
            else if (i % 3 == 1)
                child.Set<Camera>(static_cast<float>(60 + i), 0.1f, static_cast<float>(1000 + i * 100), i % 2 == 0);
            else
            {
                child.Set<Vec3>(static_cast<float>(i), static_cast<float>(i), 0.0f);
                child.Set<Camera>(static_cast<float>(45 + i), 0.2f, static_cast<float>(800 + i * 50), false);
            }

            child.ChildOf(parent);
        }

        // Pack parent as scene entity; parent is ps.entities[0]
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 10);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 10);

        int vec3Count = 0;
        int cameraCount = 0;
        int bothCount = 0;

        for (auto &child : children)
        {
            bool hasVec3 = child.Has<Vec3>();
            bool hasCamera = child.Has<Camera>();

            if (hasVec3 && hasCamera)
                bothCount++;
            else if (hasVec3)
                vec3Count++;
            else if (hasCamera)
                cameraCount++;
        }

        // i=0,3,6,9 → Vec3-only (4), i=1,4,7 → Camera-only (3), i=2,5,8 → both (3)
        CHECK(vec3Count == 4);
        CHECK(cameraCount == 3);
        CHECK(bothCount == 3);

        for (auto &child : children)
            CHECK(child.GetParent() == restored);

        CHECK(restored.GetChildren().size() == 10);
    }

    TEST_CASE("Deep and wide - 5 levels deep with multiple children at each level")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity root = world.Entity("L0_Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 3; ++i)
        {
            std::string l1Name = "L1_" + std::to_string(i);
            duin::Entity l1 = world.Entity(l1Name.c_str());

            if (i == 0)
                l1.Set<Vec3>(1.0f, 0.0f, 0.0f);
            else if (i == 1)
                l1.Set<Camera>(90.0f, 0.1f, 1000.0f, true);
            else
                l1.Set<Vec3>(2.0f, 0.0f, 0.0f).Set<Camera>(60.0f, 0.2f, 500.0f, false);

            l1.ChildOf(root);

            for (int j = 0; j < 2; ++j)
            {
                std::string l2Name = "L2_" + std::to_string(i) + "_" + std::to_string(j);
                duin::Entity l2 = world.Entity(l2Name.c_str()).Set<Vec3>(static_cast<float>(i + j), 1.0f, 0.0f);
                l2.ChildOf(l1);

                for (int k = 0; k < 2; ++k)
                {
                    std::string l3Name =
                        "L3_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                    duin::Entity l3 = world.Entity(l3Name.c_str())
                                          .Set<Camera>(static_cast<float>(45 + k), 0.1f, 800.0f, k == 0);
                    l3.ChildOf(l2);

                    std::string l4Name =
                        "L4_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                    duin::Entity l4 =
                        world.Entity(l4Name.c_str())
                            .Set<Vec3>(static_cast<float>(i), static_cast<float>(j), static_cast<float>(k));
                    l4.ChildOf(l3);
                }
            }
        }

        // Pack root as scene entity
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({root});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 3);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "L0_Root");
        CHECK(restored.Has<Vec3>());

        std::vector<duin::Entity> l1Children = restored.GetChildren();
        CHECK(l1Children.size() == 3);

        if (l1Children.size() >= 1)
        {
            std::vector<duin::Entity> l2Children = l1Children[0].GetChildren();
            CHECK(l2Children.size() == 2);

            if (l2Children.size() >= 1)
            {
                std::vector<duin::Entity> l3Children = l2Children[0].GetChildren();
                CHECK(l3Children.size() == 2);

                if (l3Children.size() >= 1)
                {
                    std::vector<duin::Entity> l4Children = l3Children[0].GetChildren();
                    CHECK(l4Children.size() == 1);
                    if (l4Children.size() >= 1)
                    {
                        CHECK(l4Children[0].Has<Vec3>());
                        CHECK(l4Children[0].GetChildren().empty());
                        CHECK(l4Children[0].GetParent() == l3Children[0]);
                    }

                    CHECK(l3Children[0].GetParent() == l2Children[0]);
                    CHECK(l3Children[1].GetParent() == l2Children[0]);
                    CHECK(l3Children[0].GetChildren().size() == 1);
                }

                CHECK(l2Children[0].GetParent() == l1Children[0]);
                CHECK(l2Children[1].GetParent() == l1Children[0]);
                CHECK(l2Children[0].GetChildren().size() == 2);
            }

            CHECK(l1Children[0].GetChildren().size() == 2);
        }

        if (l1Children.size() >= 3)
        {
            CHECK(l1Children[0].GetParent() == restored);
            CHECK(l1Children[1].GetParent() == restored);
            CHECK(l1Children[2].GetParent() == restored);
        }

        CHECK(restored.GetChildren().size() == 3);
    }

    TEST_CASE("Mixed scene - some entities with deep hierarchies, others standalone")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Create world-root entities (no shared container)
        duin::Entity standalone1 = world.Entity("Standalone1").Set<Vec3>(1.0f, 0.0f, 0.0f);

        duin::Entity deepRoot = world.Entity("DeepRoot").Set<Vec3>(0.0f, 1.0f, 0.0f);
        duin::Entity deepChild1 = world.Entity("DeepChild1").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        deepChild1.ChildOf(deepRoot);
        duin::Entity deepChild2 = world.Entity("DeepChild2").Set<Vec3>(0.0f, 2.0f, 0.0f);
        deepChild2.ChildOf(deepChild1);
        duin::Entity deepChild3 = world.Entity("DeepChild3").Set<Camera>(60.0f, 0.2f, 500.0f, false);
        deepChild3.ChildOf(deepChild2);

        duin::Entity standalone2 = world.Entity("Standalone2").Set<Camera>(45.0f, 0.5f, 300.0f, true);

        duin::Entity mediumRoot =
            world.Entity("MediumRoot").Set<Vec3>(2.0f, 0.0f, 0.0f).Set<Camera>(75.0f, 0.1f, 800.0f, false);
        duin::Entity mediumChild = world.Entity("MediumChild").Set<Vec3>(2.5f, 1.0f, 0.0f);
        mediumChild.ChildOf(mediumRoot);

        // Pack all world-root entities (4 entities: 2 standalone + deepRoot + mediumRoot)
        duin::SceneBuilder sceneBuilder(&world);
        duin::PackedScene ps = sceneBuilder.PackScene(&world);
        CHECK(ps.entities.size() == 4);

        // Serialize and deserialize
        duin::JSONValue json = sceneBuilder.SerializeScene(ps);
        duin::PackedScene ps2 = sceneBuilder.DeserializeScene(json);

        // Instantiate as children of a new root
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        duin::Entity newRoot = world2.Entity("NewSceneRoot");
        sb2.InstantiateSceneAsChildren(ps2, newRoot);

        std::vector<duin::Entity> rootChildren = newRoot.GetChildren();
        CHECK(rootChildren.size() == 4);

        int standaloneCount = 0;
        int hierarchyCount = 0;

        for (auto &child : rootChildren)
        {
            if (child.GetChildren().empty())
                standaloneCount++;
            else
                hierarchyCount++;
        }

        CHECK(standaloneCount == 2);
        CHECK(hierarchyCount == 2);

        for (auto &child : rootChildren)
        {
            CHECK(child.GetParent() == newRoot);

            for (auto &grandchild : child.GetChildren())
            {
                CHECK(grandchild.GetParent() == child);
                for (auto &ggc : grandchild.GetChildren())
                    CHECK(ggc.GetParent() == grandchild);
            }
        }

        int checkedStandalone = 0;
        for (auto &child : rootChildren)
        {
            if (child.GetChildren().empty())
            {
                CHECK(child.GetParent() == newRoot);
                checkedStandalone++;
            }
        }
        CHECK(checkedStandalone == 2);
    }

    TEST_CASE("Verify parent-child relationships are preserved through serialization")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Root -> Parent1 -> Child1 -> Grandchild1
        //      -> Parent2 -> Child2
        duin::Entity root = world.Entity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        duin::Entity parent1 = world.Entity("Parent1").Set<Vec3>(1.0f, 0.0f, 0.0f);
        parent1.ChildOf(root);

        duin::Entity child1 = world.Entity("Child1").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        child1.ChildOf(parent1);

        duin::Entity grandchild1 = world.Entity("Grandchild1").Set<Vec3>(2.0f, 0.0f, 0.0f);
        grandchild1.ChildOf(child1);

        duin::Entity parent2 = world.Entity("Parent2").Set<Camera>(60.0f, 0.2f, 500.0f, false);
        parent2.ChildOf(root);

        duin::Entity child2 = world.Entity("Child2").Set<Vec3>(3.0f, 0.0f, 0.0f);
        child2.ChildOf(parent2);

        // Verify original relationships
        CHECK(parent1.GetParent() == root);
        CHECK(parent2.GetParent() == root);
        CHECK(child1.GetParent() == parent1);
        CHECK(child2.GetParent() == parent2);
        CHECK(grandchild1.GetParent() == child1);

        // Pack, serialize, deserialize via SceneBuilder instance
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({root});
        duin::JSONValue json = sb.SerializeScene(ps);
        std::string jsonStr = json.Write();

        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = sb.DeserializeScene(json2);

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps2, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        std::vector<duin::Entity> restoredRootChildren = restored.GetChildren();
        CHECK(restoredRootChildren.size() == 2);

        // Find Parent1 and Parent2
        duin::Entity *restoredParent1 = nullptr;
        duin::Entity *restoredParent2 = nullptr;
        for (auto &child : restoredRootChildren)
        {
            std::string bn = BaseName(child.GetName());
            if (bn == "Parent1")
                restoredParent1 = &child;
            if (bn == "Parent2")
                restoredParent2 = &child;
        }

        REQUIRE(restoredParent1 != nullptr);
        REQUIRE(restoredParent2 != nullptr);

        // Verify Parent1 hierarchy
        CHECK(restoredParent1->GetParent() == restored);
        std::vector<duin::Entity> parent1Children = restoredParent1->GetChildren();
        CHECK(parent1Children.size() == 1);
        CHECK(BaseName(parent1Children[0].GetName()) == "Child1");
        CHECK(parent1Children[0].GetParent() == *restoredParent1);

        std::vector<duin::Entity> child1Children = parent1Children[0].GetChildren();
        CHECK(child1Children.size() == 1);
        CHECK(BaseName(child1Children[0].GetName()) == "Grandchild1");
        CHECK(child1Children[0].GetParent() == parent1Children[0]);
        CHECK(child1Children[0].GetChildren().size() == 0);

        // Verify Parent2 hierarchy
        CHECK(restoredParent2->GetParent() == restored);
        std::vector<duin::Entity> parent2Children = restoredParent2->GetChildren();
        CHECK(parent2Children.size() == 1);
        CHECK(BaseName(parent2Children[0].GetName()) == "Child2");
        CHECK(parent2Children[0].GetParent() == *restoredParent2);
        CHECK(parent2Children[0].GetChildren().size() == 0);

        // Verify no cross-contamination
        bool child2InParent1 = false;
        for (auto &child : restoredParent1->GetChildren())
        {
            if (BaseName(child.GetName()) == "Child2")
                child2InParent1 = true;
        }
        CHECK(!child2InParent1);

        // Verify bidirectional consistency
        for (auto &child : restoredRootChildren)
        {
            CHECK(child.GetParent() == restored);
            for (auto &grandchild : child.GetChildren())
            {
                CHECK(grandchild.GetParent() == child);
                for (auto &ggc : grandchild.GetChildren())
                    CHECK(ggc.GetParent() == grandchild);
            }
        }
    }
}

} // namespace TestSceneBuilder
