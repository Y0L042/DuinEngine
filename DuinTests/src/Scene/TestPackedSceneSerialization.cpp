#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("SceneBuilder - PackedScene Serialization")
{
    TEST_CASE("Serialize empty PackedScene")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("abc123def456");
        scene.name = "EmptyScene";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember("sceneUUID"));
        CHECK(json.HasMember("sceneName"));
        CHECK(json["sceneUUID"].GetString() == duin::UUID::FromStringHex("abc123def456").ToStrHex());
        CHECK(json["sceneName"].GetString() == "EmptyScene");
        CHECK(json.HasMember("metadata"));
        CHECK(json.HasMember("externalDependencies"));
        CHECK(json.HasMember("entities"));
    }

    TEST_CASE("Serialize PackedScene with metadata")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("def456abc123");
        scene.name = "TestScene";
        scene.metadata.editorVersion = "1.0";
        scene.metadata.engineVersion = "0.1.0";
        scene.metadata.lastModified = "2025-11-16T12:00:00Z";
        scene.metadata.author = "DuinEditor";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember("metadata"));

        duin::JSONValue metadata = json["metadata"];
        CHECK(metadata.IsObject());
        CHECK(metadata.HasMember("editorVersion"));
        CHECK(metadata["editorVersion"].GetString() == "1.0");
        CHECK(metadata.HasMember("engineVersion"));
        CHECK(metadata["engineVersion"].GetString() == "0.1.0");
        CHECK(metadata.HasMember("lastModified"));
        CHECK(metadata["lastModified"].GetString() == "2025-11-16T12:00:00Z");
        CHECK(metadata.HasMember("author"));
        CHECK(metadata["author"].GetString() == "DuinEditor");
    }

    TEST_CASE("Serialize PackedScene with external dependencies")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("aabbccdd11223344");
        scene.name = "DependentScene";

        duin::PackedExternalDependency dep1;
        dep1.uuid = duin::UUID::FromStringHex("1111111111111111");
        dep1.type = "scene";

        duin::PackedExternalDependency dep2;
        dep2.uuid = duin::UUID::FromStringHex("2222222222222222");
        dep2.type = "asset";

        scene.externalDependencies.push_back(dep1);
        scene.externalDependencies.push_back(dep2);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.HasMember("externalDependencies"));
        CHECK(json["externalDependencies"].IsArray());
        CHECK(json["externalDependencies"].Capacity() == 2);

        duin::JSONValue dep1Json = json["externalDependencies"][0];
        CHECK(dep1Json["uuid"].GetString() == duin::UUID::FromStringHex("1111111111111111").ToStrHex());
        CHECK(dep1Json["type"].GetString() == "scene");

        duin::JSONValue dep2Json = json["externalDependencies"][1];
        CHECK(dep2Json["uuid"].GetString() == duin::UUID::FromStringHex("2222222222222222").ToStrHex());
        CHECK(dep2Json["type"].GetString() == "asset");
    }

    TEST_CASE("Serialize PackedScene with entities")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("fedcba9876543210");
        scene.name = "EntityScene";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("1234567890abcdef");
        entity.name = "Player";
        entity.enabled = true;

        // Add tags as PackedComponents
        duin::PackedComponent tag1;
        tag1.componentTypeName = "PxDynamic";
        tag1.jsonData = R"({"type":"PxDynamic"})";
        entity.tags.push_back(tag1);

        duin::PackedComponent tag2;
        tag2.componentTypeName = "CameraIsActive";
        tag2.jsonData = R"({"type":"CameraIsActive"})";
        entity.tags.push_back(tag2);

        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.HasMember("entities"));
        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Capacity() == 1);

        duin::JSONValue entityJson = json["entities"][0];
        CHECK(entityJson["uuid"].GetString() == duin::UUID::FromStringHex("1234567890abcdef").ToStrHex());
        CHECK(entityJson["name"].GetString() == "Player");
        CHECK(entityJson["enabled"].GetBool() == true);
    }

    TEST_CASE("Serialize complete PackedScene")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("550e8400e29b41d4");
        scene.name = "MainScene";

        // Metadata
        scene.metadata.editorVersion = "1.0";
        scene.metadata.engineVersion = "0.1.0";
        scene.metadata.lastModified = "2025-11-16T12:00:00Z";
        scene.metadata.author = "DuinEditor";

        // External dependencies
        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID::FromStringHex("789e0123e89b12d3");
        dep.type = "scene";
        scene.externalDependencies.push_back(dep);

        // Entities
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("123e4567e89b12d3");
        entity.name = "Player";
        entity.enabled = true;

        // Add tags as PackedComponents
        duin::PackedComponent tag1;
        tag1.componentTypeName = "PxDynamic";
        tag1.jsonData = R"({"type":"PxDynamic"})";
        entity.tags.push_back(tag1);

        duin::PackedComponent tag2;
        tag2.componentTypeName = "CameraIsActive";
        tag2.jsonData = R"({"type":"CameraIsActive"})";
        entity.tags.push_back(tag2);

        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember("sceneUUID"));
        CHECK(json["sceneUUID"].GetString() == duin::UUID::FromStringHex("550e8400e29b41d4").ToStrHex());
        CHECK(json.HasMember("sceneName"));
        CHECK(json["sceneName"].GetString() == "MainScene");
        CHECK(json.HasMember("metadata"));
        CHECK(json["metadata"].IsObject());
        CHECK(json.HasMember("externalDependencies"));
        CHECK(json["externalDependencies"].IsArray());
        CHECK(json["externalDependencies"].Capacity() == 1);
        CHECK(json.HasMember("entities"));
        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Capacity() == 1);
    }

    TEST_CASE("Serialize PackedEntity with components in scene")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("aaa111bbb222");
        scene.name = "ComponentTest";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("ccc333ddd444");
        entity.name = "TestEntity";
        entity.enabled = true;

        duin::PackedComponent comp;
        comp.componentTypeName = "Transform";
        comp.jsonData = R"({"type":"Transform","position":{"x":0.0,"y":0.0,"z":0.0}})";
        entity.components.push_back(comp);

        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Capacity() == 1);
        CHECK(json["entities"][0]["components"].IsArray());
        CHECK(json["entities"][0]["components"].Capacity() == 1);
        CHECK(json["entities"][0]["components"][0]["type"].GetString() == "Transform");
    }
}

TEST_SUITE("SceneBuilder - PackedScene Deserialization")
{
    TEST_CASE("Deserialize minimal JSON scene")
    {
        std::string jsonStr = R"({
            "sceneUUID": "abc123def456",
            "sceneName": "MinimalScene"
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("abc123def456"));
        CHECK(scene.name == "MinimalScene");
        CHECK(scene.entities.empty());
        CHECK(scene.externalDependencies.empty());
    }

    TEST_CASE("Deserialize scene with metadata")
    {
        std::string jsonStr = R"({
            "sceneUUID": "def456abc789",
            "sceneName": "MetadataScene",
            "metadata": {
                "editorVersion": "1.0",
                "engineVersion": "0.1.0",
                "lastModified": "2025-11-16T12:00:00Z",
                "author": "DuinEditor"
            }
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("def456abc789"));
        CHECK(scene.name == "MetadataScene");
        CHECK(scene.metadata.editorVersion == "1.0");
        CHECK(scene.metadata.engineVersion == "0.1.0");
        CHECK(scene.metadata.lastModified == "2025-11-16T12:00:00Z");
        CHECK(scene.metadata.author == "DuinEditor");
    }

    TEST_CASE("Deserialize scene with external dependencies")
    {
        std::string jsonStr = R"({
            "sceneUUID": "aabbccdd11223344",
            "sceneName": "DependencyScene",
            "externalDependencies": [
                {
                    "uuid": "1111111111111111",
                    "type": "scene"
                },
                {
                    "uuid": "2222222222222222",
                    "type": "asset"
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("aabbccdd11223344"));
        CHECK(scene.name == "DependencyScene");
        CHECK(scene.externalDependencies.size() == 2);
        if (scene.externalDependencies.size() >= 2)
        {
            CHECK(scene.externalDependencies[0].uuid == duin::UUID::FromStringHex("1111111111111111"));
            CHECK(scene.externalDependencies[0].type == "scene");
            CHECK(scene.externalDependencies[1].uuid == duin::UUID::FromStringHex("2222222222222222"));
            CHECK(scene.externalDependencies[1].type == "asset");
        }
    }

    TEST_CASE("Deserialize scene with single entity")
    {
        std::string jsonStr = R"({
            "sceneUUID": "fedcba9876543210",
            "sceneName": "SingleEntityScene",
            "entities": [
                {
                    "uuid": "1234567890abcdef",
                    "name": "Player",
                    "enabled": true,
                    "tags": ["PxDynamic", "CameraIsActive"],
                    "components": [],
                    "children": []
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("fedcba9876543210"));
        CHECK(scene.name == "SingleEntityScene");
        CHECK(scene.entities.size() == 1);
        if (scene.entities.size() >= 1)
        {
            CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("1234567890abcdef"));
            CHECK(scene.entities[0].name == "Player");
            CHECK(scene.entities[0].enabled == true);
            CHECK(scene.entities[0].tags.size() == 2);
            if (scene.entities[0].tags.size() >= 2)
            {
                CHECK(scene.entities[0].tags[0].componentTypeName == "PxDynamic");
                CHECK(scene.entities[0].tags[1].componentTypeName == "CameraIsActive");
            }
        }
    }

    TEST_CASE("Deserialize complete scene structure")
    {
        std::string jsonStr = R"({
            "sceneUUID": "550e8400e29b41d4",
            "sceneName": "MainScene",
            "metadata": {
                "editorVersion": "1.0",
                "engineVersion": "0.1.0",
                "lastModified": "2025-11-16T12:00:00Z",
                "author": "DuinEditor"
            },
            "externalDependencies": [
                {
                    "uuid": "789e0123e89b12d3",
                    "type": "scene"
                }
            ],
            "entities": [
                {
                    "uuid": "123e4567e89b12d3",
                    "name": "Player",
                    "enabled": true,
                    "tags": ["PxDynamic", "CameraIsActive"],
                    "components": [],
                    "children": []
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("550e8400e29b41d4"));
        CHECK(scene.name == "MainScene");
        CHECK(scene.metadata.editorVersion == "1.0");
        CHECK(scene.metadata.engineVersion == "0.1.0");
        CHECK(scene.metadata.lastModified == "2025-11-16T12:00:00Z");
        CHECK(scene.metadata.author == "DuinEditor");
        CHECK(scene.externalDependencies.size() == 1);
        if (scene.externalDependencies.size() >= 1)
        {
            CHECK(scene.externalDependencies[0].uuid == duin::UUID::FromStringHex("789e0123e89b12d3"));
            CHECK(scene.externalDependencies[0].type == "scene");
        }
        CHECK(scene.entities.size() == 1);
        if (scene.entities.size() >= 1)
        {
            CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("123e4567e89b12d3"));
            CHECK(scene.entities[0].name == "Player");
        }
    }

    TEST_CASE("Deserialize entity with components")
    {
        std::string jsonStr = R"({
            "sceneUUID": "aaa111bbb222",
            "sceneName": "ComponentScene",
            "entities": [
                {
                    "uuid": "ccc333ddd444",
                    "name": "TestEntity",
                    "enabled": true,
                    "tags": [],
                    "components": [
                        {
                            "type": "Transform",
                            "position": {"x": 0.0, "y": 5.0, "z": 0.0}
                        }
                    ],
                    "children": []
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.entities.size() == 1);
        if (scene.entities.size() >= 1)
        {
            CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("ccc333ddd444"));
            CHECK(scene.entities[0].name == "TestEntity");
            CHECK(scene.entities[0].components.size() == 1);
            if (scene.entities[0].components.size() >= 1)
            {
                CHECK(scene.entities[0].components[0].componentTypeName == "Transform");

                duin::JSONValue compData = duin::JSONValue::Parse(scene.entities[0].components[0].jsonData);
                CHECK(compData["type"].GetString() == "Transform");
                CHECK(compData.HasMember("position"));
            }
        }
    }

    TEST_CASE("Deserialize scene with entity hierarchy")
    {
        std::string jsonStr = R"({
            "sceneUUID": "parent123child456",
            "sceneName": "HierarchyScene",
            "entities": [
                {
                    "uuid": "parent111111",
                    "name": "Parent",
                    "enabled": true,
                    "tags": ["root"],
                    "components": [],
                    "children": [
                        {
                            "uuid": "child2222222",
                            "name": "Child",
                            "enabled": true,
                            "tags": ["leaf"],
                            "components": [],
                            "children": []
                        }
                    ]
                }
            ]
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.entities.size() == 1);
        if (scene.entities.size() >= 1)
        {
            CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("parent111111"));
            CHECK(scene.entities[0].name == "Parent");
            CHECK(scene.entities[0].children.size() == 1);
            if (scene.entities[0].children.size() >= 1)
            {
                CHECK(scene.entities[0].children[0].uuid == duin::UUID::FromStringHex("child2222222"));
                CHECK(scene.entities[0].children[0].name == "Child");
                if (scene.entities[0].children[0].tags.size() >= 1)
                {
                    CHECK(scene.entities[0].children[0].tags[0].componentTypeName == "leaf");
                }
            }
        }
    }
}

TEST_SUITE("SceneBuilder - Round-trip Serialization")
{
    TEST_CASE("Round-trip empty scene")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("abc123def456");
        original.name = "EmptyScene";

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(deserialized.entities.empty());
        CHECK(deserialized.externalDependencies.empty());
    }

    TEST_CASE("Round-trip scene with metadata")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("def456abc789");
        original.name = "MetadataScene";
        original.metadata.editorVersion = "1.0";
        original.metadata.engineVersion = "0.1.0";
        original.metadata.lastModified = "2025-11-16T12:00:00Z";
        original.metadata.author = "DuinEditor";

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.metadata.editorVersion == deserialized.metadata.editorVersion);
        CHECK(original.metadata.engineVersion == deserialized.metadata.engineVersion);
        CHECK(original.metadata.lastModified == deserialized.metadata.lastModified);
        CHECK(original.metadata.author == deserialized.metadata.author);
    }

    TEST_CASE("Round-trip scene with dependencies")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("aabbccdd11223344");
        original.name = "DependencyScene";

        duin::PackedExternalDependency dep1;
        dep1.uuid = duin::UUID::FromStringHex("1111111111111111");
        dep1.type = "scene";
        original.externalDependencies.push_back(dep1);

        duin::PackedExternalDependency dep2;
        dep2.uuid = duin::UUID::FromStringHex("2222222222222222");
        dep2.type = "asset";
        original.externalDependencies.push_back(dep2);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.externalDependencies.size() == deserialized.externalDependencies.size());
        if (original.externalDependencies.size() >= 2 && deserialized.externalDependencies.size() >= 2)
        {
            CHECK(original.externalDependencies[0].uuid == deserialized.externalDependencies[0].uuid);
            CHECK(original.externalDependencies[0].type == deserialized.externalDependencies[0].type);
            CHECK(original.externalDependencies[1].uuid == deserialized.externalDependencies[1].uuid);
            CHECK(original.externalDependencies[1].type == deserialized.externalDependencies[1].type);
        }
    }

    TEST_CASE("Round-trip scene with entities")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("fedcba9876543210");
        original.name = "EntityScene";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("1234567890abcdef");
        entity.name = "Player";
        entity.enabled = true;

        // Add tags as PackedComponents
        duin::PackedComponent tag1;
        tag1.componentTypeName = "PxDynamic";
        tag1.jsonData = R"({"type":"PxDynamic"})";
        entity.tags.push_back(tag1);

        duin::PackedComponent tag2;
        tag2.componentTypeName = "CameraIsActive";
        tag2.jsonData = R"({"type":"CameraIsActive"})";
        entity.tags.push_back(tag2);
        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.entities.size() == deserialized.entities.size());
        if (original.entities.size() >= 1 && deserialized.entities.size() >= 1)
        {
            CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
            CHECK(original.entities[0].name == deserialized.entities[0].name);
            CHECK(original.entities[0].enabled == deserialized.entities[0].enabled);
            CHECK(original.entities[0].tags.size() == deserialized.entities[0].tags.size());
            if (original.entities[0].tags.size() >= 2 && deserialized.entities[0].tags.size() >= 2)
            {
                CHECK(original.entities[0].tags[0].componentTypeName == deserialized.entities[0].tags[0].componentTypeName);
                CHECK(original.entities[0].tags[1].componentTypeName == deserialized.entities[0].tags[1].componentTypeName);
            }
        }
    }

    TEST_CASE("Round-trip complete scene")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("550e8400e29b41d4");
        original.name = "CompleteScene";

        // Metadata
        original.metadata.editorVersion = "1.0";
        original.metadata.engineVersion = "0.1.0";
        original.metadata.lastModified = "2025-11-16T12:00:00Z";
        original.metadata.author = "DuinEditor";

        // Dependencies
        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID::FromStringHex("789e0123e89b12d3");
        dep.type = "scene";
        original.externalDependencies.push_back(dep);

        // Entities
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("123e4567e89b12d3");
        entity.name = "Player";
        entity.enabled = true;

        // Add tags as PackedComponents
        duin::PackedComponent tag1;
        tag1.componentTypeName = "PxDynamic";
        tag1.jsonData = R"({"type":"PxDynamic"})";
        entity.tags.push_back(tag1);

        duin::PackedComponent tag2;
        tag2.componentTypeName = "CameraIsActive";
        tag2.jsonData = R"({"type":"CameraIsActive"})";
        entity.tags.push_back(tag2);

        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.metadata.editorVersion == deserialized.metadata.editorVersion);
        CHECK(original.metadata.engineVersion == deserialized.metadata.engineVersion);
        CHECK(original.metadata.lastModified == deserialized.metadata.lastModified);
        CHECK(original.metadata.author == deserialized.metadata.author);
        CHECK(original.externalDependencies.size() == deserialized.externalDependencies.size());
        if (original.externalDependencies.size() >= 1 && deserialized.externalDependencies.size() >= 1)
        {
            CHECK(original.externalDependencies[0].uuid == deserialized.externalDependencies[0].uuid);
        }
        CHECK(original.entities.size() == deserialized.entities.size());
        if (original.entities.size() >= 1 && deserialized.entities.size() >= 1)
        {
            CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
            CHECK(original.entities[0].name == deserialized.entities[0].name);
        }
    }

    TEST_CASE("Round-trip entity with children")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("parent123child456");
        original.name = "HierarchyScene";

        duin::PackedEntity parent;
        parent.uuid = duin::UUID::FromStringHex("parent111111");
        parent.name = "Parent";
        parent.enabled = true;

        // Add parent tag
        duin::PackedComponent parentTag;
        parentTag.componentTypeName = "root";
        parentTag.jsonData = R"({"type":"root"})";
        parent.tags.push_back(parentTag);

        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("child2222222");
        child.name = "Child";
        child.enabled = false;

        // Add child tag
        duin::PackedComponent childTag;
        childTag.componentTypeName = "leaf";
        childTag.jsonData = R"({"type":"leaf"})";
        child.tags.push_back(childTag);

        parent.children.push_back(child);
        original.entities.push_back(parent);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.entities.size() == deserialized.entities.size());
        if (original.entities.size() >= 1 && deserialized.entities.size() >= 1)
        {
            CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
            CHECK(original.entities[0].name == deserialized.entities[0].name);
            CHECK(original.entities[0].children.size() == deserialized.entities[0].children.size());
            if (original.entities[0].children.size() >= 1 && deserialized.entities[0].children.size() >= 1)
            {
                CHECK(original.entities[0].children[0].uuid == deserialized.entities[0].children[0].uuid);
                CHECK(original.entities[0].children[0].name == deserialized.entities[0].children[0].name);
                CHECK(original.entities[0].children[0].enabled == deserialized.entities[0].children[0].enabled);
            }
        }
    }

    TEST_CASE("Round-trip entity with components")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID::FromStringHex("aaa111bbb222");
        original.name = "ComponentScene";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("ccc333ddd444");
        entity.name = "TestEntity";
        entity.enabled = true;

        duin::PackedComponent comp;
        comp.componentTypeName = "Transform";
        comp.jsonData = R"({"type":"Transform","position":{"x":1.5,"y":2.5,"z":3.5}})";
        entity.components.push_back(comp);

        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.entities.size() == deserialized.entities.size());
        if (original.entities.size() >= 1 && deserialized.entities.size() >= 1)
        {
            CHECK(original.entities[0].components.size() == deserialized.entities[0].components.size());
            if (original.entities[0].components.size() >= 1 && deserialized.entities[0].components.size() >= 1)
            {
                CHECK(original.entities[0].components[0].componentTypeName ==
                      deserialized.entities[0].components[0].componentTypeName);

                duin::JSONValue origComp = duin::JSONValue::Parse(original.entities[0].components[0].jsonData);
                duin::JSONValue deserComp = duin::JSONValue::Parse(deserialized.entities[0].components[0].jsonData);
                CHECK(origComp["type"].GetString() == deserComp["type"].GetString());
                CHECK(origComp["position"]["x"].GetDouble() == doctest::Approx(deserComp["position"]["x"].GetDouble()));
            }
        }
    }
}

} // namespace TestSceneBuilder
