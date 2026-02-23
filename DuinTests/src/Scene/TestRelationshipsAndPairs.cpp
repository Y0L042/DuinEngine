#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>
#include "Defines.h"
#include <algorithm>

namespace TestSceneBuilder
{

TEST_SUITE("Relationship and Pair Tests")
{
    struct Health { float value = 100.0f; };
    struct Damage { float value = 10.0f; };
    struct Team { int id = 0; };

    // Relationship tags
    struct Likes {};
    struct Hates {};
    struct Targets {};
    struct Owns {};

    TEST_CASE("Pack and Instantiate entity with pair relationships")
    {
        duin::World world;
        duin::SceneBuilder sceneBuilder(&world);
        world.Component<Vec3>();
        world.Component<Likes>();
        world.Component<Targets>();

        duin::Entity player = world.CreateEntity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity enemy = world.CreateEntity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);
        duin::Entity battie = world.CreateEntity().Set<Vec3>(10.0f, 0.0f, 0.0f);

        // Add relationship: player targets enemy
        player.Add<Targets>(enemy);
        player.Add<Targets>(battie);

        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        // Verify relationship is packed
        MSG_CHECK(pe.name, pe.name == "Player");
        //MSG_CHECK(pe.pairs.size(), pe.pairs.size() == 1); // Doesnt account for builtin pairs
        
        auto targetsPair = std::find_if(pe.pairs.begin(), pe.pairs.end(), 
            [](const duin::PackedPair& pair) { return pair.relationshipName == "Targets"; });
        MSG_CHECK(targetsPair != pe.pairs.end(), targetsPair != pe.pairs.end());
        if (targetsPair != pe.pairs.end()) {
            MSG_CHECK(targetsPair->targetName, targetsPair->targetName == "Enemy");
        }

        // Instantiate and verify
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Targets>();

        // Create target entity first (must exist before instantiating pair)
        duin::Entity enemy2 = world2.CreateEntity("Enemy");

        duin::Entity player2 = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, player2);

        MSG_CHECK(player2.GetName(), player2.GetName() == "Player");
        CHECK(player2.HasPair(world2.Component<Targets>().GetID(), enemy2.GetID()));
    }

    TEST_CASE("Parent-child relationships as pairs")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.CreateEntity("Child").Set<Vec3>(1.0f, 0.0f, 0.0f);
        
        child.ChildOf(parent);

        // Verify parent-child relationship
        CHECK(child.GetParent() == parent);
        CHECK(parent.GetChildren().size() == 1);

        duin::PackedEntity pe = duin::PackedEntity::Pack(parent);
        
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Child");

        // Instantiate and verify relationship preserved
        duin::World world2;
        world2.Component<Vec3>();

        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        std::vector<duin::Entity> restoredChildren = restored.GetChildren();
        CHECK(restoredChildren.size() == 1);
        CHECK(restoredChildren[0].GetParent() == restored);
    }

    TEST_CASE("Multiple relationship pairs on single entity")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Team>();
        world.Component<Owns>();
        world.Component<Targets>();

        duin::Entity player = world.CreateEntity("Player")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Team>({1});

        duin::Entity weapon = world.CreateEntity("Weapon").Set<Vec3>(0.5f, 1.0f, 0.0f);
        duin::Entity enemy = world.CreateEntity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);

        // Player owns weapon
        player.Add<Owns>(weapon);
        
        // Player targets enemy
        player.Add<Targets>(enemy);

        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        CHECK(pe.name == "Player");
        // Pairs should be serialized as part of component data or relationships
    }

    TEST_CASE("Symmetric relationships")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Likes>();

        duin::Entity alice = world.CreateEntity("Alice").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity bob = world.CreateEntity("Bob").Set<Vec3>(5.0f, 0.0f, 0.0f);

        // Mutual relationship
        alice.Add<Likes>(bob);
        bob.Add<Likes>(alice);

        duin::PackedEntity alicePacked = duin::PackedEntity::Pack(alice);
        duin::PackedEntity bobPacked = duin::PackedEntity::Pack(bob);

        CHECK(alicePacked.name == "Alice");
        CHECK(bobPacked.name == "Bob");
        CHECK(alicePacked.pairs.size() == 1);
        CHECK(bobPacked.pairs.size() == 1);
        
        auto aliceLikesPair = std::find_if(alicePacked.pairs.begin(), alicePacked.pairs.end(),
            [](const duin::PackedPair& pair) { return pair.relationshipName == "Likes"; });
        if (aliceLikesPair != alicePacked.pairs.end()) {
            CHECK(aliceLikesPair->targetName == "Bob");
        }
        
        auto bobLikesPair = std::find_if(bobPacked.pairs.begin(), bobPacked.pairs.end(),
            [](const duin::PackedPair& pair) { return pair.relationshipName == "Likes"; });
        if (bobLikesPair != bobPacked.pairs.end()) {
            CHECK(bobLikesPair->targetName == "Alice");
        }
    }

    TEST_CASE("PackedPair serialization and deserialization")
    {
        // Create a sample PackedPair
        duin::PackedPair original;
        original.relationshipName = "Targets";
        original.targetName = "Enemy";
        original.jsonData = "";

        // Serialize to JSON
        duin::JSONValue json = duin::PackedPair::Serialize(original);

        // Verify JSON structure
        CHECK(json.HasMember("relationship"));
        CHECK(json.HasMember("target"));
        CHECK(json.GetMember("relationship").GetString() == "Targets");
        CHECK(json.GetMember("target").GetString() == "Enemy");

        // Deserialize back
        duin::PackedPair restored = duin::PackedPair::Deserialize(json);

        // Verify restored data
        CHECK(restored.relationshipName == "Targets");
        CHECK(restored.targetName == "Enemy");
        CHECK(restored.jsonData.empty());
    }

    TEST_CASE("PackedPair with IsA relationship (prefabs)")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();

        // Create a prefab
        duin::Entity enemyPrefab = world.CreateEntity("EnemyPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f});

        // Create an instance using IsA
        duin::Entity enemy1 = world.CreateEntity("Enemy1").IsA(enemyPrefab);

        // Pack the entity
        duin::PackedEntity pe = duin::PackedEntity::Pack(enemy1);

        // Verify IsA relationship is packed
        CHECK(pe.name == "Enemy1");
        CHECK(pe.pairs.size() == 1);
        
        auto isAPair = std::find_if(pe.pairs.begin(), pe.pairs.end(),
            [](const duin::PackedPair& pair) { return pair.relationshipName == "IsA"; });
        CHECK(isAPair != pe.pairs.end());
        if (isAPair != pe.pairs.end()) {
            CHECK(isAPair->targetName == "EnemyPrefab");
        }
    }

    TEST_CASE("PackedEntity JSON roundtrip with pairs")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Targets>();

        duin::Entity player = world.CreateEntity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity enemy = world.CreateEntity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);

        player.Add<Targets>(enemy);

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        // Serialize to JSON
        duin::JSONValue json = duin::PackedEntity::Serialize(pe);

        // Verify pairs array exists in JSON
        CHECK(json.HasMember("pairs"));
        CHECK(json.GetMember("pairs").IsArray());

        // Deserialize
        duin::PackedEntity restored = duin::PackedEntity::Deserialize(json);

        // Verify pairs are restored
        MSG_CHECK(restored.pairs.size() , restored.pairs.size() == 1);
        
        auto restoredTargetsPair = std::find_if(restored.pairs.begin(), restored.pairs.end(),
            [](const duin::PackedPair& pair) { return pair.relationshipName == "Targets"; });
        MSG_CHECK(restoredTargetsPair != restored.pairs.end(), restoredTargetsPair != restored.pairs.end());
        if (restoredTargetsPair != restored.pairs.end()) {
            MSG_CHECK(restoredTargetsPair->targetName, restoredTargetsPair->targetName == "Enemy");
        }
    }
}

} // namespace TestSceneBuilder
