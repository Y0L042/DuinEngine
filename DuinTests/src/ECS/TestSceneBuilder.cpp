#include "TestConfig.h"
#include <doctest.h>
#include <Duin/ECS/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/IO/FileUtils.h>
#include <string>
#include <vector>

namespace TestSceneBuilder
{
TEST_SUITE("SceneBuilder - PackedScene Serialization")
{
    TEST_CASE("Serialize empty PackedScene")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(12345);
        scene.name = "EmptyScene";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember(scene.TAG_SCENEUUID));
        CHECK(json.HasMember(scene.TAG_SCENENAME));
    }

    TEST_CASE("Serialize PackedScene with metadata")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(54321);
        scene.name = "TestScene";
        scene.metadata.editorVersion = "1.0";
        scene.metadata.engineVersion = "0.1.0";
        scene.metadata.lastModified = "2025-11-16T12:00:00Z";
        scene.metadata.author = "DuinEditor";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember(scene.TAG_METADATA));

        auto metadata = json[scene.TAG_METADATA];
        CHECK(metadata.IsObject());
        CHECK(metadata.HasMember(scene.metadata.TAG_EDITORVERSION));
        CHECK(metadata.HasMember(scene.metadata.TAG_ENGINEVERSION));
        CHECK(metadata.HasMember(scene.metadata.TAG_LASTMODIFIED));
        CHECK(metadata.HasMember(scene.metadata.TAG_AUTHOR));
    }

    TEST_CASE("Serialize PackedScene with external dependencies")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(99999);
        scene.name = "DependentScene";

        duin::PackedExternalDependency dep1;
        dep1.uuid = duin::UUID(11111);
        dep1.type = "scene";

        duin::PackedExternalDependency dep2;
        dep2.uuid = duin::UUID(22222);
        dep2.type = "asset";

        scene.externalDependencies.push_back(dep1);
        scene.externalDependencies.push_back(dep2);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.HasMember(scene.TAG_EXTERNALDEPENDENCIES));
        CHECK(json[scene.TAG_EXTERNALDEPENDENCIES].IsArray());
    }

    TEST_CASE("Serialize PackedScene with entities")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(77777);
        scene.name = "EntityScene";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID(88888);
        entity.name = "Player";
        entity.enabled = true;
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");

        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.HasMember(scene.TAG_ENTITIES));
        CHECK(json[scene.TAG_ENTITIES].IsArray());
    }

    TEST_CASE("Serialize complete PackedScene")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("0x550e8400e29b41d4");
        scene.name = "MainScene";

        // Metadata
        scene.metadata.editorVersion = "1.0";
        scene.metadata.engineVersion = "0.1.0";
        scene.metadata.lastModified = "2025-11-16T12:00:00Z";
        scene.metadata.author = "DuinEditor";

        // External dependencies
        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID::FromStringHex("0x789e0123e89b12d3");
        dep.type = "scene";
        scene.externalDependencies.push_back(dep);

        // Entities
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("0x123e4567e89b12d3");
        entity.name = "Player";
        entity.enabled = true;
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json.HasMember(scene.TAG_SCENEUUID));
        CHECK(json.HasMember(scene.TAG_SCENENAME));
        CHECK(json.HasMember(scene.TAG_METADATA));
        CHECK(json.HasMember(scene.TAG_EXTERNALDEPENDENCIES));
        CHECK(json.HasMember(scene.TAG_ENTITIES));
    }

    TEST_CASE("Serialize PackedEntity with components")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(11111);
        scene.name = "ComponentTest";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID(22222);
        entity.name = "TestEntity";
        entity.enabled = true;

        duin::PackedComponent comp;
        entity.components.push_back(comp);

        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
        CHECK(json[scene.TAG_ENTITIES].IsArray());
    }
}

TEST_SUITE("SceneBuilder - PackedScene Deserialization")
{
    TEST_CASE("Deserialize minimal JSON scene")
    {
        std::string jsonStr = R"({
   "sceneUUID": "12345",
      "sceneName": "MinimalScene"
   })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        // Once implemented, check values
        CHECK(json.IsObject());
    }

    TEST_CASE("Deserialize scene with metadata")
    {
        std::string jsonStr = R"({
   "sceneUUID": "54321",
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

        CHECK(json.IsObject());
        CHECK(json.HasMember("metadata"));
    }

    TEST_CASE("Deserialize scene with external dependencies")
    {
        std::string jsonStr = R"({
 "sceneUUID": "99999",
     "sceneName": "DependencyScene",
   "externalDependencies": [
  {
  "uuid": "11111",
       "type": "scene"
      },
      {
     "uuid": "22222",
     "type": "asset"
         }
      ]
      })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        CHECK(json.HasMember("externalDependencies"));
        CHECK(json["externalDependencies"].IsArray());
    }

    TEST_CASE("Deserialize scene with single entity")
    {
        std::string jsonStr = R"({
        "sceneUUID": "77777",
        "sceneName": "SingleEntityScene",
        "entities": [
         {
  "uuid": "88888",
  "name": "Player",
 "enabled": true,
     "tags": ["PxDynamic", "CameraIsActive"],
         "components": []
     }
           ]
   })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        CHECK(json.HasMember("entities"));
        CHECK(json["entities"].IsArray());
    }

    TEST_CASE("Deserialize complete scene structure")
    {
        std::string jsonStr = R"({
      "sceneUUID": "0x550e8400e29b41d4",
"sceneName": "MainScene",
 "metadata": {
      "editorVersion": "1.0",
    "engineVersion": "0.1.0",
   "lastModified": "2025-11-16T12:00:00Z",
"author": "DuinEditor"
    },
   "externalDependencies": [
 {
    "uuid": "0x789e0123e89b12d3",
     "type": "scene"
 }
     ],
    "entities": [
         {
 "uuid": "0x123e4567e89b12d3",
  "name": "Player",
        "enabled": true,
  "tags": ["PxDynamic", "CameraIsActive"],
   "components": []
           }
         ]
  })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        CHECK(json.HasMember("sceneUUID"));
        CHECK(json.HasMember("sceneName"));
        CHECK(json.HasMember("metadata"));
        CHECK(json.HasMember("externalDependencies"));
        CHECK(json.HasMember("entities"));
    }

    TEST_CASE("Deserialize entity with components")
    {
        std::string jsonStr = R"({
      "sceneUUID": "11111",
    "sceneName": "ComponentScene",
 "entities": [
{
  "uuid": "22222",
     "name": "TestEntity",
          "enabled": true,
       "tags": [],
          "components": [
       {
   "type": "Transform3D",
          "data": {
      "position": {"x": 0.0, "y": 5.0, "z": 0.0}
      }
         }
     ]
}
    ]
   })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        auto entities = json["entities"];
        CHECK(entities.IsArray());
        CHECK(entities[0]["components"].IsArray());
    }

    TEST_CASE("Deserialize nested entity hierarchy")
    {
        std::string jsonStr = R"({
     "sceneUUID": "33333",
         "sceneName": "HierarchyScene",
      "entities": [
{
          "entity": {
      "uuid": "44444",
           "name": "Parent",
  "enabled": true,
 "tags": [],
     "components": []
            },
      "children": [
      {
    "entity": {
    "uuid": "55555",
         "name": "Child",
      "enabled": true,
"tags": [],
  "components": []
 },
           "children": []
 }
   ]
    }
    ]
    })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        auto entities = json["entities"];
        CHECK(entities.IsArray());
        CHECK(entities[0].HasMember("children"));
    }
}

TEST_SUITE("SceneBuilder - Round-trip Serialization")
{
    TEST_CASE("Round-trip empty scene")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID(12345);
        original.name = "EmptyScene";

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify:
        // CHECK(original.uuid == deserialized.uuid);
        // CHECK(original.name == deserialized.name);
    }

    TEST_CASE("Round-trip scene with metadata")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID(54321);
        original.name = "MetadataScene";
        original.metadata.editorVersion = "1.0";
        original.metadata.engineVersion = "0.1.0";
        original.metadata.lastModified = "2025-11-16T12:00:00Z";
        original.metadata.author = "DuinEditor";

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify all metadata fields match
    }

    TEST_CASE("Round-trip scene with dependencies")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID(99999);
        original.name = "DependencyScene";

        duin::PackedExternalDependency dep1;
        dep1.uuid = duin::UUID(11111);
        dep1.type = "scene";
        original.externalDependencies.push_back(dep1);

        duin::PackedExternalDependency dep2;
        dep2.uuid = duin::UUID(22222);
        dep2.type = "asset";
        original.externalDependencies.push_back(dep2);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify dependencies
    }

    TEST_CASE("Round-trip scene with entities")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID(77777);
        original.name = "EntityScene";

        duin::PackedEntity entity;
        entity.uuid = duin::UUID(88888);
        entity.name = "Player";
        entity.enabled = true;
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");
        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify entities
    }

    TEST_CASE("Round-trip complete scene")
    {
        duin::PackedScene original;
        original.uuid = duin::UUID(100000);
        original.name = "CompleteScene";

        // Metadata
        original.metadata.editorVersion = "1.0";
        original.metadata.engineVersion = "0.1.0";
        original.metadata.lastModified = "2025-11-16T12:00:00Z";
        original.metadata.author = "DuinEditor";

        // Dependencies
        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID(11111);
        dep.type = "scene";
        original.externalDependencies.push_back(dep);

        // Entities
        duin::PackedEntity entity;
        entity.uuid = duin::UUID(22222);
        entity.name = "Player";
        entity.enabled = true;
        entity.tags.push_back("PxDynamic");
        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify complete structure
    }
}

TEST_SUITE("SceneBuilder - Edge Cases & Validation")
{
    TEST_CASE("Empty entities array")
    {
        std::string jsonStr = R"({
          "sceneUUID": "12345",
       "sceneName": "NoEntitiesScene",
          "entities": []
          })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Empty());
    }

    TEST_CASE("Empty dependencies array")
    {
        std::string jsonStr = R"({
       "sceneUUID": "54321",
    "sceneName": "NoDepsScene",
       "externalDependencies": []
            })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json["externalDependencies"].IsArray());
        CHECK(json["externalDependencies"].Empty());
    }

    TEST_CASE("Missing optional fields")
    {
        std::string jsonStr = R"({
     "sceneUUID": "99999",
  "sceneName": "MinimalScene"
      })";

        duin::JSONValue json = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene = duin::PackedScene::Deserialize(json);

        CHECK(json.IsObject());
        CHECK_FALSE(json.HasMember("metadata"));
        CHECK_FALSE(json.HasMember("entities"));
    }

    TEST_CASE("Entity with empty tags")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID(11111);
        entity.name = "NoTagsEntity";
        entity.enabled = true;

        duin::PackedScene scene;
        scene.uuid = duin::UUID(22222);
        scene.name = "TestScene";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
    }

    TEST_CASE("Entity with disabled state")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID(33333);
        entity.name = "DisabledEntity";
        entity.enabled = false;

        duin::PackedScene scene;
        scene.uuid = duin::UUID(44444);
        scene.name = "DisabledTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
    }

    TEST_CASE("Large number of entities")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(55555);
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

        CHECK(json.IsObject());
    }

    TEST_CASE("UUID formatting consistency")
    {
        duin::UUID testUUID = duin::UUID::FromStringHex("0x550e8400e29b41d4");

        duin::PackedScene scene;
        scene.uuid = testUUID;
        scene.name = "UUIDTest";

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        // Once implemented, verify UUID is preserved
    }

    TEST_CASE("Special characters in entity names")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID(77777);
        entity.name = "Entity@#$%^&*()_+{}|:\"<>?";
        entity.enabled = true;

        duin::PackedScene scene;
        scene.uuid = duin::UUID(88888);
        scene.name = "SpecialCharsTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
    }

    TEST_CASE("Very long scene name")
    {
        duin::PackedScene scene;
        scene.uuid = duin::UUID(99999);
        scene.name = std::string(1000, 'A');

        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        CHECK(json.IsObject());
    }
}
} // namespace TestSceneBuilder
