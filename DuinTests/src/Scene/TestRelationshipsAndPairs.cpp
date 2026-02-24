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

static std::string BaseName(const std::string &name)
{
    auto pos = name.find('#');
    return pos != std::string::npos ? name.substr(0, pos) : name;
}

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
        world.Component<Vec3>();
        world.Component<Likes>();
        world.Component<Targets>();

        duin::Entity player = world.CreateEntity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity enemy = world.CreateEntity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);
        duin::Entity battie = world.CreateEntity().Set<Vec3>(10.0f, 0.0f, 0.0f);

        // Add relationship: player targets enemy (and unnamed battie)
        player.Add<Targets>(enemy);
        player.Add<Targets>(battie);

        // Pack player and enemy together for cross-entity relationship resolution
        duin::SceneBuilder sceneBuilder(&world);
        duin::PackedScene ps = sceneBuilder.PackScene({player, enemy, battie});

        // Verify player's pair is packed
        const duin::PackedEntity &playerPacked = ps.entities[0];
        MSG_CHECK(playerPacked.name, playerPacked.name == "Player");
        //MSG_CHECK(playerPacked.pairs.size(), playerPacked.pairs.size() == 1); // Doesn't account for builtin pairs

        auto targetsPair = std::find_if(playerPacked.pairs.begin(), playerPacked.pairs.end(),
            [](const duin::PackedPair &pair) { return pair.relationshipName == "Targets"; });
        MSG_CHECK(targetsPair != playerPacked.pairs.end(), targetsPair != playerPacked.pairs.end());
        if (targetsPair != playerPacked.pairs.end())
        {
            MSG_CHECK(targetsPair->targetName, targetsPair->targetName == "Enemy");
        }

        // Instantiate and verify
        duin::World world2;
        world2.Component<Vec3>();
        duin::Entity te = world2.Component<Targets>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        DN_INFO("World Size: {}", worldEntities.size());
        world2.IterateChildren([](duin::Entity child) { DN_INFO("NAME: {}", child.GetName()); });

        REQUIRE(worldEntities.size() >= 2);

        duin::Entity player2 = world2.Lookup("Player");
        duin::Entity enemy2 = world2.Lookup("Enemy");

        REQUIRE(player2.IsValid());
        REQUIRE(enemy2.IsValid());

        CHECK(player2.Has(te, enemy2));
        CHECK(player2.Has<Targets>(enemy2));
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

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Child");

        // Instantiate and verify relationship preserved
        duin::World world2;
        world2.Component<Vec3>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        std::vector<duin::Entity> restoredChildren = restored.GetChildren();
        CHECK(restoredChildren.size() == 1);
        if (restoredChildren.size() >= 1)
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

        // Player owns weapon, targets enemy
        player.Add<Owns>(weapon);
        player.Add<Targets>(enemy);

        // Pack all three together for cross-entity relationship resolution
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({player, weapon, enemy});

        CHECK(ps.entities.size() == 3);
        CHECK(ps.entities[0].name == "Player");
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

        duin::SceneBuilder sbAlice(&world);
        duin::PackedScene psAlice = sbAlice.PackScene({alice});

        duin::SceneBuilder sbBob(&world);
        duin::PackedScene psBob = sbBob.PackScene({bob});

        CHECK(psAlice.entities[0].name == "Alice");
        CHECK(psBob.entities[0].name == "Bob");
        CHECK(psAlice.entities[0].pairs.size() == 1);
        CHECK(psBob.entities[0].pairs.size() == 1);

        auto aliceLikesPair = std::find_if(psAlice.entities[0].pairs.begin(), psAlice.entities[0].pairs.end(),
            [](const duin::PackedPair &pair) { return pair.relationshipName == "Likes"; });
        if (aliceLikesPair != psAlice.entities[0].pairs.end())
        {
            CHECK(aliceLikesPair->targetName == "Bob");
        }

        auto bobLikesPair = std::find_if(psBob.entities[0].pairs.begin(), psBob.entities[0].pairs.end(),
            [](const duin::PackedPair &pair) { return pair.relationshipName == "Likes"; });
        if (bobLikesPair != psBob.entities[0].pairs.end())
        {
            CHECK(bobLikesPair->targetName == "Alice");
        }
    }

    TEST_CASE("PackedPair serialization and deserialization")
    {
        // Create a sample PackedPair — this test specifically validates the lower-level API
        duin::PackedPair original;
        original.relationshipName = "Targets";
        original.targetName = "Enemy";
        original.jsonData = "";

        // Serialize to JSON
        duin::SceneBuilder builder(nullptr);
        duin::JSONValue json = builder.SerializePair(original);

        // Verify JSON structure
        CHECK(json.HasMember("relationship"));
        CHECK(json.HasMember("target"));
        CHECK(json.GetMember("relationship").GetString() == "Targets");
        CHECK(json.GetMember("target").GetString() == "Enemy");

        // Deserialize back
        duin::PackedPair restored = builder.DeserializePair(json);

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
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({enemy1});

        // Verify IsA relationship is packed
        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Enemy1");
        CHECK(ps.entities[0].pairs.size() == 1);

        auto isAPair = std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
            [](const duin::PackedPair &pair) { return pair.relationshipName == "IsA"; });
        CHECK(isAPair != ps.entities[0].pairs.end());
        if (isAPair != ps.entities[0].pairs.end())
        {
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

        // Pack player and enemy together for cross-entity relationship resolution
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({player, enemy});

        // Serialize to JSON
        duin::JSONValue json = sb.SerializeScene(ps);

        // Verify entities array exists in JSON
        CHECK(json.HasMember("entities"));
        CHECK(json.GetMember("entities").IsArray());

        // Deserialize
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        // Verify pairs are restored in player's packed data
        CHECK(ps2.entities.size() == 2);
        if (ps2.entities.size() >= 1)
        {
            const duin::PackedEntity &playerRestored = ps2.entities[0];
            MSG_CHECK(playerRestored.pairs.size(), playerRestored.pairs.size() == 1);

            auto restoredTargetsPair = std::find_if(playerRestored.pairs.begin(), playerRestored.pairs.end(),
                [](const duin::PackedPair &pair) { return pair.relationshipName == "Targets"; });
            MSG_CHECK(restoredTargetsPair != playerRestored.pairs.end(), restoredTargetsPair != playerRestored.pairs.end());
            if (restoredTargetsPair != playerRestored.pairs.end())
            {
                MSG_CHECK(restoredTargetsPair->targetName, restoredTargetsPair->targetName == "Enemy");
            }
        }
    }
}

} // namespace TestSceneBuilder
