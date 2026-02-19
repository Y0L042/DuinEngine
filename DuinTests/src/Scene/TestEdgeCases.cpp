#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("SceneBuilder - Edge Cases & Validation")
{
    TEST_CASE("Empty entities array")
    {
        std::string jsonStr = R"({
            "sceneUUID": "abc123def456",
            "sceneName": "NoEntitiesScene",
            "entities": []
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("abc123def456"));
        CHECK(scene.name == "NoEntitiesScene");
        CHECK(scene.entities.empty());
    }

    TEST_CASE("Empty dependencies array")
    {
        std::string jsonStr = R"({
            "sceneUUID": "def456abc789",
            "sceneName": "NoDepsScene",
            "externalDependencies": []
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("def456abc789"));
        CHECK(scene.name == "NoDepsScene");
        CHECK(scene.externalDependencies.empty());
    }

    TEST_CASE("Missing optional fields")
    {
        std::string jsonStr = R"({
            "sceneUUID": "aabbccdd11223344",
            "sceneName": "MinimalScene"
        })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == duin::UUID::FromStringHex("aabbccdd11223344"));
        CHECK(scene.name == "MinimalScene");
        CHECK(scene.entities.empty());
        CHECK(scene.externalDependencies.empty());
        CHECK(scene.metadata.editorVersion.empty());
        CHECK(scene.metadata.engineVersion.empty());
        CHECK(scene.metadata.lastModified.empty());
        CHECK(scene.metadata.author.empty());
    }

    TEST_CASE("Entity with empty tags")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("1111111111111111");
        entity.name = "NoTagsEntity";
        entity.enabled = true;

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("2222222222222222");
        scene.name = "TestScene";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(deserialized.entities.size() == 1);
        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].tags.empty());
        }
    }

    TEST_CASE("Entity with disabled state")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("3333333333333333");
        entity.name = "DisabledEntity";
        entity.enabled = false;

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("4444444444444444");
        scene.name = "DisabledTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(deserialized.entities.size() == 1);
        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].enabled == false);
        }
    }

    TEST_CASE("Large number of entities")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("5555555555555555");
        scene.name = "ManyEntities";

        for (int i = 0; i < 100; ++i)
        {
            duin::PackedEntity entity;
            entity.uuid = duin::UUID(60000 + i);
            entity.name = "Entity_" + std::to_string(i);
            entity.enabled = true;
            scene.entities.push_back(entity);
        }

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(deserialized.entities.size() == 100);
        if (deserialized.entities.size() >= 100)
        {
            CHECK(deserialized.entities[0].name == "Entity_0");
            CHECK(deserialized.entities[99].name == "Entity_99");
        }
    }

    TEST_CASE("UUID formatting consistency")
    {
        duin::UUID testUUID = duin::UUID::FromStringHex("550e8400e29b41d4");

        duin::PackedScene scene;
        scene.uuid = testUUID;
        scene.name = "UUIDTest";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(scene.uuid == deserialized.uuid);
        CHECK(json["sceneUUID"].GetString() == testUUID.ToStrHex());
    }

    TEST_CASE("Special characters in entity names")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("7777777777777777");
        entity.name = "Entity_@#$%^&*()_+";
        entity.enabled = true;

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("8888888888888888");
        scene.name = "SpecialCharsTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].name == "Entity_@#$%^&*()_+");
        }
    }

    TEST_CASE("Very long scene name")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("9999999999999999");
        scene.name = std::string(1000, 'A');

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(deserialized.name.size() == 1000);
        CHECK(deserialized.name == scene.name);
    }

    TEST_CASE("Empty component array")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("aaaaaaaaaaaaaaaa");
        entity.name = "NoComponents";
        entity.enabled = true;

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("bbbbbbbbbbbbbbbb");
        scene.name = "EmptyComponentTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].components.empty());
        }
    }

    TEST_CASE("Multiple tags per entity")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("cccccccccccccccc");
        entity.name = "MultiTagEntity";
        entity.enabled = true;
        
        // Create tags as PackedComponents
        for (int i = 1; i <= 5; i++)
        {
            duin::PackedComponent tag;
            tag.componentTypeName = "tag" + std::to_string(i);
            tag.jsonData = R"({"type":")" + tag.componentTypeName + R"("})";
            entity.tags.push_back(tag);
        }

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("dddddddddddddddd");
        scene.name = "MultiTagTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].tags.size() == 5);
            if (deserialized.entities[0].tags.size() >= 5)
            {
                CHECK(deserialized.entities[0].tags[0].componentTypeName == "tag1");
                CHECK(deserialized.entities[0].tags[4].componentTypeName == "tag5");
            }
        }
    }

    TEST_CASE("Deep entity hierarchy")
    {
        duin::PackedEntity level3;
        level3.uuid = duin::UUID::FromStringHex("eeeeeeeeeeeeeeee");
        level3.name = "Level3";
        level3.enabled = true;

        duin::PackedEntity level2;
        level2.uuid = duin::UUID::FromStringHex("ffffffffffffffff");
        level2.name = "Level2";
        level2.enabled = true;
        level2.children.push_back(level3);

        duin::PackedEntity level1;
        level1.uuid = duin::UUID::FromStringHex("0000000000000001");
        level1.name = "Level1";
        level1.enabled = true;
        level1.children.push_back(level2);

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("0000000000000002");
        scene.name = "DeepHierarchy";
        scene.entities.push_back(level1);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].children.size() == 1);
            if (deserialized.entities[0].children.size() >= 1)
            {
                CHECK(deserialized.entities[0].children[0].children.size() == 1);
                if (deserialized.entities[0].children[0].children.size() >= 1)
                {
                    CHECK(deserialized.entities[0].children[0].children[0].name == "Level3");
                }
            }
        }
    }

    TEST_CASE("Multiple components per entity")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("0000000000000003");
        entity.name = "MultiComponentEntity";
        entity.enabled = true;

        duin::PackedComponent comp1;
        comp1.componentTypeName = "Transform";
        comp1.jsonData = R"({"type":"Transform","position":{"x":0.0,"y":0.0,"z":0.0}})";
        entity.components.push_back(comp1);

        duin::PackedComponent comp2;
        comp2.componentTypeName = "RigidBody";
        comp2.jsonData = R"({"type":"RigidBody","mass":1.0})";
        entity.components.push_back(comp2);

        duin::PackedComponent comp3;
        comp3.componentTypeName = "MeshRenderer";
        comp3.jsonData = R"({"type":"MeshRenderer","meshPath":"test.mesh"})";
        entity.components.push_back(comp3);

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("0000000000000004");
        scene.name = "MultiComponentTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        if (deserialized.entities.size() >= 1)
        {
            CHECK(deserialized.entities[0].components.size() == 3);
            if (deserialized.entities[0].components.size() >= 3)
            {
                CHECK(deserialized.entities[0].components[0].componentTypeName == "Transform");
                CHECK(deserialized.entities[0].components[1].componentTypeName == "RigidBody");
                CHECK(deserialized.entities[0].components[2].componentTypeName == "MeshRenderer");
            }
        }
    }
}

} // namespace TestSceneBuilder
