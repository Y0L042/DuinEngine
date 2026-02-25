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
    // --- Component types (have data) ---
    struct Health
    {
        float value = 100.0f;
        bool operator==(const Health &o) const
        {
            return value == o.value;
        }
    };
    struct Damage
    {
        float value = 10.0f;
        bool operator==(const Damage &o) const
        {
            return value == o.value;
        }
    };
    struct Team
    {
        int id = 0;
        bool operator==(const Team &o) const
        {
            return id == o.id;
        }
    };
    struct Strength
    {
        int value = 1;
        bool operator==(const Strength &o) const
        {
            return value == o.value;
        }
    };

    // --- Tag types (no data, zero-size) ---
    struct Likes
    {
    };
    struct Hates
    {
    };
    struct Targets
    {
    };
    struct Owns
    {
    };
    struct Ally
    {
    };
    struct Enemy
    {
    };
    struct Follows
    {
    };

    // =========================================================================
    // ORIGINAL TESTS (preserved)
    // =========================================================================

    TEST_CASE("Pack and Instantiate entity with pair relationships")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Likes>();
        world.Component<Targets>();

        duin::Entity player = world.Entity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity enemy = world.Entity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);
        duin::Entity battie = world.Entity().Set<Vec3>(10.0f, 0.0f, 0.0f);

        // Add relationship: player targets enemy (and unnamed battie)
        player.Add<Targets>(enemy);
        player.Add<Targets>(battie);

        // Pack player and enemy together for cross-entity relationship resolution
        duin::SceneBuilder sceneBuilder(&world);
        duin::PackedScene ps = sceneBuilder.PackScene({player, enemy, battie});

        // Verify player's pair is packed
        const duin::PackedEntity &playerPacked = ps.entities[0];
        MSG_CHECK(playerPacked.name, playerPacked.name == "Player");

        auto targetsPair =
            std::find_if(playerPacked.pairs.begin(), playerPacked.pairs.end(),
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

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.Entity("Child").Set<Vec3>(1.0f, 0.0f, 0.0f);

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

        duin::Entity player = world.Entity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f).Set<Team>({1});

        duin::Entity weapon = world.Entity("Weapon").Set<Vec3>(0.5f, 1.0f, 0.0f);
        duin::Entity foe = world.Entity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);

        // Player owns weapon, targets enemy
        player.Add<Owns>(weapon);
        player.Add<Targets>(foe);

        // Pack all three together for cross-entity relationship resolution
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({player, weapon, foe});

        CHECK(ps.entities.size() == 3);
        CHECK(ps.entities[0].name == "Player");
    }

    TEST_CASE("Symmetric relationships")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Likes>();

        duin::Entity alice = world.Entity("Alice").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity bob = world.Entity("Bob").Set<Vec3>(5.0f, 0.0f, 0.0f);

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

        auto aliceLikesPair =
            std::find_if(psAlice.entities[0].pairs.begin(), psAlice.entities[0].pairs.end(),
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
        duin::Entity enemyPrefab = world.Entity("EnemyPrefab").Set<Vec3>(0.0f, 0.0f, 0.0f).Set<Health>({100.0f});

        // Create an instance using IsA
        duin::Entity enemy1 = world.Entity("Enemy1").IsA(enemyPrefab);

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

        duin::Entity player = world.Entity("Player").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity foe = world.Entity("Enemy").Set<Vec3>(10.0f, 0.0f, 0.0f);

        player.Add<Targets>(foe);

        // Pack player and enemy together for cross-entity relationship resolution
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({player, foe});

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

            auto restoredTargetsPair =
                std::find_if(playerRestored.pairs.begin(), playerRestored.pairs.end(),
                             [](const duin::PackedPair &pair) { return pair.relationshipName == "Targets"; });
            MSG_CHECK(restoredTargetsPair != playerRestored.pairs.end(),
                      restoredTargetsPair != playerRestored.pairs.end());
            if (restoredTargetsPair != playerRestored.pairs.end())
            {
                MSG_CHECK(restoredTargetsPair->targetName, restoredTargetsPair->targetName == "Enemy");
            }
        }
    }

    // =========================================================================
    // COMBINATION MATRIX: (Tag, Component, Entity) x (Tag, Component, Entity)
    //
    // Relationship \ Target   | Tag   | Component | Entity
    // ------------------------|-------|-----------|-------
    // Tag relationship        |  [1]  |    [2]    |  [3]
    // Component relationship  |  [4]  |    [5]    |  [6]
    // Entity relationship     |  [7]  |    [8]    |  [9]
    //
    // [1] Tag rel + Tag target   — e.g., (Targets, Ally)      both zero-size
    // [2] Tag rel + Cmp target   — e.g., (Targets, Health)    rel=tag, tgt=component
    // [3] Tag rel + Entity tgt   — e.g., (Targets, enemy)     rel=tag, tgt=runtime entity
    // [4] Cmp rel + Tag target   — e.g., (Damage, Ally)       rel=component, tgt=tag
    // [5] Cmp rel + Cmp target   — e.g., (Damage, Health)     both components (type-pair)
    // [6] Cmp rel + Entity tgt   — e.g., (Damage, enemy)      rel=component, tgt=runtime entity
    // [7] Entity rel + Tag tgt   — e.g., (weapon, Ally)       rel=runtime entity, tgt=tag
    // [8] Entity rel + Cmp tgt   — e.g., (weapon, Health)     rel=runtime entity, tgt=component
    // [9] Entity rel + Entity tgt— e.g., (weapon, enemy)      both runtime entities
    // =========================================================================

    // [1] Tag relationship → Tag target
    TEST_CASE("Pair: tag relationship with tag target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Ally>();

        duin::Entity unit = world.Entity("Unit");
        // (Targets, Ally): both are zero-size tag types
        unit.Add<Targets, Ally>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        REQUIRE(ps.entities.size() == 1);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Targets" && p.targetName == "Ally";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == true);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 1);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Targets" && p.targetName == "Ally";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
        if (pair2 != ps2.entities[0].pairs.end())
        {
            CHECK(pair2->relationshipIsComponent == true);
            CHECK(pair2->targetIsComponent == true);
        }
    }

    // [1] Tag+Tag — instantiate and verify
    TEST_CASE("Pair: tag relationship with tag target — instantiate")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Ally>();

        duin::Entity unit = world.Entity("Unit");
        unit.Add<Targets, Ally>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        duin::World world2;
        world2.Component<Targets>();
        world2.Component<Ally>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        REQUIRE(unit2.IsValid());
        CHECK(unit2.Has<Targets, Ally>());
    }

    // [2] Tag relationship → Component target
    TEST_CASE("Pair: tag relationship with component target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Health>();

        duin::Entity unit = world.Entity("Unit");
        // (Targets, Health): rel=tag, tgt=component
        unit.Add<Targets, Health>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        REQUIRE(ps.entities.size() == 1);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Targets" && p.targetName == "Health";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == true);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 1);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Targets" && p.targetName == "Health";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
    }

    // [2] Tag+Component — instantiate and verify
    TEST_CASE("Pair: tag relationship with component target — instantiate")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Health>();

        duin::Entity unit = world.Entity("Unit");
        unit.Add<Targets, Health>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        duin::World world2;
        world2.Component<Targets>();
        world2.Component<Health>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        REQUIRE(unit2.IsValid());
        CHECK(unit2.Has<Targets, Health>());
    }

    // [3] Tag relationship → Entity target (existing test improved)
    TEST_CASE("Pair: tag relationship with entity target — pack and instantiate")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Targets>();

        duin::Entity attacker = world.Entity("Attacker").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity foe = world.Entity("Foe").Set<Vec3>(5.0f, 0.0f, 0.0f);
        attacker.Add<Targets>(foe);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({attacker, foe});

        REQUIRE(ps.entities.size() == 2);
        auto pair = std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
                                 [](const duin::PackedPair &p) { return p.relationshipName == "Targets"; });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->targetName == "Foe");
            CHECK(pair->targetIsComponent == false);
            CHECK(pair->relationshipIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 2);
        auto pair2 = std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(),
                                  [](const duin::PackedPair &p) { return p.relationshipName == "Targets"; });
        CHECK(pair2 != ps2.entities[0].pairs.end());

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Targets>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity attacker2 = world2.Lookup("Attacker");
        duin::Entity foe2 = world2.Lookup("Foe");
        REQUIRE(attacker2.IsValid());
        REQUIRE(foe2.IsValid());
        CHECK(attacker2.Has<Targets>(foe2));
    }

    // [4] Component relationship → Tag target
    TEST_CASE("Pair: component relationship with tag target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Damage>();
        world.Component<Ally>();

        duin::Entity unit = world.Entity("Unit");
        // (Damage, Ally): rel=component, tgt=tag
        unit.Add<Damage, Ally>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        REQUIRE(ps.entities.size() == 1);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Damage" && p.targetName == "Ally";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == true);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 1);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Damage" && p.targetName == "Ally";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
    }

    // [4] Component+Tag — instantiate and verify
    TEST_CASE("Pair: component relationship with tag target — instantiate")
    {
        duin::World world;
        world.Component<Damage>();
        world.Component<Ally>();

        duin::Entity unit = world.Entity("Unit");
        unit.Add<Damage, Ally>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        duin::World world2;
        world2.Component<Damage>();
        world2.Component<Ally>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        REQUIRE(unit2.IsValid());
        CHECK(unit2.Has<Damage, Ally>());
    }

    // [5] Component relationship → Component target
    TEST_CASE("Pair: component relationship with component target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Damage>();
        world.Component<Health>();

        duin::Entity unit = world.Entity("Unit");
        // (Damage, Health): both components
        unit.Add<Damage, Health>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        REQUIRE(ps.entities.size() == 1);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Damage" && p.targetName == "Health";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == true);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 1);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Damage" && p.targetName == "Health";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
        if (pair2 != ps2.entities[0].pairs.end())
        {
            CHECK(pair2->relationshipIsComponent == true);
            CHECK(pair2->targetIsComponent == true);
        }
    }

    // [5] Component+Component — instantiate and verify
    TEST_CASE("Pair: component relationship with component target — instantiate")
    {
        duin::World world;
        world.Component<Damage>();
        world.Component<Health>();

        duin::Entity unit = world.Entity("Unit");
        unit.Add<Damage, Health>();

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        duin::World world2;
        world2.Component<Damage>();
        world2.Component<Health>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        REQUIRE(unit2.IsValid());
        CHECK(unit2.Has<Damage, Health>());
    }

    // [6] Component relationship → Entity target
    TEST_CASE("Pair: component relationship with entity target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Damage>();

        duin::Entity attacker = world.Entity("Attacker");
        duin::Entity victim = world.Entity("Victim");
        // (Damage, victim): rel=component, tgt=runtime entity
        attacker.Add<Damage>(victim);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({attacker, victim});

        REQUIRE(ps.entities.size() == 2);
        auto pair = std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
                                 [](const duin::PackedPair &p) { return p.relationshipName == "Damage"; });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->targetName == "Victim");
            CHECK(pair->relationshipIsComponent == true);
            CHECK(pair->targetIsComponent == false);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 2);
        auto pair2 = std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(),
                                  [](const duin::PackedPair &p) { return p.relationshipName == "Damage"; });
        CHECK(pair2 != ps2.entities[0].pairs.end());
        if (pair2 != ps2.entities[0].pairs.end())
        {
            CHECK(pair2->targetIsComponent == false);
        }
    }

    // [6] Component+Entity — instantiate and verify
    TEST_CASE("Pair: component relationship with entity target — instantiate")
    {
        duin::World world;
        world.Component<Damage>();

        duin::Entity attacker = world.Entity("Attacker");
        duin::Entity victim = world.Entity("Victim");
        attacker.Add<Damage>(victim);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({attacker, victim});

        duin::World world2;
        duin::Entity damageType = world2.Component<Damage>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity attacker2 = world2.Lookup("Attacker");
        duin::Entity victim2 = world2.Lookup("Victim");
        REQUIRE(attacker2.IsValid());
        REQUIRE(victim2.IsValid());
        CHECK(attacker2.Has(damageType, victim2));
    }

    // [7] Entity relationship → Tag target
    TEST_CASE("Pair: entity relationship with tag target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Ally>();

        duin::Entity weapon = world.Entity("Weapon");
        duin::Entity holder = world.Entity("Holder");
        // (weapon, Ally): rel=runtime entity, tgt=tag
        holder.Add(weapon, world.Component<Ally>());

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({holder, weapon});

        REQUIRE(ps.entities.size() == 2);
        const duin::PackedEntity &holderPacked = ps.entities[0];
        auto pair = std::find_if(holderPacked.pairs.begin(), holderPacked.pairs.end(), [](const duin::PackedPair &p) {
            return p.relationshipName == "Weapon" && p.targetName == "Ally";
        });
        CHECK(pair != holderPacked.pairs.end());
        if (pair != holderPacked.pairs.end())
        {
            CHECK(pair->relationshipIsComponent == false);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 2);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Weapon" && p.targetName == "Ally";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
        if (pair2 != ps2.entities[0].pairs.end())
        {
            CHECK(pair2->relationshipIsComponent == false);
            CHECK(pair2->targetIsComponent == true);
        }
    }

    // [7] Entity+Tag — instantiate and verify
    TEST_CASE("Pair: entity relationship with tag target — instantiate")
    {
        duin::World world;
        world.Component<Ally>();

        duin::Entity weapon = world.Entity("Weapon");
        duin::Entity holder = world.Entity("Holder");
        holder.Add(weapon, world.Component<Ally>());

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({holder, weapon});

        duin::World world2;
        duin::Entity allyType = world2.Component<Ally>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity holder2 = world2.Lookup("Holder");
        duin::Entity weapon2 = world2.Lookup("Weapon");
        REQUIRE(holder2.IsValid());
        REQUIRE(weapon2.IsValid());
        CHECK(holder2.Has(weapon2, allyType));
    }

    // [8] Entity relationship → Component target
    TEST_CASE("Pair: entity relationship with component target — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Health>();

        duin::Entity buff = world.Entity("Buff");
        duin::Entity unit = world.Entity("Unit");
        // (buff, Health): rel=runtime entity, tgt=component
        unit.Add(buff, world.Component<Health>());

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, buff});

        REQUIRE(ps.entities.size() == 2);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Buff" && p.targetName == "Health";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == false);
            CHECK(pair->targetIsComponent == true);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 2);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Buff" && p.targetName == "Health";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
    }

    // [8] Entity+Component — instantiate and verify
    TEST_CASE("Pair: entity relationship with component target — instantiate")
    {
        duin::World world;
        world.Component<Health>();

        duin::Entity buff = world.Entity("Buff");
        duin::Entity unit = world.Entity("Unit");
        unit.Add(buff, world.Component<Health>());

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, buff});

        duin::World world2;
        duin::Entity healthType = world2.Component<Health>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        duin::Entity buff2 = world2.Lookup("Buff");
        REQUIRE(unit2.IsValid());
        REQUIRE(buff2.IsValid());
        CHECK(unit2.Has(buff2, healthType));
    }

    // [9] Entity relationship → Entity target
    TEST_CASE("Pair: entity relationship with entity target — pack and JSON roundtrip")
    {
        duin::World world;

        duin::Entity relation = world.Entity("Weapon");
        duin::Entity target = world.Entity("Enemy");
        duin::Entity unit = world.Entity("Unit");
        // (weapon entity, enemy entity): both runtime entities
        unit.Add(relation, target);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, relation, target});

        REQUIRE(ps.entities.size() == 3);
        auto pair =
            std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Weapon" && p.targetName == "Enemy";
            });
        CHECK(pair != ps.entities[0].pairs.end());
        if (pair != ps.entities[0].pairs.end())
        {
            CHECK(pair->relationshipIsComponent == false);
            CHECK(pair->targetIsComponent == false);
        }

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 3);
        auto pair2 =
            std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(), [](const duin::PackedPair &p) {
                return p.relationshipName == "Weapon" && p.targetName == "Enemy";
            });
        CHECK(pair2 != ps2.entities[0].pairs.end());
        if (pair2 != ps2.entities[0].pairs.end())
        {
            CHECK(pair2->relationshipIsComponent == false);
            CHECK(pair2->targetIsComponent == false);
        }
    }

    // [9] Entity+Entity — instantiate and verify
    TEST_CASE("Pair: entity relationship with entity target — instantiate")
    {
        duin::World world;

        duin::Entity relation = world.Entity("Weapon");
        duin::Entity target = world.Entity("Enemy");
        duin::Entity unit = world.Entity("Unit");
        unit.Add(relation, target);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, relation, target});

        duin::World world2;
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        duin::Entity rel2 = world2.Lookup("Weapon");
        duin::Entity tgt2 = world2.Lookup("Enemy");
        REQUIRE(unit2.IsValid());
        REQUIRE(rel2.IsValid());
        REQUIRE(tgt2.IsValid());
        CHECK(unit2.Has(rel2, tgt2));
    }

    // =========================================================================
    // CHILDOF TESTS
    // =========================================================================

    TEST_CASE("ChildOf: single level pack and instantiate")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.Entity("Child").ChildOf(parent).Set<Vec3>(1.0f, 0.0f, 0.0f);

        CHECK(child.GetParent() == parent);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        REQUIRE(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Parent");
        REQUIRE(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Child");

        duin::World world2;
        world2.Component<Vec3>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> roots = world2.GetChildren();
        REQUIRE(roots.size() == 1);
        std::vector<duin::Entity> children = roots[0].GetChildren();
        REQUIRE(children.size() == 1);
        CHECK(BaseName(children[0].GetName()) == "Child");
        CHECK(children[0].GetParent() == roots[0]);
    }

    TEST_CASE("ChildOf: three-level hierarchy pack and instantiate")
    {
        duin::World world;

        duin::Entity root = world.Entity("Root");
        duin::Entity mid = world.Entity("Mid").ChildOf(root);
        duin::Entity leaf = world.Entity("Leaf").ChildOf(mid);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({root});

        REQUIRE(ps.entities.size() == 1);
        REQUIRE(ps.entities[0].children.size() == 1);
        REQUIRE(ps.entities[0].children[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].children[0].name == "Leaf");

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 1);
        REQUIRE(ps2.entities[0].children.size() == 1);
        REQUIRE(ps2.entities[0].children[0].children.size() == 1);
        CHECK(ps2.entities[0].children[0].children[0].name == "Leaf");

        // Instantiate
        duin::World world2;
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> roots = world2.GetChildren();
        REQUIRE(roots.size() == 1);
        std::vector<duin::Entity> midChildren = roots[0].GetChildren();
        REQUIRE(midChildren.size() == 1);
        CHECK(BaseName(midChildren[0].GetName()) == "Mid");
        std::vector<duin::Entity> leafChildren = midChildren[0].GetChildren();
        REQUIRE(leafChildren.size() == 1);
        CHECK(BaseName(leafChildren[0].GetName()) == "Leaf");
        CHECK(leafChildren[0].GetParent() == midChildren[0]);
    }

    TEST_CASE("ChildOf: multiple siblings under one parent")
    {
        duin::World world;

        duin::Entity parent = world.Entity("Parent");
        world.Entity("ChildA").ChildOf(parent);
        world.Entity("ChildB").ChildOf(parent);
        world.Entity("ChildC").ChildOf(parent);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        REQUIRE(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 3);

        std::vector<std::string> names;
        for (const auto &c : ps.entities[0].children)
            names.push_back(c.name);
        CHECK(std::find(names.begin(), names.end(), "ChildA") != names.end());
        CHECK(std::find(names.begin(), names.end(), "ChildB") != names.end());
        CHECK(std::find(names.begin(), names.end(), "ChildC") != names.end());

        duin::World world2;
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> roots = world2.GetChildren();
        REQUIRE(roots.size() == 1);
        CHECK(roots[0].GetChildren().size() == 3);
    }

    TEST_CASE("ChildOf: child has own components, parent relationship preserved")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();

        duin::Entity parent = world.Entity("Boss").Set<Vec3>(0.0f, 0.0f, 0.0f).Set<Health>({500.0f});
        duin::Entity minion =
            world.Entity("Minion").ChildOf(parent).Set<Vec3>(1.0f, 0.0f, 0.0f).Set<Health>({50.0f});

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        REQUIRE(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Minion");

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> roots = world2.GetChildren();
        REQUIRE(roots.size() == 1);
        std::vector<duin::Entity> children = roots[0].GetChildren();
        REQUIRE(children.size() == 1);
        CHECK(children[0].Has<Vec3>());
        CHECK(children[0].Has<Health>());
        CHECK(children[0].GetMut<Health>() == Health{50.0f});
    }

    // =========================================================================
    // ISIA / INHERITANCE TESTS
    // =========================================================================

    TEST_CASE("IsA: basic inheritance pack and instantiate")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();

        duin::Entity basePrefab = world.Entity("BasePrefab").Set<Vec3>(0.0f, 0.0f, 0.0f).Set<Health>({100.0f});

        duin::Entity instance = world.Entity("Instance").IsA(basePrefab);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({instance});

        REQUIRE(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Instance");

        auto isAPair = std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
                                    [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        REQUIRE(isAPair != ps.entities[0].pairs.end());
        CHECK(isAPair->targetName == "BasePrefab");

        // JSON roundtrip
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 1);
        auto isAPair2 = std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(),
                                     [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        REQUIRE(isAPair2 != ps2.entities[0].pairs.end());
        CHECK(isAPair2->targetName == "BasePrefab");
    }

    TEST_CASE("IsA: inherited component values accessible on instance")
    {
        duin::World world;
        world.Component<Health>();
        world.Component<Strength>();

        duin::Entity base = world.Entity("BaseWarrior").Set<Health>({200.0f}).Set<Strength>({10});

        duin::Entity instance = world.Entity("Warrior").IsA(base);

        // Via IsA, inherited values should be accessible
        CHECK(instance.Has<Health>());
        CHECK(instance.Has<Strength>());
    }

    TEST_CASE("IsA: overridden component value on instance")
    {
        duin::World world;
        world.Component<Health>();

        duin::Entity base = world.Entity("BaseUnit").Set<Health>({100.0f});
        duin::Entity strongInstance = world.Entity("StrongUnit").IsA(base).Set<Health>({250.0f}); // override

        CHECK(strongInstance.GetMut<Health>() == Health{250.0f});
    }

    TEST_CASE("IsA: multiple inheritance levels — pack and JSON roundtrip")
    {
        duin::World world;
        world.Component<Health>();
        world.Component<Damage>();

        duin::Entity baseA = world.Entity("BaseA").Set<Health>({100.0f});
        duin::Entity baseB = world.Entity("BaseB").IsA(baseA).Set<Damage>({5.0f});
        duin::Entity leaf = world.Entity("LeafUnit").IsA(baseB);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({leaf});

        REQUIRE(ps.entities.size() == 1);
        auto isAPair = std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
                                    [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        REQUIRE(isAPair != ps.entities[0].pairs.end());
        CHECK(isAPair->targetName == "BaseB");

        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        REQUIRE(ps2.entities.size() == 1);
        auto isAPair2 = std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(),
                                     [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        REQUIRE(isAPair2 != ps2.entities[0].pairs.end());
        CHECK(isAPair2->targetName == "BaseB");
    }

    TEST_CASE("IsA: prefab and non-prefab in same packed scene")
    {
        duin::World world;
        world.Component<Health>();
        world.Component<Vec3>();

        duin::Entity prefab = world.Entity("EnemyPrefab").Set<Health>({80.0f}).Set<Vec3>(0.0f, 0.0f, 0.0f);

        duin::Entity inst1 = world.Entity("Enemy1").IsA(prefab).Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity inst2 = world.Entity("Enemy2").IsA(prefab).Set<Vec3>(2.0f, 0.0f, 0.0f);
        duin::Entity standalone = world.Entity("Soldier").Set<Health>({120.0f});

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({inst1, inst2, standalone});

        CHECK(ps.entities.size() == 3);

        auto findIsA = [](const duin::PackedEntity &pe) {
            return std::find_if(pe.pairs.begin(), pe.pairs.end(),
                                [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        };

        auto it1 = findIsA(ps.entities[0]);
        auto it2 = findIsA(ps.entities[1]);
        // inst1 and inst2 should have IsA pairs
        CHECK(it1 != ps.entities[0].pairs.end());
        CHECK(it2 != ps.entities[1].pairs.end());

        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        CHECK(ps2.entities.size() == 3);
    }

    // =========================================================================
    // MIXED / EDGE-CASE TESTS
    // =========================================================================

    TEST_CASE("Entity with both ChildOf and custom pair relationship")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Targets>();

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.Entity("Child").ChildOf(parent).Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity target = world.Entity("TargetMan").Set<Vec3>(5.0f, 0.0f, 0.0f);
        child.Add<Targets>(target);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent, target});

        REQUIRE(ps.entities.size() == 2);
        REQUIRE(ps.entities[0].children.size() == 1);

        const duin::PackedEntity &childPacked = ps.entities[0].children[0];
        auto targetsPair = std::find_if(childPacked.pairs.begin(), childPacked.pairs.end(),
                                        [](const duin::PackedPair &p) { return p.relationshipName == "Targets"; });
        CHECK(targetsPair != childPacked.pairs.end());
        if (targetsPair != childPacked.pairs.end())
            CHECK(targetsPair->targetName == "TargetMan");

        duin::World world2;

        world2.Component<Vec3>();
        world2.Component<Targets>();
        duin::SceneBuilder sb2(&world2);

        sb2.InstantiateScene(ps, &world2);

        flecs::entity cmp =
            world2.GetFlecsWorld().lookup("::TestSceneBuilder::DOCTEST_ANON_SUITE_2::Targets", "::", "::", true);
        duin::Entity dnCMP = world2.Lookup("::TestSceneBuilder::DOCTEST_ANON_SUITE_2::Targets");

        duin::Entity child2 = world2.Lookup("Parent::Child");
        duin::Entity target2 = world2.Lookup("TargetMan");
        REQUIRE(child2.IsValid());
        REQUIRE(target2.IsValid());
        CHECK(child2.Has<Targets>(582));
        CHECK(child2.Has<Targets>(target2));
        // ChildOf must still be intact
        duin::Entity parent2 = world2.Lookup("Parent");
        REQUIRE(parent2.IsValid());
        CHECK(child2.GetParent() == parent2);
    }

    TEST_CASE("Entity with IsA and ChildOf simultaneously")
    {
        duin::World world;
        world.Component<Health>();

        duin::Entity prefab = world.Entity("SoldierPrefab").Set<Health>({100.0f});
        duin::Entity group = world.Entity("Squad");
        duin::Entity inst = world.Entity("Soldier1").IsA(prefab).ChildOf(group);

        CHECK(inst.Has<Health>());
        CHECK(inst.GetParent() == group);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({group});

        REQUIRE(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Squad");
        REQUIRE(ps.entities[0].children.size() == 1);

        const duin::PackedEntity &instPacked = ps.entities[0].children[0];
        CHECK(instPacked.name == "Soldier1");
        auto isAPair = std::find_if(instPacked.pairs.begin(), instPacked.pairs.end(),
                                    [](const duin::PackedPair &p) { return p.relationshipName == "IsA"; });
        CHECK(isAPair != instPacked.pairs.end());
    }

    TEST_CASE("Multiple custom pairs on one entity — all combinations present")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Owns>();
        world.Component<Follows>();

        duin::Entity unit = world.Entity("Unit");
        duin::Entity a = world.Entity("A");
        duin::Entity b = world.Entity("B");
        duin::Entity c = world.Entity("C");

        unit.Add<Targets>(a);
        unit.Add<Owns>(b);
        unit.Add<Follows>(c);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, a, b, c});

        REQUIRE(ps.entities.size() == 4);

        auto findPair = [&](const std::string &rel) {
            return std::find_if(ps.entities[0].pairs.begin(), ps.entities[0].pairs.end(),
                                [&rel](const duin::PackedPair &p) { return p.relationshipName == rel; });
        };

        CHECK(findPair("Targets") != ps.entities[0].pairs.end());
        CHECK(findPair("Owns") != ps.entities[0].pairs.end());
        CHECK(findPair("Follows") != ps.entities[0].pairs.end());

        // JSON roundtrip preserves all three
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);

        REQUIRE(ps2.entities.size() == 4);
        auto findPair2 = [&](const std::string &rel) {
            return std::find_if(ps2.entities[0].pairs.begin(), ps2.entities[0].pairs.end(),
                                [&rel](const duin::PackedPair &p) { return p.relationshipName == rel; });
        };
        CHECK(findPair2("Targets") != ps2.entities[0].pairs.end());
        CHECK(findPair2("Owns") != ps2.entities[0].pairs.end());
        CHECK(findPair2("Follows") != ps2.entities[0].pairs.end());
    }

    TEST_CASE("Multiple custom pairs — instantiate all relationships")
    {
        duin::World world;
        world.Component<Targets>();
        world.Component<Owns>();
        world.Component<Follows>();

        duin::Entity unit = world.Entity("Unit");
        duin::Entity a = world.Entity("A");
        duin::Entity b = world.Entity("B");
        duin::Entity c = world.Entity("C");

        unit.Add<Targets>(a);
        unit.Add<Owns>(b);
        unit.Add<Follows>(c);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit, a, b, c});

        duin::World world2;
        world2.Component<Targets>();
        world2.Component<Owns>();
        world2.Component<Follows>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        duin::Entity unit2 = world2.Lookup("Unit");
        duin::Entity a2 = world2.Lookup("A");
        duin::Entity b2 = world2.Lookup("B");
        duin::Entity c2 = world2.Lookup("C");

        REQUIRE(unit2.IsValid());
        REQUIRE(a2.IsValid());
        REQUIRE(b2.IsValid());
        REQUIRE(c2.IsValid());

        CHECK(unit2.Has<Targets>(a2));
        CHECK(unit2.Has<Owns>(b2));
        CHECK(unit2.Has<Follows>(c2));
    }

    TEST_CASE("PackedPair fields survive full JSON roundtrip — all metadata fields")
    {
        duin::PackedPair original;
        original.relationshipName = "MyRelation";
        original.relationshipUUID = duin::UUID(0xAABBCCDD11223344ULL);
        original.relationshipIsComponent = true;
        original.relationshipPath = "/MyRelation";
        original.targetName = "MyTarget";
        original.targetUUID = duin::UUID(0x1122334455667788ULL);
        original.targetIsComponent = false;
        original.targetPath = "/MyTarget";
        original.jsonData = "{\"key\":\"value\"}";

        duin::SceneBuilder builder(nullptr);
        duin::JSONValue json = builder.SerializePair(original);
        duin::PackedPair restored = builder.DeserializePair(json);

        CHECK(restored.relationshipName == original.relationshipName);
        CHECK(restored.relationshipUUID == original.relationshipUUID);
        CHECK(restored.relationshipIsComponent == original.relationshipIsComponent);
        CHECK(restored.relationshipPath == original.relationshipPath);
        CHECK(restored.targetName == original.targetName);
        CHECK(restored.targetUUID == original.targetUUID);
        CHECK(restored.targetIsComponent == original.targetIsComponent);
        CHECK(restored.targetPath == original.targetPath);
        CHECK(restored.jsonData == original.jsonData);
    }

    TEST_CASE("Pair with empty target name — serialization graceful")
    {
        duin::PackedPair pp;
        pp.relationshipName = "Targets";
        pp.targetName = "";
        pp.jsonData = "";

        duin::SceneBuilder builder(nullptr);
        duin::JSONValue json = builder.SerializePair(pp);
        duin::PackedPair restored = builder.DeserializePair(json);

        CHECK(restored.relationshipName == "Targets");
        CHECK(restored.targetName.empty());
    }

    TEST_CASE("Relationship on entity outside packed set — target name not resolved")
    {
        // When the target entity is NOT included in the packed set,
        // the pair is still packed but the target may be referenced by UUID only.
        duin::World world;
        world.Component<Targets>();

        duin::Entity unit = world.Entity("Unit");
        duin::Entity outsider = world.Entity("Outsider");
        unit.Add<Targets>(outsider);

        // Pack only unit — outsider is NOT in the packed set
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({unit});

        // The pair should still be packed (with whatever information is available)
        REQUIRE(ps.entities.size() == 1);
        // We just verify no crash and the scene is valid
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene ps2 = sb.DeserializeScene(json);
        CHECK(ps2.entities.size() == 1);
    }

    TEST_CASE("InstantiateSceneAsChildren attaches entities under given parent")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity a = world.Entity("A").Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity b = world.Entity("B").Set<Vec3>(2.0f, 0.0f, 0.0f);

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({a, b});

        duin::World world2;
        world2.Component<Vec3>();

        duin::Entity sceneRoot = world2.Entity("SceneRoot");
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateSceneAsChildren(ps, sceneRoot);

        std::vector<duin::Entity> children = sceneRoot.GetChildren();
        CHECK(children.size() == 2);

        std::vector<std::string> names;
        for (auto &c : children)
            names.push_back(BaseName(c.GetName()));
        CHECK(std::find(names.begin(), names.end(), "A") != names.end());
        CHECK(std::find(names.begin(), names.end(), "B") != names.end());
    }
}

} // namespace TestSceneBuilder
