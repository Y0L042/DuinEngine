#include "TestConfig.h"
#include <doctest.h>
#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/IO/FileUtils.h>
#include <Duin/ECS/ECSModule.h>
#include <string>
#include <vector>

namespace TestSceneBuilder
{
const std::string FULL_ENTITY_JSON = R"({
  "uuid": "f5a3e8d1c9b2a7f4",
  "name": "PlayerCharacter",
  "tags": ["player", "controllable", "persistent"],
  "enabled": true,
  "components": [
    {
      "type": "Transform",
      "position": {"x": 10.5, "y": 0.0, "z": -3.2},
      "rotation": {"x": 0.0, "y": 45.0, "z": 0.0},
      "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
    },
    {
      "type": "RigidBody",
      "mass": 75.0,
      "friction": 0.5,
      "restitution": 0.2,
      "kinematic": false
    },
    {
      "type": "MeshRenderer",
      "meshPath": "assets/models/character.mesh",
      "materialPath": "assets/materials/character_skin.mat",
      "castShadows": true
    }
  ],
  "children": [
    {
      "uuid": "1a2b3c4d5e6f7890",
      "name": "Camera",
      "tags": ["camera"],
      "enabled": true,
      "components": [
        {
          "type": "Transform",
          "position": {"x": 0.0, "y": 1.8, "z": 0.5},
          "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
          "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        },
        {
          "type": "Camera",
          "fov": 75.0,
          "nearPlane": 0.1,
          "farPlane": 1000.0,
          "isPrimary": true
        }
      ],
      "children": []
    },
    {
      "uuid": "9876543210abcdef",
      "name": "WeaponSlot",
      "tags": ["equipment"],
      "enabled": false,
      "components": [
        {
          "type": "Transform",
          "position": {"x": 0.3, "y": 1.2, "z": 0.4},
          "rotation": {"x": 0.0, "y": 90.0, "z": 0.0},
          "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        }
      ],
      "children": []
    }
  ]
})";

const std::string SIMPLE_ENTITY_JSON = R"(   
{
    "uuid": "1a2b3c4d5e6f7890",
    "name": "Camera",
    "tags": ["camera"],
    "enabled": true,
    "components": 
    [
        {
            "type": "Transform",
            "position": {"x": 0.0, "y": 1.8, "z": 0.5},
            "rotation": {"x": 0.0, "y": 0.0, "z": 0.0},
            "scale": {"x": 1.0, "y": 1.0, "z": 1.0}
        },
        {
            "type": "Camera",
            "fov": 75.0,
            "nearPlane": 0.1,
            "farPlane": 1000.0,
            "isPrimary": true
        }
    ],
    "children": []
})";

struct Vec3
{
    float x, y, z;
};
struct TestStructX
{
    int x;
};
struct Transform
{
    Vec3 position, rotation, scale;
};
struct Camera
{
    float fov, nearPlane, farPlane;
    bool isPrimary;
};

TEST_SUITE("Component Serialization")
{
    TEST_CASE("Deserialize Component")
    {
        std::string jsonStr = R"(
        {   
            "type" : "TestStructX",
            "x" : "3"
        })";
        std::string normalizedJsonStr = duin::JSONValue::Parse(jsonStr).Write();
        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);

        duin::PackedComponent p = duin::PackedComponent::Deserialize(v);

        CHECK(p.jsonData == normalizedJsonStr);
    }

    TEST_CASE("Serialize Component")
    {
        std::string jsonStr = R"({"type":"TestStructX","x":"3"})";
        duin::PackedComponent p{.jsonData = jsonStr};

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);

        std::string pstr = duin::PackedComponent::Serialize(p).Write();

        CHECK(pstr == jsonStr);
    }
}

TEST_SUITE("Simple Entity Serialization")
{
    TEST_CASE("Deserialize Entity")
    {
        duin::JSONValue v = duin::JSONValue::Parse(SIMPLE_ENTITY_JSON);

        duin::PackedEntity p = duin::PackedEntity::Deserialize(v);

        CHECK(p.uuid == duin::UUID::FromStringHex("1a2b3c4d5e6f7890"));
        CHECK(p.name == "Camera");
        CHECK(p.enabled == true);
        CHECK(p.tags.size() == 1);
        CHECK(p.tags[0] == "camera");
        CHECK(p.components.size() == 2);
        CHECK(p.children.size() == 0);

        int txIdx = p.components[0].componentTypeName == "Transform" ? 0 : 1;
        int cmIdx = p.components[1].componentTypeName == "Camera" ? 1 : 0;

        // Check first component (Transform)
        CHECK(p.components[txIdx].componentTypeName == "Transform");
        duin::JSONValue comp0 = duin::JSONValue::Parse(p.components[txIdx].jsonData);
        CHECK(comp0.HasMember("type"));
        CHECK(comp0["type"].GetString() == "Transform");
        CHECK(comp0.HasMember("position"));
        CHECK(comp0["position"]["x"].GetDouble() == doctest::Approx(0.0));
        CHECK(comp0["position"]["y"].GetDouble() == doctest::Approx(1.8));
        CHECK(comp0["position"]["z"].GetDouble() == doctest::Approx(0.5));

        // Check second component (Camera)
        CHECK(p.components[cmIdx].componentTypeName == "Camera");
        duin::JSONValue comp1 = duin::JSONValue::Parse(p.components[cmIdx].jsonData);
        CHECK(comp1.HasMember("type"));
        CHECK(comp1["type"].GetString() == "Camera");
        CHECK(comp1["fov"].GetDouble() == doctest::Approx(75.0));
        CHECK(comp1["nearPlane"].GetDouble() == doctest::Approx(0.1));
        CHECK(comp1["farPlane"].GetDouble() == doctest::Approx(1000.0));
        CHECK(comp1["isPrimary"].GetBool() == true);
    }

    TEST_CASE("Serialize Entity")
    {
        // Create a PackedEntity with expected values
        duin::PackedEntity p;
        p.uuid = duin::UUID::FromStringHex("1a2b3c4d5e6f7890");
        p.name = "Camera";
        p.enabled = true;
        p.tags.push_back("camera");

        // Add Transform component
        duin::PackedComponent transformComp;
        transformComp.componentTypeName = "Transform";
        transformComp.jsonData = R"({"type":"Transform","position":{"x":0.0,"y":1.8,"z":0.5},"rotation":{"x":0.0,"y":0.0,"z":0.0},"scale":{"x":1.0,"y":1.0,"z":1.0}})";
        p.components.push_back(transformComp);
        
        // Add Camera component
        duin::PackedComponent cameraComp;
        cameraComp.componentTypeName = "Camera";
        cameraComp.jsonData = R"({"type":"Camera","fov":75.0,"nearPlane":0.1,"farPlane":1000.0,"isPrimary":true})";
        p.components.push_back(cameraComp);
        
        // Serialize to JSON
        duin::JSONValue json = duin::PackedEntity::Serialize(p);
        
        // Verify JSON structure
        CHECK(json.IsObject());
        CHECK(json.HasMember("uuid"));
        CHECK(json["uuid"].GetString() == duin::UUID::FromStringHex("1a2b3c4d5e6f7890").ToStrHex());
        CHECK(json.HasMember("name"));
        CHECK(json["name"].GetString() == "Camera");
        CHECK(json.HasMember("enabled"));
        CHECK(json["enabled"].GetBool() == true);
        CHECK(json.HasMember("tags"));
        CHECK(json["tags"].IsArray());
        CHECK(json["tags"][0].GetString() == "camera");
        CHECK(json.HasMember("components"));
        CHECK(json["components"].IsArray());
        CHECK(json["components"].Capacity() == 2);
        CHECK(json.HasMember("children"));
        CHECK(json["children"].IsArray());
        CHECK(json["children"].Empty());
        
        // Verify component structure
        duin::JSONValue comp0 = json["components"][0];
        CHECK(comp0.HasMember("type"));
        CHECK(comp0["type"].GetString() == "Transform");
        
        duin::JSONValue comp1 = json["components"][1];
        CHECK(comp1.HasMember("type"));
        CHECK(comp1["type"].GetString() == "Camera");
    }
    TEST_CASE("Pack Entity")
    {
        WARN("EMPTY");
    }
    TEST_CASE("Unpack Entity")
    {
        WARN("EMPTY");
    }
}

#if 0
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
#endif
} // namespace TestSceneBuilder
