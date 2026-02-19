#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("Complex Hierarchy Tests")
{
    TEST_CASE("Pack and Instantiate entity tree with mixed components at all levels")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Create a complex hierarchy:
        // Root (Vec3)
        //   ?? ChildA (Camera)
        //   ?   ?? GrandchildA1 (Vec3)
        //   ?   ?? GrandchildA2 (Vec3, Camera)
        //   ?? ChildB (Vec3, Camera)
        //       ?? GrandchildB1 (Camera)
        //           ?? GreatGrandchildB1 (Vec3)

        duin::Entity root = world.CreateEntity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        duin::Entity childA = world.CreateEntity("ChildA").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        childA.ChildOf(root);

        duin::Entity grandchildA1 = world.CreateEntity("GrandchildA1").Set<Vec3>(1.0f, 0.0f, 0.0f);
        grandchildA1.ChildOf(childA);

        duin::Entity grandchildA2 =
            world.CreateEntity("GrandchildA2").Set<Vec3>(2.0f, 0.0f, 0.0f).Set<Camera>(60.0f, 0.2f, 500.0f, false);
        grandchildA2.ChildOf(childA);

        duin::Entity childB =
            world.CreateEntity("ChildB").Set<Vec3>(0.0f, 1.0f, 0.0f).Set<Camera>(75.0f, 0.1f, 800.0f, false);
        childB.ChildOf(root);

        duin::Entity grandchildB1 = world.CreateEntity("GrandchildB1").Set<Camera>(45.0f, 0.5f, 300.0f, true);
        grandchildB1.ChildOf(childB);

        duin::Entity greatGrandchildB1 = world.CreateEntity("GreatGrandchildB1").Set<Vec3>(3.0f, 3.0f, 3.0f);
        greatGrandchildB1.ChildOf(grandchildB1);

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(root);

        CHECK(pe.name == "Root");
        CHECK(pe.children.size() == 2);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        // Verify structure
        CHECK(restored.GetName() == "Root");
        CHECK(restored.Has<Vec3>());
        CHECK(restored.GetMut<Vec3>() == Vec3{0.0f, 0.0f, 0.0f});

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 2);

        // Find ChildA and ChildB
        duin::Entity *restoredChildA = nullptr;
        duin::Entity *restoredChildB = nullptr;
        for (auto &child : children)
        {
            if (child.GetName() == "ChildA")
                restoredChildA = &child;
            if (child.GetName() == "ChildB")
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
            CHECK(grandchildrenB[0].GetName() == "GrandchildB1");
            CHECK(grandchildrenB[0].Has<Camera>());

            std::vector<duin::Entity> greatGrandchildren = grandchildrenB[0].GetChildren();
            CHECK(greatGrandchildren.size() == 1);
            if (greatGrandchildren.size() >= 1)
            {
                CHECK(greatGrandchildren[0].GetName() == "GreatGrandchildB1");
                CHECK(greatGrandchildren[0].Has<Vec3>());
                CHECK(greatGrandchildren[0].GetMut<Vec3>() == Vec3{3.0f, 3.0f, 3.0f});
            }
        }

        // Verify parent-child relationships are correctly established
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
            if (grandchildrenB[0].GetChildren().size() >= 1)
            {
                std::vector<duin::Entity> greatGrandchildren = grandchildrenB[0].GetChildren();
                CHECK(greatGrandchildren[0].GetParent() == grandchildrenB[0]);
            }
        }

        // Verify children recognize their parent
        duin::Entity parentOfChildA = restoredChildA->GetParent();
        CHECK(parentOfChildA.GetName() == "Root");
        CHECK(parentOfChildA == restored);
    }

    TEST_CASE("Scene with multiple complex entity hierarchies")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity sceneRoot = world.CreateEntity("SceneRoot");

        // Create first hierarchy - Player with equipment
        duin::Entity player =
            world.CreateEntity("Player").Set<Vec3>(10.0f, 0.0f, 0.0f).Set<Camera>(90.0f, 0.1f, 2000.0f, true);
        player.ChildOf(sceneRoot);

        duin::Entity playerCamera = world.CreateEntity("PlayerCamera").Set<Camera>(75.0f, 0.1f, 1500.0f, false);
        playerCamera.ChildOf(player);

        duin::Entity weapon = world.CreateEntity("Weapon").Set<Vec3>(0.5f, 1.2f, 0.3f);
        weapon.ChildOf(player);

        duin::Entity weaponEffect = world.CreateEntity("WeaponEffect").Set<Vec3>(0.1f, 0.1f, 0.1f);
        weaponEffect.ChildOf(weapon);

        // Create second hierarchy - Enemy with AI
        duin::Entity enemy =
            world.CreateEntity("Enemy").Set<Vec3>(-10.0f, 0.0f, 0.0f).Set<Camera>(60.0f, 0.2f, 1000.0f, false);
        enemy.ChildOf(sceneRoot);

        duin::Entity enemyWeapon = world.CreateEntity("EnemyWeapon").Set<Vec3>(0.3f, 1.0f, 0.2f);
        enemyWeapon.ChildOf(enemy);

        // Create third hierarchy - Environment
        duin::Entity environment = world.CreateEntity("Environment").Set<Vec3>(0.0f, -1.0f, 0.0f);
        environment.ChildOf(sceneRoot);

        duin::Entity light1 = world.CreateEntity("Light1").Set<Vec3>(5.0f, 10.0f, 5.0f);
        light1.ChildOf(environment);

        duin::Entity light2 = world.CreateEntity("Light2").Set<Vec3>(-5.0f, 10.0f, -5.0f);
        light2.ChildOf(environment);

        // Pack scene
        duin::PackedScene ps = duin::PackedScene::Pack({sceneRoot});
        ps.uuid = duin::UUID::FromStringHex("complexscene1111");
        ps.name = "ComplexScene";

        CHECK(ps.entities.size() == 3); // Player, Enemy, Environment

        // Serialize and deserialize
        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        std::string jsonStr = json.Write();
        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json2);

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewSceneRoot");
        duin::PackedScene::Instantiate(ps2, &world2);

        // Verify
        std::vector<duin::Entity> rootChildren = newRoot.GetChildren();
        CHECK(rootChildren.size() == 3);

        // Find and verify Player hierarchy
        duin::Entity *restoredPlayer = nullptr;
        duin::Entity *restoredEnemy = nullptr;
        duin::Entity *restoredEnvironment = nullptr;

        for (auto &child : rootChildren)
        {
            if (child.GetName() == "Player")
                restoredPlayer = &child;
            if (child.GetName() == "Enemy")
                restoredEnemy = &child;
            if (child.GetName() == "Environment")
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
            if (child.GetName() == "Weapon")
            {
                foundWeapon = true;
                CHECK(child.Has<Vec3>());
                std::vector<duin::Entity> weaponChildren = child.GetChildren();
                CHECK(weaponChildren.size() == 1);
                if (weaponChildren.size() >= 1)
                {
                    CHECK(weaponChildren[0].GetName() == "WeaponEffect");
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
            CHECK(enemyChildren[0].GetName() == "EnemyWeapon");
        }

        // Verify Environment
        CHECK(restoredEnvironment->Has<Vec3>());
        std::vector<duin::Entity> envChildren = restoredEnvironment->GetChildren();
        CHECK(envChildren.size() == 2);

        // Verify parent-child relationships
        CHECK(restoredPlayer->GetParent() == newRoot);
        CHECK(restoredEnemy->GetParent() == newRoot);
        CHECK(restoredEnvironment->GetParent() == newRoot);

        // Verify Player's children know their parent
        for (auto &child : playerChildren)
        {
            CHECK(child.GetParent() == *restoredPlayer);
        }

        // Verify Enemy's children know their parent
        for (auto &child : enemyChildren)
        {
            CHECK(child.GetParent() == *restoredEnemy);
        }

        // Verify Environment's children know their parent
        for (auto &child : envChildren)
        {
            CHECK(child.GetParent() == *restoredEnvironment);
        }

        // Verify deep hierarchy: WeaponEffect -> Weapon relationship
        if (foundWeapon)
        {
            for (auto &child : playerChildren)
            {
                if (child.GetName() == "Weapon")
                {
                    std::vector<duin::Entity> weaponChildren = child.GetChildren();
                    if (!weaponChildren.empty())
                    {
                        CHECK(weaponChildren[0].GetParent() == child);
                    }
                }
            }
        }
    }

    TEST_CASE("Wide hierarchy - entity with many children each having components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);

        // Create 10 children, each with different component combinations
        for (int i = 0; i < 10; ++i)
        {
            std::string name = "Child" + std::to_string(i);
            duin::Entity child = world.CreateEntity(name.c_str());

            // Alternate component types
            if (i % 3 == 0)
            {
                child.Set<Vec3>(static_cast<float>(i), 0.0f, 0.0f);
            }
            else if (i % 3 == 1)
            {
                child.Set<Camera>(static_cast<float>(60 + i), 0.1f, static_cast<float>(1000 + i * 100), i % 2 == 0);
            }
            else
            {
                child.Set<Vec3>(static_cast<float>(i), static_cast<float>(i), 0.0f);
                child.Set<Camera>(static_cast<float>(45 + i), 0.2f, static_cast<float>(800 + i * 50), false);
            }

            child.ChildOf(parent);
        }

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(parent);
        CHECK(pe.children.size() == 10);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        // Verify
        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 10);

        // Verify each child has correct components
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

        // We should have: 4 Vec3-only (i=0,3,6,9), 3 Camera-only (i=1,4,7), 3 Both (i=2,5,8)
        CHECK(vec3Count == 4);
        CHECK(cameraCount == 3);
        CHECK(bothCount == 3);

        // Verify all children recognize their parent
        for (auto &child : children)
        {
            CHECK(child.GetParent() == restored);
        }

        // Verify parent knows all its children
        CHECK(restored.GetChildren().size() == 10);
    }

    TEST_CASE("Deep and wide - 5 levels deep with multiple children at each level")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Build a tree structure:
        // Level 0: Root (Vec3)
        // Level 1: 3 children (Vec3, Camera, or both)
        // Level 2: Each L1 has 2 children
        // Level 3: Each L2 has 2 children
        // Level 4: Each L3 has 1 child

        duin::Entity root = world.CreateEntity("L0_Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        for (int i = 0; i < 3; ++i)
        {
            std::string l1Name = "L1_" + std::to_string(i);
            duin::Entity l1 = world.CreateEntity(l1Name.c_str());

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
                duin::Entity l2 = world.CreateEntity(l2Name.c_str()).Set<Vec3>(static_cast<float>(i + j), 1.0f, 0.0f);
                l2.ChildOf(l1);

                for (int k = 0; k < 2; ++k)
                {
                    std::string l3Name = "L3_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                    duin::Entity l3 = world.CreateEntity(l3Name.c_str())
                                          .Set<Camera>(static_cast<float>(45 + k), 0.1f, 800.0f, k == 0);
                    l3.ChildOf(l2);

                    std::string l4Name = "L4_" + std::to_string(i) + "_" + std::to_string(j) + "_" + std::to_string(k);
                    duin::Entity l4 =
                        world.CreateEntity(l4Name.c_str())
                            .Set<Vec3>(static_cast<float>(i), static_cast<float>(j), static_cast<float>(k));
                    l4.ChildOf(l3);
                }
            }
        }

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(root);
        CHECK(pe.children.size() == 3);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        // Verify structure
        CHECK(restored.GetName() == "L0_Root");
        CHECK(restored.Has<Vec3>());

        std::vector<duin::Entity> l1Children = restored.GetChildren();
        CHECK(l1Children.size() == 3);

        // Check that we have the correct total depth
        // Pick one branch and verify it goes 5 levels deep
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

                        // Verify the deepest node has no children
                        std::vector<duin::Entity> l5Children = l4Children[0].GetChildren();
                        CHECK(l5Children.empty());

                        // Verify parent-child relationships at each level
                        CHECK(l4Children[0].GetParent() == l3Children[0]);

                        // Verify that GetChildren returns correct counts at each level
                        CHECK(l4Children[0].GetChildren().size() == 0);
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

        // Verify that GetChildren returns correct counts at each level
        CHECK(restored.GetChildren().size() == 3);
    }

    TEST_CASE("Mixed scene - some entities with deep hierarchies, others standalone")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity sceneRoot = world.CreateEntity("SceneRoot");

        // Add a standalone entity
        duin::Entity standalone1 = world.CreateEntity("Standalone1").Set<Vec3>(1.0f, 0.0f, 0.0f);
        standalone1.ChildOf(sceneRoot);

        // Add a deep hierarchy
        duin::Entity deepRoot = world.CreateEntity("DeepRoot").Set<Vec3>(0.0f, 1.0f, 0.0f);
        deepRoot.ChildOf(sceneRoot);

        duin::Entity deepChild1 = world.CreateEntity("DeepChild1").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        deepChild1.ChildOf(deepRoot);

        duin::Entity deepChild2 = world.CreateEntity("DeepChild2").Set<Vec3>(0.0f, 2.0f, 0.0f);
        deepChild2.ChildOf(deepChild1);

        duin::Entity deepChild3 = world.CreateEntity("DeepChild3").Set<Camera>(60.0f, 0.2f, 500.0f, false);
        deepChild3.ChildOf(deepChild2);

        // Add another standalone
        duin::Entity standalone2 = world.CreateEntity("Standalone2").Set<Camera>(45.0f, 0.5f, 300.0f, true);
        standalone2.ChildOf(sceneRoot);

        // Add a medium hierarchy
        duin::Entity mediumRoot =
            world.CreateEntity("MediumRoot").Set<Vec3>(2.0f, 0.0f, 0.0f).Set<Camera>(75.0f, 0.1f, 800.0f, false);
        mediumRoot.ChildOf(sceneRoot);

        duin::Entity mediumChild = world.CreateEntity("MediumChild").Set<Vec3>(2.5f, 1.0f, 0.0f);
        mediumChild.ChildOf(mediumRoot);

        // Pack scene
        duin::PackedScene ps = duin::PackedScene::Pack({sceneRoot});
        CHECK(ps.entities.size() == 4); // 2 standalone + 2 hierarchies

        // Serialize, deserialize, and instantiate
        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewSceneRoot");
        duin::PackedScene::Instantiate(ps2, &world2);

        // Verify
        std::vector<duin::Entity> rootChildren = newRoot.GetChildren();
        CHECK(rootChildren.size() == 4);

        // Count entities with and without children
        int standaloneCount = 0;
        int hierarchyCount = 0;

        for (auto &child : rootChildren)
        {
            std::vector<duin::Entity> children = child.GetChildren();
            if (children.empty())
                standaloneCount++;
            else
                hierarchyCount++;
        }

        CHECK(standaloneCount == 2);
        CHECK(hierarchyCount == 2);

        // Verify parent-child relationships for hierarchies
        for (auto &child : rootChildren)
        {
            CHECK(child.GetParent() == newRoot);

            std::vector<duin::Entity> childChildren = child.GetChildren();
            if (!childChildren.empty())
            {
                // Verify grandchildren know their parent
                for (auto &grandchild : childChildren)
                {
                    CHECK(grandchild.GetParent() == child);

                    // Check for great-grandchildren
                    std::vector<duin::Entity> greatGrandchildren = grandchild.GetChildren();
                    for (auto &ggc : greatGrandchildren)
                    {
                        CHECK(ggc.GetParent() == grandchild);
                    }
                }
            }
        }

        // Verify standalone entities have no children
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

        // Create a hierarchy: Root -> Parent1 -> Child1 -> Grandchild1
        //                           -> Parent2 -> Child2
        duin::Entity root = world.CreateEntity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);

        duin::Entity parent1 = world.CreateEntity("Parent1").Set<Vec3>(1.0f, 0.0f, 0.0f);
        parent1.ChildOf(root);

        duin::Entity child1 = world.CreateEntity("Child1").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        child1.ChildOf(parent1);

        duin::Entity grandchild1 = world.CreateEntity("Grandchild1").Set<Vec3>(2.0f, 0.0f, 0.0f);
        grandchild1.ChildOf(child1);

        duin::Entity parent2 = world.CreateEntity("Parent2").Set<Camera>(60.0f, 0.2f, 500.0f, false);
        parent2.ChildOf(root);

        duin::Entity child2 = world.CreateEntity("Child2").Set<Vec3>(3.0f, 0.0f, 0.0f);
        child2.ChildOf(parent2);

        // Verify original hierarchy relationships
        CHECK(parent1.GetParent() == root);
        CHECK(parent2.GetParent() == root);
        CHECK(child1.GetParent() == parent1);
        CHECK(child2.GetParent() == parent2);
        CHECK(grandchild1.GetParent() == child1);

        CHECK(root.GetChildren().size() == 2);
        CHECK(parent1.GetChildren().size() == 1);
        CHECK(parent2.GetChildren().size() == 1);
        CHECK(child1.GetChildren().size() == 1);
        CHECK(grandchild1.GetChildren().size() == 0);

        // Pack and serialize
        duin::PackedEntity pe = duin::PackedEntity::Pack(root);
        duin::JSONValue json = duin::PackedEntity::Serialize(pe);
        std::string jsonStr = json.Write();

        // Deserialize
        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedEntity pe2 = duin::PackedEntity::Deserialize(json2);

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe2, restored);

        // Verify restored hierarchy structure
        std::vector<duin::Entity> restoredRootChildren = restored.GetChildren();
        CHECK(restoredRootChildren.size() == 2);

        // Find Parent1 and Parent2
        duin::Entity *restoredParent1 = nullptr;
        duin::Entity *restoredParent2 = nullptr;
        for (auto &child : restoredRootChildren)
        {
            if (child.GetName() == "Parent1")
                restoredParent1 = &child;
            if (child.GetName() == "Parent2")
                restoredParent2 = &child;
        }

        REQUIRE(restoredParent1 != nullptr);
        REQUIRE(restoredParent2 != nullptr);

        // Verify Parent1 hierarchy
        CHECK(restoredParent1->GetParent() == restored);
        std::vector<duin::Entity> parent1Children = restoredParent1->GetChildren();
        CHECK(parent1Children.size() == 1);
        CHECK(parent1Children[0].GetName() == "Child1");
        CHECK(parent1Children[0].GetParent() == *restoredParent1);

        std::vector<duin::Entity> child1Children = parent1Children[0].GetChildren();
        CHECK(child1Children.size() == 1);
        CHECK(child1Children[0].GetName() == "Grandchild1");
        CHECK(child1Children[0].GetParent() == parent1Children[0]);
        CHECK(child1Children[0].GetChildren().size() == 0);

        // Verify Parent2 hierarchy
        CHECK(restoredParent2->GetParent() == restored);
        std::vector<duin::Entity> parent2Children = restoredParent2->GetChildren();
        CHECK(parent2Children.size() == 1);
        CHECK(parent2Children[0].GetName() == "Child2");
        CHECK(parent2Children[0].GetParent() == *restoredParent2);
        CHECK(parent2Children[0].GetChildren().size() == 0);

        // Verify no cross-contamination: Child2 should not be a child of Parent1
        bool child2InParent1 = false;
        for (auto &child : restoredParent1->GetChildren())
        {
            if (child.GetName() == "Child2")
                child2InParent1 = true;
        }
        CHECK(!child2InParent1);

        // Verify bidirectional consistency: if A.GetChildren() contains B, then B.GetParent() == A
        for (auto &child : restoredRootChildren)
        {
            CHECK(child.GetParent() == restored);
            for (auto &grandchild : child.GetChildren())
            {
                CHECK(grandchild.GetParent() == child);
                for (auto &ggc : grandchild.GetChildren())
                {
                    CHECK(ggc.GetParent() == grandchild);
                }
            }
        }
    }
}

} // namespace TestSceneBuilder
