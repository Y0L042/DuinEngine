#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("Full Pipeline: String to Instantiation")
{
    TEST_CASE("JSON string -> PackedScene -> Instantiate entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        std::string jsonStr = R"({
            "sceneUUID": "aabb112233445566",
            "sceneName": "PipelineScene",
            "metadata": {
                "editorVersion": "1.0",
                "engineVersion": "0.1.0",
                "lastModified": "2025-01-20T10:00:00Z",
                "author": "PipelineTest"
            },
            "externalDependencies": [],
            "entities": [
                {
                    "uuid": "ent111111111111",
                    "name": "MainEntity",
                    "enabled": true,
                    "tags": ["main"],
                    "components": [
                        {
                            "type": "Vec3",
                            "x": 5.0,
                            "y": 10.0,
                            "z": 15.0
                        }
                    ],
                    "children": [
                        {
                            "uuid": "ent222222222222",
                            "name": "ChildCamera",
                            "enabled": true,
                            "tags": ["camera"],
                            "components": [
                                {
                                    "type": "Camera",
                                    "fov": 75.0,
                                    "nearPlane": 0.1,
                                    "farPlane": 1000.0,
                                    "isPrimary": true
                                }
                            ],
                            "children": []
                        }
                    ]
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("aabb112233445566"));
        CHECK(scene.name == "PipelineScene");
        CHECK(scene.entities.size() == 1);
        if (scene.entities.size() >= 1)
        {
            CHECK(scene.entities[0].name == "MainEntity");
            CHECK(scene.entities[0].children.size() == 1);
            if (scene.entities[0].children.size() >= 1)
            {
                CHECK(scene.entities[0].children[0].name == "ChildCamera");
            }
        }

        duin::Entity root = world.CreateEntity("SceneRoot");
        duin::SceneBuilder builder(&world);
        builder.InstantiateScene(scene, &world);

        std::vector<duin::Entity> rootChildren = root.GetChildren();
        CHECK(rootChildren.size() == 1);
        if (rootChildren.size() >= 1)
        {
            CHECK(rootChildren[0].GetName() == "MainEntity");
            CHECK(rootChildren[0].Has<Vec3>());
            CHECK(rootChildren[0].GetMut<Vec3>() == Vec3{5.0f, 10.0f, 15.0f});

            std::vector<duin::Entity> grandChildren = rootChildren[0].GetChildren();
            CHECK(grandChildren.size() == 1);
            if (grandChildren.size() >= 1)
            {
                CHECK(grandChildren[0].GetName() == "ChildCamera");
                CHECK(grandChildren[0].Has<Camera>());
                CHECK(grandChildren[0].GetMut<Camera>() == Camera{75.0f, 0.1f, 1000.0f, true});
            }
        }
    }

    TEST_CASE("JSON string -> PackedScene -> Instantiate multiple root entities")
    {
        duin::World world;
        world.Component<Vec3>();

        std::string jsonStr = R"({
            "sceneUUID": "ccdd556677889900",
            "sceneName": "MultiRootScene",
            "entities": [
                {
                    "uuid": "root1111111111",
                    "name": "EntityA",
                    "enabled": true,
                    "tags": [],
                    "components": [
                        {"type": "Vec3", "x": 1.0, "y": 0.0, "z": 0.0}
                    ],
                    "children": []
                },
                {
                    "uuid": "root2222222222",
                    "name": "EntityB",
                    "enabled": true,
                    "tags": [],
                    "components": [
                        {"type": "Vec3", "x": 0.0, "y": 1.0, "z": 0.0}
                    ],
                    "children": []
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        duin::Entity root = world.CreateEntity("SceneRoot");
        duin::SceneBuilder builder(&world);
        builder.InstantiateScene(scene, &world);

        std::vector<duin::Entity> children = root.GetChildren();
        CHECK(children.size() == 2);
    }

    TEST_CASE("JSON string -> Deserialize -> Serialize -> Deserialize consistency")
    {
        std::string jsonStr = R"({
            "sceneUUID": "eeff112233aabb00",
            "sceneName": "ConsistencyScene",
            "metadata": {
                "editorVersion": "2.0",
                "engineVersion": "1.5.0",
                "lastModified": "2025-06-15T08:00:00Z",
                "author": "ConsistencyTest"
            },
            "externalDependencies": [
                {"uuid": "dep1111111111", "type": "scene"},
                {"uuid": "dep2222222222", "type": "asset"}
            ],
            "entities": [
                {
                    "uuid": "ent333333333333",
                    "name": "Root",
                    "enabled": true,
                    "tags": ["persistent"],
                    "components": [
                        {"type": "Vec3", "x": 1.0, "y": 2.0, "z": 3.0}
                    ],
                    "children": [
                        {
                            "uuid": "ent444444444444",
                            "name": "Leaf",
                            "enabled": false,
                            "tags": ["leaf", "static"],
                            "components": [],
                            "children": []
                        }
                    ]
                }
            ]
        })";

        duin::JSONValue json1 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene1 = duin::PackedScene::Deserialize(json1);

        duin::JSONValue json2 = duin::PackedScene::Serialize(scene1);
        duin::PackedScene scene2 = duin::PackedScene::Deserialize(json2);

        CHECK(scene1.uuid == scene2.uuid);
        CHECK(scene1.name == scene2.name);
        CHECK(scene1.metadata.editorVersion == scene2.metadata.editorVersion);
        CHECK(scene1.metadata.engineVersion == scene2.metadata.engineVersion);
        CHECK(scene1.metadata.lastModified == scene2.metadata.lastModified);
        CHECK(scene1.metadata.author == scene2.metadata.author);
        CHECK(scene1.externalDependencies.size() == scene2.externalDependencies.size());
        if (scene1.externalDependencies.size() >= 1)
        {
            CHECK(scene1.externalDependencies[0].uuid == scene2.externalDependencies[0].uuid);
            CHECK(scene1.externalDependencies[0].type == scene2.externalDependencies[0].type);
        }
        CHECK(scene1.entities.size() == scene2.entities.size());
        if (scene1.entities.size() >= 1)
        {
            CHECK(scene1.entities[0].uuid == scene2.entities[0].uuid);
            CHECK(scene1.entities[0].name == scene2.entities[0].name);
            CHECK(scene1.entities[0].enabled == scene2.entities[0].enabled);
            CHECK(scene1.entities[0].tags.size() == scene2.entities[0].tags.size());
            CHECK(scene1.entities[0].children.size() == scene2.entities[0].children.size());
            if (scene1.entities[0].children.size() >= 1)
            {
                CHECK(scene1.entities[0].children[0].uuid == scene2.entities[0].children[0].uuid);
                CHECK(scene1.entities[0].children[0].enabled == scene2.entities[0].children[0].enabled);
                CHECK(scene1.entities[0].children[0].tags.size() == scene2.entities[0].children[0].tags.size());
            }
        }
    }
}

TEST_SUITE("Full Pipeline: Instantiation to String")
{
    TEST_CASE("Create entities -> Pack -> Serialize to JSON string")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity sceneRoot = world.CreateEntity("SceneRoot");

        duin::Entity player =
            world.CreateEntity("Player").Set<Vec3>(10.0f, 0.0f, -5.0f).Set<Camera>(90.0f, 0.1f, 2000.0f, true);
        player.ChildOf(sceneRoot);

        duin::Entity weapon = world.CreateEntity("Weapon").Set<Vec3>(0.5f, 1.0f, 0.3f);
        weapon.ChildOf(player);

        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({sceneRoot});
        ps.uuid = duin::UUID::FromStringHex("scene111111111111");
        ps.name = "GameScene";
        ps.metadata.editorVersion = "1.0";
        ps.metadata.engineVersion = "0.1.0";
        ps.metadata.lastModified = "2025-01-20T12:00:00Z";
        ps.metadata.author = "TestUser";

        CHECK(ps.entities.size() == 1);
        if (ps.entities.size() >= 1)
        {
            CHECK(ps.entities[0].name == "Player");
            CHECK(ps.entities[0].children.size() == 1);
            if (ps.entities[0].children.size() >= 1)
            {
                CHECK(ps.entities[0].children[0].name == "Weapon");
            }
        }

        duin::JSONValue json = duin::PackedScene::Serialize(ps);

        CHECK(json.IsObject());
        CHECK(json.HasMember("sceneUUID"));
        CHECK(json["sceneUUID"].GetString() == duin::UUID::FromStringHex("scene111111111111").ToStrHex());
        CHECK(json.HasMember("sceneName"));
        CHECK(json["sceneName"].GetString() == "GameScene");
        CHECK(json.HasMember("metadata"));
        CHECK(json["metadata"]["editorVersion"].GetString() == "1.0");
        CHECK(json.HasMember("entities"));
        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Capacity() == 1);
        CHECK(json["entities"][0]["name"].GetString() == "Player");
        CHECK(json["entities"][0]["children"].IsArray());
        CHECK(json["entities"][0]["children"].Capacity() == 1);
        CHECK(json["entities"][0]["children"][0]["name"].GetString() == "Weapon");

        std::string jsonStr = json.Write();
        CHECK(!jsonStr.empty());
    }

    TEST_CASE("Create entities -> Pack -> Serialize -> Deserialize -> Instantiate")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity sceneRoot = world.CreateEntity("SceneRoot");
        duin::Entity obj =
            world.CreateEntity("Object").Set<Vec3>(3.0f, 6.0f, 9.0f).Set<Camera>(45.0f, 0.5f, 100.0f, false);
        obj.ChildOf(sceneRoot);

        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({sceneRoot});
        ps.uuid = duin::UUID::FromStringHex("roundtrip111111");
        ps.name = "RoundTripScene";

        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        std::string jsonStr = json.Write();

        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json2);

        CHECK(ps2.uuid == duin::UUID::FromStringHex("roundtrip111111"));
        CHECK(ps2.name == "RoundTripScene");
        CHECK(ps2.entities.size() == 1);
        if (ps2.entities.size() >= 1)
        {
            CHECK(ps2.entities[0].name == "Object");
            CHECK(ps2.entities[0].components.size() == 2);
        }

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(ps2, &world2);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].GetName() == "Object");
            CHECK(children[0].Has<Vec3>());
            CHECK(children[0].Has<Camera>());
            CHECK(children[0].GetMut<Vec3>() == Vec3{3.0f, 6.0f, 9.0f});
            CHECK(children[0].GetMut<Camera>() == Camera{45.0f, 0.5f, 100.0f, false});
        }
    }

    TEST_CASE("Full pipeline with hierarchy: Create -> Pack -> Serialize -> String -> Deserialize -> Instantiate")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Build original hierarchy
        duin::Entity root = world.CreateEntity("Root");
        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(1.0f, 2.0f, 3.0f);
        duin::Entity child = world.CreateEntity("Child").Set<Camera>(60.0f, 0.2f, 500.0f, true);
        duin::Entity grandchild = world.CreateEntity("Grandchild").Set<Vec3>(4.0f, 5.0f, 6.0f);
        parent.ChildOf(root);
        child.ChildOf(parent);
        grandchild.ChildOf(child);

        // Pack
        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({root});
        ps.uuid = duin::UUID::FromStringHex("fullpipe11111111");
        ps.name = "FullPipelineScene";

        // Serialize to JSON string
        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        std::string jsonStr = json.Write();
        CHECK(!jsonStr.empty());

        // Deserialize from string
        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json2);

        CHECK(ps2.name == "FullPipelineScene");
        CHECK(ps2.entities.size() == 1);
        if (ps2.entities.size() >= 1)
        {
            CHECK(ps2.entities[0].name == "Parent");
        }

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(ps2, &world2);

        // Verify hierarchy
        std::vector<duin::Entity> level1 = newRoot.GetChildren();
        CHECK(level1.size() == 1);
        if (level1.size() >= 1)
        {
            CHECK(level1[0].GetName() == "Parent");
            CHECK(level1[0].Has<Vec3>());
            CHECK(level1[0].GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});

            std::vector<duin::Entity> level2 = level1[0].GetChildren();
            CHECK(level2.size() == 1);
            if (level2.size() >= 1)
            {
                CHECK(level2[0].GetName() == "Child");
                CHECK(level2[0].Has<Camera>());
                CHECK(level2[0].GetMut<Camera>() == Camera{60.0f, 0.2f, 500.0f, true});

                std::vector<duin::Entity> level3 = level2[0].GetChildren();
                CHECK(level3.size() == 1);
                if (level3.size() >= 1)
                {
                    CHECK(level3[0].GetName() == "Grandchild");
                    CHECK(level3[0].Has<Vec3>());
                    CHECK(level3[0].GetMut<Vec3>() == Vec3{4.0f, 5.0f, 6.0f});
                }
            }
        }
    }
}

} // namespace TestSceneBuilder
