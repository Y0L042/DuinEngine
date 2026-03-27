#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/IO/FileUtils.h>
#include <Duin/IO/Filesystem.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

// Embedded scene data — copied from DuinFPS/data/world1.pscn and playerScene.pscn
// (stripped to bare entity hierarchy, no components/pairs, to keep tests focused).

const std::string ARTIFACT_PATH = "./artifacts";

static const char *WORLD1_PSCN = R"({
    "sceneUUID": "0xB9322953E1B3C459",
    "sceneName": "World1",
    "metadata": {
        "editorVersion": "",
        "engineVersion": "",
        "lastModified": "",
        "author": ""
    },
    "entities": [
        {
            "uuid": "0xB9322953E1B3C382",
            "name": "Player",
            "enabled": true,
            "tags": [],
            "pairs": [],
            "components": [],
            "children": [],
            "instanceOf": {
                "rPath": "___REPLACE_WITH_TEMP_PATH___",
                "uuid": "0xB9322953E1B3C456",
                "fileType": "FS_FILETYPE_TEXT_EXT",
                "fileExt": "FS_FILEEXT_PSCN"
            }
        }
    ]
})";

static const char *PLAYER_SCENE_PSCN = R"({
    "sceneUUID": "0xB9322953E1B3C456",
    "sceneName": "PlayerScene",
    "metadata": {
        "editorVersion": "",
        "engineVersion": "",
        "lastModified": "",
        "author": ""
    },
    "entities": [
        {
            "uuid": "0x290F25D657B96ADF",
            "name": "PlayerRoot",
            "enabled": true,
            "tags": [],
            "pairs": [],
            "components": [],
            "children": [
                {
                    "uuid": "0x3CB1B60968CD0A60",
                    "name": "CameraRoot",
                    "enabled": true,
                    "tags": [],
                    "pairs": [],
                    "components": [],
                    "children": [
                        {
                            "uuid": "0x03B8918EC9D0FFAB",
                            "name": "PlayerCamera",
                            "enabled": true,
                            "tags": [],
                            "pairs": [],
                            "components": [],
                            "children": []
                        }
                    ]
                }
            ]
        }
    ]
})";

// Helper: write the player scene to a temp file and return the path.
// Patches WORLD1_PSCN's rPath placeholder with the actual temp file path.
static std::string WritePlayerSceneToDisk()
{
    std::string path = ARTIFACT_PATH + "/playerScene_test.pscn";
    duin::FileUtils::WriteStringIntoFile(path, PLAYER_SCENE_PSCN);
    return path;
}

static std::string PatchWorld1WithPath(const std::string &playerScenePath)
{
    std::string patched = WORLD1_PSCN;
    std::string placeholder = "___REPLACE_WITH_TEMP_PATH___";
    auto pos = patched.find(placeholder);
    if (pos != std::string::npos)
    {
        patched.replace(pos, placeholder.size(), playerScenePath);
    }
    return patched;
}

TEST_SUITE("External Scene Instantiation")
{
    // ---------------------------------------------------------------
    // Instantiate: instanceOf loads external scene entities as children
    // ---------------------------------------------------------------

    TEST_CASE("Instantiate entity with instanceOf loads external scene children from disk")
    {
        // Write the external player scene to a temp file
        std::string playerPath = WritePlayerSceneToDisk();
        std::string world1Json = PatchWorld1WithPath(playerPath);

        duin::SceneBuilder sb;
        duin::PackedScene world1 = sb.DeserializeScene(duin::JSONValue::Parse(world1Json));

        duin::World world;
        sb.InstantiateScene(world1, &world);

        // The "Player" entity should exist at world root
        duin::Entity player = world.Lookup("Player");
        REQUIRE(player.IsValid());

        // The external scene root ("PlayerRoot") is merged into "Player",
        // so Player inherits PlayerRoot's children directly.
        // Player -> CameraRoot -> PlayerCamera
        REQUIRE(player.GetChildren().size() == 1);
        duin::Entity cameraRoot = player.Lookup("CameraRoot");
        CHECK(cameraRoot.IsValid());

        REQUIRE(cameraRoot.GetChildren().size() == 1);
        duin::Entity playerCamera = cameraRoot.Lookup("PlayerCamera");
        CHECK(playerCamera.IsValid());

        // Clean up temp file
        duin::fs::RemovePath(playerPath);
    }

    TEST_CASE("Instantiate entity with instanceOf pointing to nonexistent file does not crash")
    {
        duin::SceneBuilder sb;

        duin::PackedScene scene;
        scene.name = "TestScene";
        duin::PackedEntity pe;
        pe.name = "BrokenRef";
        pe.enabled = true;
        pe.instanceOf = duin::PackedExternalDependency("nonexistent/path/that/does/not/exist.pscn");
        scene.entities.push_back(pe);

        duin::World world;
        // Should not crash — just warns and skips the external load
        sb.InstantiateScene(scene, &world);

        duin::Entity brokenRef = world.Lookup("BrokenRef");
        REQUIRE(brokenRef.IsValid());
        // No children since the file couldn't be loaded
        CHECK(brokenRef.GetChildren().empty());
    }

    TEST_CASE("Instantiate entity with empty rPath instanceOf does not crash")
    {
        duin::SceneBuilder sb;

        duin::PackedScene scene;
        scene.name = "TestScene";
        duin::PackedEntity pe;
        pe.name = "EmptyRef";
        pe.enabled = true;
        pe.instanceOf = duin::PackedExternalDependency("");
        scene.entities.push_back(pe);

        duin::World world;
        sb.InstantiateScene(scene, &world);

        duin::Entity emptyRef = world.Lookup("EmptyRef");
        REQUIRE(emptyRef.IsValid());
        CHECK(emptyRef.GetChildren().empty());
    }

    TEST_CASE("Instantiate entity without instanceOf has no surprise children")
    {
        duin::SceneBuilder sb;

        duin::PackedScene scene;
        scene.name = "PlainScene";
        duin::PackedEntity pe;
        pe.name = "PlainEntity";
        pe.enabled = true;
        // No instanceOf set
        scene.entities.push_back(pe);

        duin::World world;
        sb.InstantiateScene(scene, &world);

        duin::Entity plain = world.Lookup("PlainEntity");
        REQUIRE(plain.IsValid());
        CHECK(plain.GetChildren().empty());
    }

    // ---------------------------------------------------------------
    // Pack: instanceOf set on a PackedEntity survives full pipeline
    // ---------------------------------------------------------------

    TEST_CASE("Pack then Serialize then Deserialize then Instantiate preserves instanceOf children")
    {
        // Write the external player scene to disk
        std::string playerPath = WritePlayerSceneToDisk();
        std::string world1Json = PatchWorld1WithPath(playerPath);

        duin::SceneBuilder sb;

        // Deserialize → Serialize → Deserialize (round-trip the packed data)
        duin::PackedScene original = sb.DeserializeScene(duin::JSONValue::Parse(world1Json));
        duin::JSONValue serialized = sb.SerializeScene(original);
        duin::PackedScene roundTripped = sb.DeserializeScene(serialized);

        // Verify instanceOf survived the round-trip
        REQUIRE(roundTripped.entities.size() == 1);
        REQUIRE(roundTripped.entities[0].instanceOf.has_value());
        CHECK(roundTripped.entities[0].instanceOf->rPath == playerPath);

        // Instantiate the round-tripped scene
        duin::World world;
        sb.InstantiateScene(roundTripped, &world);

        duin::Entity player = world.Lookup("Player");
        REQUIRE(player.IsValid());

        // External scene root merges into Player, so Player gets PlayerRoot's children
        REQUIRE(player.GetChildren().size() == 1);
        CHECK(player.Lookup("CameraRoot").IsValid());

        duin::fs::RemovePath(playerPath);
    }

    TEST_CASE("Pack scene from live world — entities without instanceOf pack correctly")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.Entity("Root").Set<Vec3>(1.0f, 2.0f, 3.0f);
        duin::Entity child = world.Entity("Child").Set<Vec3>(4.0f, 5.0f, 6.0f);
        child.ChildOf(root);

        duin::SceneBuilder sb;
        duin::PackedScene ps = sb.PackScene({root});

        REQUIRE(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Root");
        CHECK_FALSE(ps.entities[0].instanceOf.has_value());

        REQUIRE(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Child");
        CHECK_FALSE(ps.entities[0].children[0].instanceOf.has_value());
    }

    // ---------------------------------------------------------------
    // Circular reference guard
    // ---------------------------------------------------------------

    //TEST_CASE("Circular instanceOf reference does not infinite-loop")
    //{
    //    // Create a scene file that references itself
    //    std::string selfRefPath = ARTIFACT_PATH + "/selfref_test.pscn";

    //    std::string selfRefScene = R"({
    //        "sceneUUID": "0xDEADBEEFDEADBEEF",
    //        "sceneName": "SelfRef",
    //        "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
    //        "entities": [
    //            {
    //                "uuid": "0xAAAAAAAAAAAAAAAA",
    //                "name": "Recursive",
    //                "enabled": true,
    //                "tags": [], "pairs": [], "components": [], "children": [],
    //                "instanceOf": {
    //                    "rPath": "___SELF_PATH___"
    //                }
    //            }
    //        ]
    //    })";

    //    // Patch in the actual path
    //    {
    //        std::string placeholder = "___SELF_PATH___";
    //        auto pos = selfRefScene.find(placeholder);
    //        if (pos != std::string::npos)
    //            selfRefScene.replace(pos, placeholder.size(), selfRefPath);
    //    }

    //    duin::FileUtils::WriteStringIntoFile(selfRefPath, selfRefScene);

    //    duin::SceneBuilder sb;
    //    duin::PackedScene scene = sb.DeserializeScene(duin::JSONValue::Parse(selfRefScene));

    //    duin::World world;
    //    // This should NOT infinite-loop. The circular reference guard skips the second load.
    //    sb.InstantiateScene(scene, &world);

    //    duin::Entity recursive = world.Lookup("Recursive");
    //    REQUIRE(recursive.IsValid());
    //    // The self-reference should have been detected and skipped,
    //    // so there should be no deeply nested children.

    //    duin::fs::RemovePath(selfRefPath);
    //}

    // ---------------------------------------------------------------
    // Width: many sibling entities each with instanceOf
    // ---------------------------------------------------------------

    TEST_CASE("Multiple sibling entities with instanceOf all resolve correctly (width)")
    {
        // Write two different external scenes to disk
        std::string sceneAPath = ARTIFACT_PATH + "/sceneA_test.pscn";
        std::string sceneBPath = ARTIFACT_PATH + "/sceneB_test.pscn";

        std::string sceneA = R"({
            "sceneUUID": "0x1111111111111111",
            "sceneName": "SceneA",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0x1111111111111AAA",
                    "name": "ChildA",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [], "children": []
                }
            ]
        })";

        std::string sceneB = R"({
            "sceneUUID": "0x2222222222222222",
            "sceneName": "SceneB",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0x2222222222222BBB",
                    "name": "ChildB",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [], "children": []
                }
            ]
        })";

        duin::FileUtils::WriteStringIntoFile(sceneAPath, sceneA);
        duin::FileUtils::WriteStringIntoFile(sceneBPath, sceneB);

        // Build a parent scene with 4 sibling entities, each referencing an external scene
        duin::PackedScene parentScene;
        parentScene.name = "WidthTest";

        for (int i = 0; i < 4; i++)
        {
            duin::PackedEntity pe;
            pe.uuid = duin::UUID();
            pe.name = "Sibling" + std::to_string(i);
            pe.enabled = true;
            pe.instanceOf = duin::PackedExternalDependency((i % 2 == 0) ? sceneAPath : sceneBPath);
            parentScene.entities.push_back(pe);
        }

        duin::SceneBuilder sb;
        duin::World world;
        sb.InstantiateScene(parentScene, &world);

        // All 4 siblings should exist. The external scene root merges into each sibling,
        // so since ChildA/ChildB have no children, each sibling has 0 children.
        for (int i = 0; i < 4; i++)
        {
            std::string name = "Sibling" + std::to_string(i);
            duin::Entity sibling = world.Lookup(name.c_str());
            REQUIRE_MESSAGE(sibling.IsValid(), "Sibling entity '", name, "' should exist");
            CHECK_MESSAGE(sibling.GetChildren().empty(), "Sibling '", name, "' should have no children");
        }

        duin::fs::RemovePath(sceneAPath);
        duin::fs::RemovePath(sceneBPath);
    }

    // ---------------------------------------------------------------
    // Depth: chained instanceOf across multiple levels
    // ---------------------------------------------------------------

    TEST_CASE("Chained instanceOf across 3 levels resolves full depth")
    {
        // Level 2 (leaf): no instanceOf, just a plain entity
        std::string leafPath = ARTIFACT_PATH + "/depth_leaf_test.pscn";
        std::string leafScene = R"({
            "sceneUUID": "0xAEAF000000000001",
            "sceneName": "LeafScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xAEAF00000000AAAA",
                    "name": "LeafEntity",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [], "children": []
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(leafPath, leafScene);

        // Level 1 (middle): references the leaf scene
        std::string middlePath = ARTIFACT_PATH + "/depth_middle_test.pscn";
        std::string middleScene = R"({
            "sceneUUID": "0xA1DD000000000001",
            "sceneName": "MiddleScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xA1DD00000000AAAA",
                    "name": "MiddleEntity",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [],
                    "children": [],
                    "instanceOf": {
                        "rPath": ")" +
                                  leafPath + R"("
                    }
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(middlePath, middleScene);

        // Level 0 (root): references the middle scene
        duin::PackedScene rootScene;
        rootScene.name = "DepthTest";
        {
            duin::PackedEntity pe;
            pe.uuid = duin::UUID();
            pe.name = "RootEntity";
            pe.enabled = true;
            pe.instanceOf = duin::PackedExternalDependency(middlePath);
            rootScene.entities.push_back(pe);
        }

        duin::SceneBuilder sb;
        duin::World world;
        sb.InstantiateScene(rootScene, &world);

        // The external scene root merges into the referencing entity at each level:
        // RootEntity merges with MiddleEntity (which merges with LeafEntity).
        // Since LeafEntity has no children, the chain collapses — RootEntity has no children.
        duin::Entity root = world.Lookup("RootEntity");
        world.IterateChildren([](duin::Entity e) {
            MESSAGE("Entity ", e.GetID(), " Name ", e.GetName());
            e.EachChild(
                [](duin::Entity child) { MESSAGE("Entity (child) ", child.GetID(), " Name ", child.GetName()); });
        });
        REQUIRE(root.IsValid());
        CHECK(root.GetChildren().empty());

        duin::fs::RemovePath(leafPath);
        duin::fs::RemovePath(middlePath);
    }

    // ---------------------------------------------------------------
    // Component application: external root components applied to referencing entity
    // ---------------------------------------------------------------

    TEST_CASE("Components on external scene root are applied to the referencing entity")
    {
        // Write an external scene whose root has a Vec3 component
        std::string extPath = ARTIFACT_PATH + "/comp_root_test.pscn";
        std::string extScene = R"({
            "sceneUUID": "0xC0A1B2C3D4E5F601",
            "sceneName": "CompRootScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xC0A1B2C3D4E5AAAA",
                    "name": "CompRootEntity",
                    "enabled": true,
                    "tags": [], "pairs": [],
                    "components": [
                        { "type": "Vec3", "x": 7.0, "y": 8.0, "z": 9.0 }
                    ],
                    "children": []
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(extPath, extScene);

        // Parent scene: one entity with instanceOf pointing to the external scene
        duin::PackedScene parentScene;
        parentScene.name = "CompRootTest";
        {
            duin::PackedEntity pe;
            pe.uuid = duin::UUID();
            pe.name = "Holder";
            pe.enabled = true;
            pe.instanceOf = duin::PackedExternalDependency(extPath);
            parentScene.entities.push_back(pe);
        }

        duin::World world;
        world.Component<Vec3>();

        duin::SceneBuilder sb;
        sb.InstantiateScene(parentScene, &world);

        duin::Entity holder = world.Lookup("Holder");
        REQUIRE(holder.IsValid());

        // The referencing entity must carry the Vec3 component from the external root
        CHECK(holder.Has<Vec3>());

        // Verify the component data was applied correctly
        const Vec3 *v = holder.TryGetMut<Vec3>();
        REQUIRE(v != nullptr);
        CHECK(v->x == 7.0f);
        CHECK(v->y == 8.0f);
        CHECK(v->z == 9.0f);

        duin::fs::RemovePath(extPath);
    }

    TEST_CASE("Components on external scene child entities are applied to the instantiated children")
    {
        // Write an external scene with a root that has a child carrying a Vec3 component
        std::string extPath = ARTIFACT_PATH + "/comp_child_test.pscn";
        std::string extScene = R"({
            "sceneUUID": "0xC0B1C2D3E4F5A001",
            "sceneName": "CompChildScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xC0B1C2D3E4F5AAAA",
                    "name": "ExtRoot",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [],
                    "children": [
                        {
                            "uuid": "0xC0B1C2D3E4F5BBBB",
                            "name": "ExtChild",
                            "enabled": true,
                            "tags": [], "pairs": [],
                            "components": [
                                { "type": "Vec3", "x": 1.0, "y": 2.0, "z": 3.0 }
                            ],
                            "children": []
                        }
                    ]
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(extPath, extScene);

        // Parent scene: one entity referencing the external scene
        duin::PackedScene parentScene;
        parentScene.name = "CompChildTest";
        {
            duin::PackedEntity pe;
            pe.uuid = duin::UUID();
            pe.name = "Parent";
            pe.enabled = true;
            pe.instanceOf = duin::PackedExternalDependency(extPath);
            parentScene.entities.push_back(pe);
        }

        duin::World world;
        world.Component<Vec3>();

        duin::SceneBuilder sb;
        sb.InstantiateScene(parentScene, &world);

        duin::Entity parent = world.Lookup("Parent");
        REQUIRE(parent.IsValid());

        // ExtRoot merges into Parent, so ExtChild is a direct child of Parent
        REQUIRE(parent.GetChildren().size() == 1);
        duin::Entity extChild = parent.Lookup("ExtChild");
        CHECK(extChild.IsValid());
        CHECK(extChild.Has<Vec3>());

        duin::fs::RemovePath(extPath);
    }

    // ---------------------------------------------------------------
    // Width + Depth combined: siblings with instanceOf at multiple levels
    // ---------------------------------------------------------------

    TEST_CASE("Mixed width and depth: multiple siblings each loading nested external scenes")
    {
        // Leaf scene with single root "Equipment" containing Weapon and Shield
        std::string leafPath = ARTIFACT_PATH + "/wd_leaf_test.pscn";
        std::string leafScene = R"({
            "sceneUUID": "0xCD1F000000000001",
            "sceneName": "WDLeafScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xCD1F0000000AAAAA",
                    "name": "Equipment",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [],
                    "children": [
                        {
                            "uuid": "0xCD1F0000000BBBBB",
                            "name": "Weapon",
                            "enabled": true,
                            "tags": [], "pairs": [], "components": [], "children": []
                        },
                        {
                            "uuid": "0xCD1F0000000CCCCC",
                            "name": "Shield",
                            "enabled": true,
                            "tags": [], "pairs": [], "components": [], "children": []
                        }
                    ]
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(leafPath, leafScene);

        // Middle scene: entity that references the leaf
        std::string middlePath = ARTIFACT_PATH + "/wd_middle_test.pscn";
        std::string middleScene = R"({
            "sceneUUID": "0xCD2D000000000001",
            "sceneName": "WDMiddleScene",
            "metadata": { "editorVersion": "", "engineVersion": "", "lastModified": "", "author": "" },
            "entities": [
                {
                    "uuid": "0xCD2D0000000AAAAA",
                    "name": "Soldier",
                    "enabled": true,
                    "tags": [], "pairs": [], "components": [],
                    "children": [],
                    "instanceOf": {
                        "rPath": ")" +
                                  leafPath + R"("
                    }
                }
            ]
        })";
        duin::FileUtils::WriteStringIntoFile(middlePath, middleScene);

        // Root scene: single root "Squad" with 3 Soldier children referencing the middle scene
        duin::PackedScene rootScene;
        rootScene.name = "WidthDepthTest";
        {
            duin::PackedEntity squad;
            squad.uuid = duin::UUID();
            squad.name = "Squad";
            squad.enabled = true;
            for (int i = 0; i < 3; i++)
            {
                duin::PackedEntity soldier;
                soldier.uuid = duin::UUID();
                soldier.name = "Soldier" + std::to_string(i);
                soldier.enabled = true;
                soldier.instanceOf = duin::PackedExternalDependency(middlePath);
                squad.children.push_back(soldier);
            }
            rootScene.entities.push_back(squad);
        }

        REQUIRE_MESSAGE(rootScene.entities.size() == 1, "rootScene should have 1 root entity");

        duin::SceneBuilder sb;
        duin::World world;
        sb.InstantiateScene(rootScene, &world);

        // Squad is the single root. Each SoldierN merges with middle root "Soldier",
        // which merges with leaf root "Equipment". Equipment's children (Weapon, Shield)
        // become children of each SoldierN.
        // Hierarchy: Squad -> {Soldier0, Soldier1, Soldier2} each -> {Weapon, Shield}
        duin::Entity squad = world.Lookup("Squad");
        REQUIRE(squad.IsValid());
        REQUIRE(squad.GetChildren().size() == 3);

        for (int i = 0; i < 3; i++)
        {
            std::string soldierName = "Soldier" + std::to_string(i);
            duin::Entity soldier = squad.Lookup(soldierName.c_str());
            REQUIRE_MESSAGE(soldier.IsValid(), soldierName, " should exist");
            REQUIRE_MESSAGE(soldier.GetChildren().size() == 2, soldierName, " should have 2 children");
            CHECK_MESSAGE(soldier.Lookup("Weapon").IsValid(), soldierName, " should have Weapon child");
            CHECK_MESSAGE(soldier.Lookup("Shield").IsValid(), soldierName, " should have Shield child");
        }

        duin::fs::RemovePath(leafPath);
        duin::fs::RemovePath(middlePath);
    }
}

} // namespace TestSceneBuilder
