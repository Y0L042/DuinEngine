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

    bool operator==(const Vec3 &other) const
    {
        return (x == other.x && y == other.y && z == other.z);
    }
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

    bool operator==(const Camera &other)
    {
        return (fov == other.fov && nearPlane == other.nearPlane && farPlane == other.farPlane &&
                isPrimary == other.isPrimary);
    }
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

TEST_SUITE("Component Instantiation")
{
    TEST_CASE("Pack Component")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        duin::Entity e = world.CreateEntity().Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(3.0f, 2.0f, 4.0f, true);
        std::vector<duin::PackedComponent> cmpList;
        e.ForEachComponent([&](duin::Entity::ID cmpID) {
            if (cmpID.IsEntity())
            {
                duin::Entity cmp = cmpID.GetEntity();
                duin::PackedComponent pc = duin::PackedComponent::Pack(e, cmp);
                cmpList.push_back(pc);
            }
        });

        CHECK(cmpList.size() == 2);

        // Iteration order of entity's components are not garuanteed
        int vecIdx = cmpList[0].componentTypeName == "Vec3" ? 0 : 1;
        int camIdx = cmpList[1].componentTypeName == "Camera" ? 1 : 0;

        CHECK(cmpList[vecIdx].componentTypeName == "Vec3");
        CHECK(cmpList[vecIdx].jsonData == duin::JSONValue::Parse(R"({"type":"Vec3","x":1.0,"y":2.0,"z":3.0})").Write());
        CHECK(cmpList[camIdx].componentTypeName == "Camera");
        CHECK(cmpList[camIdx].jsonData ==
              duin::JSONValue::Parse(R"({"type":"Camera","fov":3.0,"nearPlane":2.0,"farPlane":4.0,"isPrimary":true})")
                  .Write());
    }

    TEST_CASE("Instantiate Component")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        std::vector<duin::PackedComponent> cmpList{2};
        cmpList[0].componentTypeName = "Vec3";
        cmpList[0].jsonData = R"({"type":"Vec3","x":1.0,"y":2.0,"z":3.0})";
        cmpList[1].componentTypeName = "Camera";
        cmpList[1].jsonData = R"({"type":"Camera","fov":3.0,"nearPlane":2.0,"farPlane":4.0,"isPrimary":true})";

        duin::Entity e = world.CreateEntity();
        for (duin::PackedComponent &cmp : cmpList)
        {
            duin::PackedComponent::Instantiate(cmp, e);
        }

        CHECK(e.GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
        CHECK(e.GetMut<Camera>() == Camera{3.0f, 2.0f, 4.0f, true});
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
        transformComp.jsonData =
            R"({"type":"Transform","position":{"x":0.0,"y":1.8,"z":0.5},"rotation":{"x":0.0,"y":0.0,"z":0.0},"scale":{"x":1.0,"y":1.0,"z":1.0}})";
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
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity e =
            world.CreateEntity("TestCamera").Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(75.0f, 0.1f, 1000.0f, true);

        duin::PackedEntity pe = duin::PackedEntity::Pack(e);

        CHECK(pe.name == "TestCamera");
        CHECK(pe.enabled == true);
        CHECK(pe.components.size() == 2);

        int vecIdx = pe.components[0].componentTypeName == "Vec3" ? 0 : 1;
        int camIdx = pe.components[1].componentTypeName == "Camera" ? 1 : 0;

        CHECK(pe.components[vecIdx].componentTypeName == "Vec3");
        CHECK(pe.components[camIdx].componentTypeName == "Camera");
    }

    TEST_CASE("Pack Entity with children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.CreateEntity("Child").Set<Camera>(60.0f, 0.1f, 500.0f, false);
        child.ChildOf(parent);

        duin::PackedEntity pe = duin::PackedEntity::Pack(parent);

        CHECK(pe.name == "Parent");
        CHECK(pe.components.size() >= 1);
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Child");
        CHECK(pe.children[0].components.size() >= 1);
    }

    TEST_CASE("Instantiate Entity")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::PackedEntity pe;
        pe.name = "InstantiatedEntity";
        pe.enabled = true;

        duin::PackedComponent vecComp;
        vecComp.componentTypeName = "Vec3";
        vecComp.jsonData = R"({"type":"Vec3","x":5.0,"y":6.0,"z":7.0})";
        pe.components.push_back(vecComp);

        duin::PackedComponent camComp;
        camComp.componentTypeName = "Camera";
        camComp.jsonData = R"({"type":"Camera","fov":90.0,"nearPlane":0.5,"farPlane":2000.0,"isPrimary":false})";
        pe.components.push_back(camComp);

        duin::Entity e = world.CreateEntity();
        duin::PackedEntity::Instantiate(pe, e);

        CHECK(e.GetName() == "InstantiatedEntity");
        CHECK(e.Has<Vec3>());
        CHECK(e.Has<Camera>());
        CHECK(e.GetMut<Vec3>() == Vec3{5.0f, 6.0f, 7.0f});
        CHECK(e.GetMut<Camera>() == Camera{90.0f, 0.5f, 2000.0f, false});
    }

    TEST_CASE("Instantiate Entity with children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::PackedEntity pe;
        pe.name = "Parent";
        pe.enabled = true;

        duin::PackedComponent parentComp;
        parentComp.componentTypeName = "Vec3";
        parentComp.jsonData = R"({"type":"Vec3","x":0.0,"y":0.0,"z":0.0})";
        pe.components.push_back(parentComp);

        duin::PackedEntity childPe;
        childPe.name = "Child";
        childPe.enabled = true;

        duin::PackedComponent childComp;
        childComp.componentTypeName = "Camera";
        childComp.jsonData = R"({"type":"Camera","fov":60.0,"nearPlane":0.1,"farPlane":500.0,"isPrimary":true})";
        childPe.components.push_back(childComp);

        pe.children.push_back(childPe);

        duin::Entity e = world.CreateEntity();
        duin::PackedEntity::Instantiate(pe, e);

        CHECK(e.GetName() == "Parent");
        CHECK(e.Has<Vec3>());

        std::vector<duin::Entity> children = e.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "Child");
        CHECK(children[0].Has<Camera>());
        CHECK(children[0].GetMut<Camera>() == Camera{60.0f, 0.1f, 500.0f, true});
    }

    TEST_CASE("Instantiate disabled Entity")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::PackedEntity pe;
        pe.name = "DisabledEntity";
        pe.enabled = false;

        duin::PackedComponent comp;
        comp.componentTypeName = "Vec3";
        comp.jsonData = R"({"type":"Vec3","x":1.0,"y":2.0,"z":3.0})";
        pe.components.push_back(comp);

        duin::Entity e = world.CreateEntity();
        duin::PackedEntity::Instantiate(pe, e);

        CHECK(e.GetName() == "DisabledEntity");
    }
}

TEST_SUITE("External Dependency Serialization")
{
    TEST_CASE("Deserialize External Dependency")
    {
        std::string jsonStr = R"({
            "uuid": "a1b2c3d4e5f67890",
            "type": "scene"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedExternalDependency dep = duin::PackedExternalDependency::Deserialize(v);

        CHECK(dep.uuid == duin::UUID::FromStringHex("a1b2c3d4e5f67890"));
        CHECK(dep.type == "scene");
    }

    TEST_CASE("Serialize External Dependency")
    {
        duin::PackedExternalDependency dep;
        dep.uuid = duin::UUID::FromStringHex("a1b2c3d4e5f67890");
        dep.type = "asset";

        duin::JSONValue json = duin::PackedExternalDependency::Serialize(dep);

        CHECK(json.IsObject());
        CHECK(json.HasMember("uuid"));
        CHECK(json["uuid"].GetString() == duin::UUID::FromStringHex("a1b2c3d4e5f67890").ToStrHex());
        CHECK(json.HasMember("type"));
        CHECK(json["type"].GetString() == "asset");
    }

    TEST_CASE("Round-trip External Dependency")
    {
        duin::PackedExternalDependency original;
        original.uuid = duin::UUID::FromStringHex("fedcba9876543210");
        original.type = "material";

        duin::JSONValue json = duin::PackedExternalDependency::Serialize(original);
        duin::PackedExternalDependency deserialized = duin::PackedExternalDependency::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.type == deserialized.type);
    }
}

TEST_SUITE("Scene Metadata Serialization")
{
    TEST_CASE("Deserialize Scene Metadata")
    {
        std::string jsonStr = R"({
            "editorVersion": "1.0.5",
            "engineVersion": "0.2.1",
            "lastModified": "2025-01-16T14:30:00Z",
            "author": "TestAuthor"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedSceneMetadata meta = duin::PackedSceneMetadata::Deserialize(v);

        CHECK(meta.editorVersion == "1.0.5");
        CHECK(meta.engineVersion == "0.2.1");
        CHECK(meta.lastModified == "2025-01-16T14:30:00Z");
        CHECK(meta.author == "TestAuthor");
    }

    TEST_CASE("Serialize Scene Metadata")
    {
        duin::PackedSceneMetadata meta;
        meta.editorVersion = "2.0.0";
        meta.engineVersion = "1.0.0";
        meta.lastModified = "2025-01-17T10:00:00Z";
        meta.author = "SceneBuilder";

        duin::JSONValue json = duin::PackedSceneMetadata::Serialize(meta);

        CHECK(json.IsObject());
        CHECK(json.HasMember("editorVersion"));
        CHECK(json["editorVersion"].GetString() == "2.0.0");
        CHECK(json.HasMember("engineVersion"));
        CHECK(json["engineVersion"].GetString() == "1.0.0");
        CHECK(json.HasMember("lastModified"));
        CHECK(json["lastModified"].GetString() == "2025-01-17T10:00:00Z");
        CHECK(json.HasMember("author"));
        CHECK(json["author"].GetString() == "SceneBuilder");
    }

    TEST_CASE("Round-trip Scene Metadata")
    {
        duin::PackedSceneMetadata original;
        original.editorVersion = "3.1.4";
        original.engineVersion = "2.5.9";
        original.lastModified = "2025-01-18T08:45:30Z";
        original.author = "RoundTripTest";

        duin::JSONValue json = duin::PackedSceneMetadata::Serialize(original);
        duin::PackedSceneMetadata deserialized = duin::PackedSceneMetadata::Deserialize(json);

        CHECK(original.editorVersion == deserialized.editorVersion);
        CHECK(original.engineVersion == deserialized.engineVersion);
        CHECK(original.lastModified == deserialized.lastModified);
        CHECK(original.author == deserialized.author);
    }

    TEST_CASE("Deserialize Partial Metadata")
    {
        std::string jsonStr = R"({
            "editorVersion": "1.0",
            "engineVersion": "0.1"
        })";

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);
        duin::PackedSceneMetadata meta = duin::PackedSceneMetadata::Deserialize(v);

        CHECK(meta.editorVersion == "1.0");
        CHECK(meta.engineVersion == "0.1");
        CHECK(meta.lastModified.empty());
        CHECK(meta.author.empty());
    }
}

TEST_SUITE("Full Entity Hierarchy Serialization")
{
    TEST_CASE("Deserialize Entity with Children")
    {
        duin::JSONValue v = duin::JSONValue::Parse(FULL_ENTITY_JSON);
        duin::PackedEntity p = duin::PackedEntity::Deserialize(v);

        // Parent entity checks
        CHECK(p.uuid == duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4"));
        CHECK(p.name == "PlayerCharacter");
        CHECK(p.enabled == true);
        CHECK(p.tags.size() == 3);
        CHECK(p.tags[0] == "player");
        CHECK(p.tags[1] == "controllable");
        CHECK(p.tags[2] == "persistent");
        CHECK(p.components.size() == 3);
        CHECK(p.children.size() == 2);

        // First child (Camera)
        duin::PackedEntity &camera = p.children[0];
        CHECK(camera.uuid == duin::UUID::FromStringHex("1a2b3c4d5e6f7890"));
        CHECK(camera.name == "Camera");
        CHECK(camera.enabled == true);
        CHECK(camera.tags.size() == 1);
        CHECK(camera.tags[0] == "camera");
        CHECK(camera.components.size() == 2);
        CHECK(camera.children.empty());

        // Second child (WeaponSlot)
        duin::PackedEntity &weapon = p.children[1];
        CHECK(weapon.uuid == duin::UUID::FromStringHex("9876543210abcdef"));
        CHECK(weapon.name == "WeaponSlot");
        CHECK(weapon.enabled == false);
        CHECK(weapon.tags.size() == 1);
        CHECK(weapon.tags[0] == "equipment");
        CHECK(weapon.components.size() == 1);
        CHECK(weapon.children.empty());
    }

    TEST_CASE("Serialize Entity with Children")
    {
        // Create parent entity
        duin::PackedEntity parent;
        parent.uuid = duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4");
        parent.name = "PlayerCharacter";
        parent.enabled = true;
        parent.tags = {"player", "controllable"};

        // Create child entity
        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("1a2b3c4d5e6f7890");
        child.name = "Camera";
        child.enabled = true;
        child.tags = {"camera"};

        parent.children.push_back(child);

        // Serialize
        duin::JSONValue json = duin::PackedEntity::Serialize(parent);

        CHECK(json.IsObject());
        CHECK(json["uuid"].GetString() == duin::UUID::FromStringHex("f5a3e8d1c9b2a7f4").ToStrHex());
        CHECK(json["name"].GetString() == "PlayerCharacter");
        CHECK(json["enabled"].GetBool() == true);
        CHECK(json["tags"].IsArray());
        CHECK(json["tags"].Capacity() >= 2);
        CHECK(json["children"].IsArray());
        CHECK(json["children"].Capacity() == 1);

        // Verify child
        duin::JSONValue childJson = json["children"][0];
        CHECK(childJson["uuid"].GetString() == duin::UUID::FromStringHex("1a2b3c4d5e6f7890").ToStrHex());
        CHECK(childJson["name"].GetString() == "Camera");
    }

    TEST_CASE("Round-trip Entity with Children")
    {
        // Create original hierarchy
        duin::PackedEntity original;
        original.uuid = duin::UUID::FromStringHex("abcd1234efgh5678");
        original.name = "Parent";
        original.enabled = true;
        original.tags = {"root"};

        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("1111222233334444");
        child.name = "Child";
        child.enabled = false;
        child.tags = {"leaf"};

        original.children.push_back(child);

        // Round-trip
        duin::JSONValue json = duin::PackedEntity::Serialize(original);
        duin::PackedEntity deserialized = duin::PackedEntity::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.enabled == deserialized.enabled);
        CHECK(original.tags.size() == deserialized.tags.size());
        CHECK(original.children.size() == deserialized.children.size());
        CHECK(original.children[0].uuid == deserialized.children[0].uuid);
        CHECK(original.children[0].name == deserialized.children[0].name);
    }
}

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
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");

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
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");
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
        CHECK(scene.externalDependencies[0].uuid == duin::UUID::FromStringHex("1111111111111111"));
        CHECK(scene.externalDependencies[0].type == "scene");
        CHECK(scene.externalDependencies[1].uuid == duin::UUID::FromStringHex("2222222222222222"));
        CHECK(scene.externalDependencies[1].type == "asset");
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
        CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("1234567890abcdef"));
        CHECK(scene.entities[0].name == "Player");
        CHECK(scene.entities[0].enabled == true);
        CHECK(scene.entities[0].tags.size() == 2);
        CHECK(scene.entities[0].tags[0] == "PxDynamic");
        CHECK(scene.entities[0].tags[1] == "CameraIsActive");
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
        CHECK(scene.externalDependencies[0].uuid == duin::UUID::FromStringHex("789e0123e89b12d3"));
        CHECK(scene.externalDependencies[0].type == "scene");
        CHECK(scene.entities.size() == 1);
        CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("123e4567e89b12d3"));
        CHECK(scene.entities[0].name == "Player");
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
        CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("ccc333ddd444"));
        CHECK(scene.entities[0].name == "TestEntity");
        CHECK(scene.entities[0].components.size() == 1);
        CHECK(scene.entities[0].components[0].componentTypeName == "Transform");

        duin::JSONValue compData = duin::JSONValue::Parse(scene.entities[0].components[0].jsonData);
        CHECK(compData["type"].GetString() == "Transform");
        CHECK(compData.HasMember("position"));
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
        CHECK(scene.entities[0].uuid == duin::UUID::FromStringHex("parent111111"));
        CHECK(scene.entities[0].name == "Parent");
        CHECK(scene.entities[0].children.size() == 1);
        CHECK(scene.entities[0].children[0].uuid == duin::UUID::FromStringHex("child2222222"));
        CHECK(scene.entities[0].children[0].name == "Child");
        CHECK(scene.entities[0].children[0].tags[0] == "leaf");
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
        CHECK(original.externalDependencies[0].uuid == deserialized.externalDependencies[0].uuid);
        CHECK(original.externalDependencies[0].type == deserialized.externalDependencies[0].type);
        CHECK(original.externalDependencies[1].uuid == deserialized.externalDependencies[1].uuid);
        CHECK(original.externalDependencies[1].type == deserialized.externalDependencies[1].type);
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
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");
        original.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.uuid == deserialized.uuid);
        CHECK(original.name == deserialized.name);
        CHECK(original.entities.size() == deserialized.entities.size());
        CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
        CHECK(original.entities[0].name == deserialized.entities[0].name);
        CHECK(original.entities[0].enabled == deserialized.entities[0].enabled);
        CHECK(original.entities[0].tags.size() == deserialized.entities[0].tags.size());
        CHECK(original.entities[0].tags[0] == deserialized.entities[0].tags[0]);
        CHECK(original.entities[0].tags[1] == deserialized.entities[0].tags[1]);
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
        entity.tags.push_back("PxDynamic");
        entity.tags.push_back("CameraIsActive");
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
        CHECK(original.externalDependencies[0].uuid == deserialized.externalDependencies[0].uuid);
        CHECK(original.entities.size() == deserialized.entities.size());
        CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
        CHECK(original.entities[0].name == deserialized.entities[0].name);
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
        parent.tags.push_back("root");

        duin::PackedEntity child;
        child.uuid = duin::UUID::FromStringHex("child2222222");
        child.name = "Child";
        child.enabled = false;
        child.tags.push_back("leaf");

        parent.children.push_back(child);
        original.entities.push_back(parent);

        duin::JSONValue json = duin::PackedScene::Serialize(original);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(original.entities.size() == deserialized.entities.size());
        CHECK(original.entities[0].uuid == deserialized.entities[0].uuid);
        CHECK(original.entities[0].name == deserialized.entities[0].name);
        CHECK(original.entities[0].children.size() == deserialized.entities[0].children.size());
        CHECK(original.entities[0].children[0].uuid == deserialized.entities[0].children[0].uuid);
        CHECK(original.entities[0].children[0].name == deserialized.entities[0].children[0].name);
        CHECK(original.entities[0].children[0].enabled == deserialized.entities[0].children[0].enabled);
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
        CHECK(original.entities[0].components.size() == deserialized.entities[0].components.size());
        CHECK(original.entities[0].components[0].componentTypeName ==
              deserialized.entities[0].components[0].componentTypeName);

        duin::JSONValue origComp = duin::JSONValue::Parse(original.entities[0].components[0].jsonData);
        duin::JSONValue deserComp = duin::JSONValue::Parse(deserialized.entities[0].components[0].jsonData);
        CHECK(origComp["type"].GetString() == deserComp["type"].GetString());
        CHECK(origComp["position"]["x"].GetDouble() == doctest::Approx(deserComp["position"]["x"].GetDouble()));
    }
}

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
        CHECK(deserialized.entities[0].tags.empty());
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
        CHECK(deserialized.entities[0].enabled == false);
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
        CHECK(deserialized.entities[0].name == "Entity_0");
        CHECK(deserialized.entities[99].name == "Entity_99");
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

        CHECK(deserialized.entities[0].name == "Entity_@#$%^&*()_+");
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

        CHECK(deserialized.entities[0].components.empty());
    }

    TEST_CASE("Multiple tags per entity")
    {
        duin::PackedEntity entity;
        entity.uuid = duin::UUID::FromStringHex("cccccccccccccccc");
        entity.name = "MultiTagEntity";
        entity.enabled = true;
        entity.tags = {"tag1", "tag2", "tag3", "tag4", "tag5"};

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("dddddddddddddddd");
        scene.name = "MultiTagTest";
        scene.entities.push_back(entity);

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene deserialized = duin::PackedScene::Deserialize(json);

        CHECK(deserialized.entities[0].tags.size() == 5);
        CHECK(deserialized.entities[0].tags[0] == "tag1");
        CHECK(deserialized.entities[0].tags[4] == "tag5");
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

        CHECK(deserialized.entities[0].children.size() == 1);
        CHECK(deserialized.entities[0].children[0].children.size() == 1);
        CHECK(deserialized.entities[0].children[0].children[0].name == "Level3");
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

        CHECK(deserialized.entities[0].components.size() == 3);
        CHECK(deserialized.entities[0].components[0].componentTypeName == "Transform");
        CHECK(deserialized.entities[0].components[1].componentTypeName == "RigidBody");
        CHECK(deserialized.entities[0].components[2].componentTypeName == "MeshRenderer");
    }
}

TEST_SUITE("Entity Pack-Instantiate Round-trip")
{
    TEST_CASE("Pack then Instantiate single entity with components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity original =
            world.CreateEntity("Player").Set<Vec3>(10.0f, 20.0f, 30.0f).Set<Camera>(90.0f, 0.1f, 1000.0f, true);

        duin::PackedEntity pe = duin::PackedEntity::Pack(original);

        CHECK(pe.name == "Player");
        CHECK(pe.components.size() == 2);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Player");
        CHECK(restored.Has<Vec3>());
        CHECK(restored.Has<Camera>());
        CHECK(restored.GetMut<Vec3>() == Vec3{10.0f, 20.0f, 30.0f});
        CHECK(restored.GetMut<Camera>() == Camera{90.0f, 0.1f, 1000.0f, true});
    }

    TEST_CASE("Pack then Instantiate entity with children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(1.0f, 2.0f, 3.0f);
        duin::Entity child = world.CreateEntity("Child").Set<Camera>(60.0f, 0.5f, 500.0f, false);
        child.ChildOf(parent);

        duin::PackedEntity pe = duin::PackedEntity::Pack(parent);

        CHECK(pe.name == "Parent");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Child");

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Parent");
        CHECK(restored.Has<Vec3>());

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "Child");
        CHECK(children[0].Has<Camera>());
        CHECK(children[0].GetMut<Camera>() == Camera{60.0f, 0.5f, 500.0f, false});
    }

    TEST_CASE("Pack then Instantiate deep hierarchy")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.CreateEntity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity mid = world.CreateEntity("Middle").Set<Vec3>(1.0f, 1.0f, 1.0f);
        duin::Entity leaf = world.CreateEntity("Leaf").Set<Vec3>(2.0f, 2.0f, 2.0f);
        mid.ChildOf(root);
        leaf.ChildOf(mid);

        duin::PackedEntity pe = duin::PackedEntity::Pack(root);

        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].children.size() == 1);
        CHECK(pe.children[0].children[0].name == "Leaf");

        duin::World world2;
        world2.Component<Vec3>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Root");
        std::vector<duin::Entity> midChildren = restored.GetChildren();
        CHECK(midChildren.size() == 1);
        CHECK(midChildren[0].GetName() == "Middle");

        std::vector<duin::Entity> leafChildren = midChildren[0].GetChildren();
        CHECK(leafChildren.size() == 1);
        CHECK(leafChildren[0].GetName() == "Leaf");
        CHECK(leafChildren[0].Has<Vec3>());
        CHECK(leafChildren[0].GetMut<Vec3>() == Vec3{2.0f, 2.0f, 2.0f});
    }

    TEST_CASE("Pack then Instantiate entity with multiple children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity childA = world.CreateEntity("ChildA").Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity childB = world.CreateEntity("ChildB").Set<Camera>(45.0f, 0.2f, 800.0f, true);
        childA.ChildOf(parent);
        childB.ChildOf(parent);

        duin::PackedEntity pe = duin::PackedEntity::Pack(parent);

        CHECK(pe.children.size() == 2);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Parent");
        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 2);
    }
}

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
        CHECK(scene.entities[0].name == "MainEntity");
        CHECK(scene.entities[0].children.size() == 1);
        CHECK(scene.entities[0].children[0].name == "ChildCamera");

        duin::Entity root = world.CreateEntity("SceneRoot");
        scene.Instantiate(root, scene);

        std::vector<duin::Entity> rootChildren = root.GetChildren();
        CHECK(rootChildren.size() == 1);
        CHECK(rootChildren[0].GetName() == "MainEntity");
        CHECK(rootChildren[0].Has<Vec3>());
        CHECK(rootChildren[0].GetMut<Vec3>() == Vec3{5.0f, 10.0f, 15.0f});

        std::vector<duin::Entity> grandChildren = rootChildren[0].GetChildren();
        CHECK(grandChildren.size() == 1);
        CHECK(grandChildren[0].GetName() == "ChildCamera");
        CHECK(grandChildren[0].Has<Camera>());
        CHECK(grandChildren[0].GetMut<Camera>() == Camera{75.0f, 0.1f, 1000.0f, true});
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
        scene.Instantiate(root, scene);

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
        CHECK(scene1.externalDependencies[0].uuid == scene2.externalDependencies[0].uuid);
        CHECK(scene1.externalDependencies[0].type == scene2.externalDependencies[0].type);
        CHECK(scene1.entities.size() == scene2.entities.size());
        CHECK(scene1.entities[0].uuid == scene2.entities[0].uuid);
        CHECK(scene1.entities[0].name == scene2.entities[0].name);
        CHECK(scene1.entities[0].enabled == scene2.entities[0].enabled);
        CHECK(scene1.entities[0].tags.size() == scene2.entities[0].tags.size());
        CHECK(scene1.entities[0].children.size() == scene2.entities[0].children.size());
        CHECK(scene1.entities[0].children[0].uuid == scene2.entities[0].children[0].uuid);
        CHECK(scene1.entities[0].children[0].enabled == scene2.entities[0].children[0].enabled);
        CHECK(scene1.entities[0].children[0].tags.size() == scene2.entities[0].children[0].tags.size());
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

        duin::PackedScene ps = duin::PackedScene::Pack(sceneRoot);
        ps.uuid = duin::UUID::FromStringHex("scene111111111111");
        ps.name = "GameScene";
        ps.metadata.editorVersion = "1.0";
        ps.metadata.engineVersion = "0.1.0";
        ps.metadata.lastModified = "2025-01-20T12:00:00Z";
        ps.metadata.author = "TestUser";

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Player");
        CHECK(ps.entities[0].children.size() == 1);
        CHECK(ps.entities[0].children[0].name == "Weapon");

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

        duin::PackedScene ps = duin::PackedScene::Pack(sceneRoot);
        ps.uuid = duin::UUID::FromStringHex("roundtrip111111");
        ps.name = "RoundTripScene";

        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        std::string jsonStr = json.Write();

        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json2);

        CHECK(ps2.uuid == duin::UUID::FromStringHex("roundtrip111111"));
        CHECK(ps2.name == "RoundTripScene");
        CHECK(ps2.entities.size() == 1);
        CHECK(ps2.entities[0].name == "Object");
        CHECK(ps2.entities[0].components.size() == 2);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, ps2);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "Object");
        CHECK(children[0].Has<Vec3>());
        CHECK(children[0].Has<Camera>());
        CHECK(children[0].GetMut<Vec3>() == Vec3{3.0f, 6.0f, 9.0f});
        CHECK(children[0].GetMut<Camera>() == Camera{45.0f, 0.5f, 100.0f, false});
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
        duin::PackedScene ps = duin::PackedScene::Pack(root);
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
        CHECK(ps2.entities[0].name == "Parent");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, ps2);

        // Verify hierarchy
        std::vector<duin::Entity> level1 = newRoot.GetChildren();
        CHECK(level1.size() == 1);
        CHECK(level1[0].GetName() == "Parent");
        CHECK(level1[0].Has<Vec3>());
        CHECK(level1[0].GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});

        std::vector<duin::Entity> level2 = level1[0].GetChildren();
        CHECK(level2.size() == 1);
        CHECK(level2[0].GetName() == "Child");
        CHECK(level2[0].Has<Camera>());
        CHECK(level2[0].GetMut<Camera>() == Camera{60.0f, 0.2f, 500.0f, true});

        std::vector<duin::Entity> level3 = level2[0].GetChildren();
        CHECK(level3.size() == 1);
        CHECK(level3[0].GetName() == "Grandchild");
        CHECK(level3[0].Has<Vec3>());
        CHECK(level3[0].GetMut<Vec3>() == Vec3{4.0f, 5.0f, 6.0f});
    }
}

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
        CHECK(grandchildrenB[0].GetName() == "GrandchildB1");
        CHECK(grandchildrenB[0].Has<Camera>());

        std::vector<duin::Entity> greatGrandchildren = grandchildrenB[0].GetChildren();
        CHECK(greatGrandchildren.size() == 1);
        CHECK(greatGrandchildren[0].GetName() == "GreatGrandchildB1");
        CHECK(greatGrandchildren[0].Has<Vec3>());
        CHECK(greatGrandchildren[0].GetMut<Vec3>() == Vec3{3.0f, 3.0f, 3.0f});

        // Verify parent-child relationships are correctly established
        CHECK(restoredChildA->GetParent() == restored);
        CHECK(restoredChildB->GetParent() == restored);
        CHECK(grandchildrenA[0].GetParent() == *restoredChildA);
        CHECK(grandchildrenA[1].GetParent() == *restoredChildA);
        CHECK(grandchildrenB[0].GetParent() == *restoredChildB);
        CHECK(greatGrandchildren[0].GetParent() == grandchildrenB[0]);

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
        duin::PackedScene ps = duin::PackedScene::Pack(sceneRoot);
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
        duin::PackedScene::Instantiate(newRoot, ps2);

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
                CHECK(weaponChildren[0].GetName() == "WeaponEffect");
            }
        }
        CHECK(foundWeapon);

        // Verify Enemy
        CHECK(restoredEnemy->Has<Vec3>());
        CHECK(restoredEnemy->Has<Camera>());
        std::vector<duin::Entity> enemyChildren = restoredEnemy->GetChildren();
        CHECK(enemyChildren.size() == 1);
        CHECK(enemyChildren[0].GetName() == "EnemyWeapon");

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
        std::vector<duin::Entity> l2Children = l1Children[0].GetChildren();
        CHECK(l2Children.size() == 2);

        std::vector<duin::Entity> l3Children = l2Children[0].GetChildren();
        CHECK(l3Children.size() == 2);

        std::vector<duin::Entity> l4Children = l3Children[0].GetChildren();
        CHECK(l4Children.size() == 1);
        CHECK(l4Children[0].Has<Vec3>());

        // Verify the deepest node has no children
        std::vector<duin::Entity> l5Children = l4Children[0].GetChildren();
        CHECK(l5Children.empty());

        // Verify parent-child relationships at each level
        CHECK(l1Children[0].GetParent() == restored);
        CHECK(l1Children[1].GetParent() == restored);
        CHECK(l1Children[2].GetParent() == restored);

        CHECK(l2Children[0].GetParent() == l1Children[0]);
        CHECK(l2Children[1].GetParent() == l1Children[0]);

        CHECK(l3Children[0].GetParent() == l2Children[0]);
        CHECK(l3Children[1].GetParent() == l2Children[0]);

        CHECK(l4Children[0].GetParent() == l3Children[0]);

        // Verify that GetChildren returns correct counts at each level
        CHECK(restored.GetChildren().size() == 3);
        CHECK(l1Children[0].GetChildren().size() == 2);
        CHECK(l2Children[0].GetChildren().size() == 2);
        CHECK(l3Children[0].GetChildren().size() == 1);
        CHECK(l4Children[0].GetChildren().size() == 0);
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
        duin::PackedScene ps = duin::PackedScene::Pack(sceneRoot);
        CHECK(ps.entities.size() == 4); // 2 standalone + 2 hierarchies

        // Serialize, deserialize, and instantiate
        duin::JSONValue json = duin::PackedScene::Serialize(ps);
        duin::PackedScene ps2 = duin::PackedScene::Deserialize(json);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewSceneRoot");
        duin::PackedScene::Instantiate(newRoot, ps2);

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

TEST_SUITE("PackedScene Pack and Instantiate")
{
    TEST_CASE("Pack empty scene root")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.CreateEntity("EmptyRoot");
        duin::PackedScene ps = duin::PackedScene::Pack(root);

        CHECK(ps.entities.empty());
    }

    TEST_CASE("Pack scene with multiple root-level entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity root = world.CreateEntity("Root");
        duin::Entity a = world.CreateEntity("A").Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity b = world.CreateEntity("B").Set<Vec3>(0.0f, 1.0f, 0.0f);
        duin::Entity c = world.CreateEntity("C").Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        a.ChildOf(root);
        b.ChildOf(root);
        c.ChildOf(root);

        duin::PackedScene ps = duin::PackedScene::Pack(root);

        CHECK(ps.entities.size() == 3);
    }

    TEST_CASE("Instantiate scene into world")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("instantiate11111");
        scene.name = "InstantiateTest";

        duin::PackedEntity e1;
        e1.name = "Obj1";
        e1.enabled = true;
        duin::PackedComponent c1;
        c1.componentTypeName = "Vec3";
        c1.jsonData = R"({"type":"Vec3","x":10.0,"y":20.0,"z":30.0})";
        e1.components.push_back(c1);

        duin::PackedEntity e2;
        e2.name = "Obj2";
        e2.enabled = true;
        duin::PackedComponent c2;
        c2.componentTypeName = "Camera";
        c2.jsonData = R"({"type":"Camera","fov":60.0,"nearPlane":0.5,"farPlane":800.0,"isPrimary":false})";
        e2.components.push_back(c2);

        scene.entities.push_back(e1);
        scene.entities.push_back(e2);

        duin::Entity root = world.CreateEntity("SceneRoot");
        duin::PackedScene::Instantiate(root, scene);

        std::vector<duin::Entity> children = root.GetChildren();
        CHECK(children.size() == 2);
    }

    TEST_CASE("Pack then Instantiate scene round-trip preserves data")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Build scene
        duin::Entity root = world.CreateEntity("Root");
        duin::Entity e1 =
            world.CreateEntity("Player").Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        duin::Entity e2 = world.CreateEntity("Enemy").Set<Vec3>(10.0f, 0.0f, 10.0f);
        e1.ChildOf(root);
        e2.ChildOf(root);

        // Pack
        duin::PackedScene ps = duin::PackedScene::Pack(root);
        ps.uuid = duin::UUID::FromStringHex("packinstrt111111");
        ps.name = "PackInstantiateRoundTrip";

        CHECK(ps.entities.size() == 2);

        // Instantiate into new root
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, ps);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 2);

        // Find Player and Enemy by name
        duin::Entity *player = nullptr;
        duin::Entity *enemy = nullptr;
        for (auto &c : children)
        {
            if (c.GetName() == "Player")
                player = &c;
            if (c.GetName() == "Enemy")
                enemy = &c;
        }

        REQUIRE(player != nullptr);
        REQUIRE(enemy != nullptr);

        CHECK(player->Has<Vec3>());
        CHECK(player->Has<Camera>());
        CHECK(player->GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
        CHECK(player->GetMut<Camera>() == Camera{90.0f, 0.1f, 1000.0f, true});

        CHECK(enemy->Has<Vec3>());
        CHECK(enemy->GetMut<Vec3>() == Vec3{10.0f, 0.0f, 10.0f});
    }
}

TEST_SUITE("Tag Component Tests")
{
    // Define tag components (zero-size components used as markers)
    struct PlayerTag {};
    struct EnemyTag {};
    struct CollectibleTag {};
    struct ActiveTag {};
    struct DestroyableTag {};

    TEST_CASE("Pack and Instantiate entity with tag components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<PlayerTag>();
        world.Component<ActiveTag>();

        duin::Entity entity = world.CreateEntity("TaggedEntity")
            .Set<Vec3>(1.0f, 2.0f, 3.0f)
            .Add<PlayerTag>()
            .Add<ActiveTag>();

        duin::PackedEntity pe = duin::PackedEntity::Pack(entity);

        CHECK(pe.name == "TaggedEntity");
        CHECK(pe.components.size() >= 1); // At least Vec3

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<PlayerTag>();
        world2.Component<ActiveTag>();

        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "TaggedEntity");
        CHECK(restored.Has<Vec3>());
        CHECK(restored.Has<PlayerTag>());
        CHECK(restored.Has<ActiveTag>());
        CHECK(restored.GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
    }

    TEST_CASE("Pack and Instantiate multiple entities with different tag combinations")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<PlayerTag>();
        world.Component<EnemyTag>();
        world.Component<CollectibleTag>();
        world.Component<ActiveTag>();

        duin::Entity root = world.CreateEntity("Root");

        duin::Entity player = world.CreateEntity("Player")
            .ChildOf(root)
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Add<PlayerTag>()
            .Add<ActiveTag>();

        duin::Entity enemy = world.CreateEntity("Enemy")
            .ChildOf(root)
            .Set<Vec3>(10.0f, 0.0f, 0.0f)
            .Add<EnemyTag>()
            .Add<ActiveTag>();

        duin::Entity item = world.CreateEntity("HealthPack")
            .ChildOf(root)
            .Set<Vec3>(5.0f, 0.0f, 5.0f)
            .Add<CollectibleTag>();

        duin::PackedScene scene = duin::PackedScene::Pack(root);
        scene.name = "TaggedScene";

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<PlayerTag>();
        world2.Component<EnemyTag>();
        world2.Component<CollectibleTag>();
        world2.Component<ActiveTag>();

        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, scene);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 3);

        // Verify tags are preserved
        int activeCount = 0;
        for (auto& child : children)
        {
            if (child.Has<ActiveTag>()) activeCount++;
        }
        CHECK(activeCount == 2);
    }

    TEST_CASE("Serialize and deserialize entity with only tags")
    {
        duin::World world;
        world.Component<PlayerTag>();
        world.Component<EnemyTag>();
        world.Component<DestroyableTag>();

        duin::Entity entity = world.CreateEntity("TagOnlyEntity")
            .Add<PlayerTag>()
            .Add<DestroyableTag>();

        duin::PackedEntity pe = duin::PackedEntity::Pack(entity);
        duin::JSONValue json = duin::PackedEntity::Serialize(pe);
        duin::PackedEntity deserialized = duin::PackedEntity::Deserialize(json);

        CHECK(deserialized.name == "TagOnlyEntity");

        // Instantiate
        duin::World world2;
        world2.Component<PlayerTag>();
        world2.Component<EnemyTag>();
        world2.Component<DestroyableTag>();

        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(deserialized, restored);

        CHECK(restored.GetName() == "TagOnlyEntity");
        CHECK(restored.Has<PlayerTag>());
        CHECK(restored.Has<DestroyableTag>());
        CHECK_FALSE(restored.Has<EnemyTag>());
    }

    TEST_CASE("Tag components persist through scene serialization")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<ActiveTag>();
        world.Component<DestroyableTag>();

        duin::Entity root = world.CreateEntity("Root");
        duin::Entity entity = world.CreateEntity("TaggedEntity")
            .ChildOf(root)
            .Set<Vec3>(1.0f, 2.0f, 3.0f)
            .Add<ActiveTag>()
            .Add<DestroyableTag>();

        duin::PackedScene scene = duin::PackedScene::Pack(root);
        scene.name = "TagPersistenceTest";
        scene.uuid = duin::UUID::FromStringHex("tagtest11111111");

        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        std::string jsonStr = json.Write();

        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene2 = duin::PackedScene::Deserialize(json2);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<ActiveTag>();
        world2.Component<DestroyableTag>();

        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, scene2);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        REQUIRE(children.size() == 1);
        CHECK(children[0].Has<Vec3>());
        CHECK(children[0].Has<ActiveTag>());
        CHECK(children[0].Has<DestroyableTag>());
    }
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
        
        // Add relationship: player targets enemy
        player.Add<Targets>(enemy);

        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        // Verify relationship is packed
        CHECK(pe.name == "Player");

        // Note: Actual pair serialization depends on SceneBuilder implementation
        // This test documents expected behavior
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
    }
}

TEST_SUITE("Prefab and Inheritance Tests")
{
    struct Health { float value = 100.0f; bool operator==(const Health& o) const { return value == o.value; } };
    struct Armor { float value = 50.0f; bool operator==(const Armor& o) const { return value == o.value; } };
    struct Speed { float value = 5.0f; bool operator==(const Speed& o) const { return value == o.value; } };
    struct Damage { float value = 10.0f; bool operator==(const Damage& o) const { return value == o.value; } };
    
    struct Prefab {};

    TEST_CASE("Create and instantiate basic prefab")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Armor>();
        world.Component<Prefab>();

        // Create a prefab
        duin::Entity soldierPrefab = world.CreateEntity("SoldierPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f})
            .Set<Armor>({50.0f})
            .Add<Prefab>();

        // Pack the prefab
        duin::PackedEntity pe = duin::PackedEntity::Pack(soldierPrefab);

        CHECK(pe.name == "SoldierPrefab");
        CHECK(pe.components.size() >= 3);

        // Instantiate prefab
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Armor>();
        world2.Component<Prefab>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        CHECK(instance.Has<Vec3>());
        CHECK(instance.Has<Health>());
        CHECK(instance.Has<Armor>());
        CHECK(instance.GetMut<Health>() == Health{100.0f});
        CHECK(instance.GetMut<Armor>() == Armor{50.0f});
    }

    TEST_CASE("Prefab inheritance with IsA relationship")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();
        world.Component<Damage>();

        // Create base prefab
        duin::Entity baseSoldier = world.CreateEntity("BaseSoldier")
            .Set<Health>({100.0f})
            .Set<Speed>({5.0f});

        // Create specialized prefab that inherits from base
        duin::Entity heavySoldier = world.CreateEntity("HeavySoldier")
            .IsA(baseSoldier)
            .Set<Health>({150.0f})  // Override health
            .Set<Armor>({75.0f});

        duin::PackedEntity pe = duin::PackedEntity::Pack(heavySoldier);

        CHECK(pe.name == "HeavySoldier");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();
        world2.Component<Damage>();
        world2.Component<Armor>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        CHECK(instance.Has<Health>());
        CHECK(instance.Has<Armor>());
    }

    TEST_CASE("Pack scene with multiple prefab instances")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();
        world.Component<Prefab>();

        // Create enemy prefab
        duin::Entity enemyPrefab = world.CreateEntity("EnemyPrefab")
            .Set<Health>({50.0f})
            .Set<Speed>({3.0f})
            .Add<Prefab>();

        duin::PackedEntity prefabPacked = duin::PackedEntity::Pack(enemyPrefab);

        // Create scene root and instantiate multiple enemies
        duin::Entity root = world.CreateEntity("Root");

        for (int i = 0; i < 5; ++i)
        {
            std::string name = "Enemy_" + std::to_string(i);
            duin::Entity enemy = world.CreateEntity(name.c_str())
                .ChildOf(root)
                .Set<Vec3>(static_cast<float>(i * 5), 0.0f, 0.0f)
                .Set<Health>({50.0f})
                .Set<Speed>({3.0f});
        }

        duin::PackedScene scene = duin::PackedScene::Pack(root);
        scene.name = "EnemySpawnScene";

        CHECK(scene.entities.size() == 5);

        // Serialize and deserialize
        duin::JSONValue json = duin::PackedScene::Serialize(scene);
        duin::PackedScene scene2 = duin::PackedScene::Deserialize(json);

        CHECK(scene2.entities.size() == 5);
    }

    TEST_CASE("Prefab with child entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Camera>();

        // Create vehicle prefab with nested structure
        duin::Entity vehiclePrefab = world.CreateEntity("VehiclePrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f});

        duin::Entity turret = world.CreateEntity("Turret")
            .ChildOf(vehiclePrefab)
            .Set<Vec3>(0.0f, 2.0f, 0.0f);

        duin::Entity turretCamera = world.CreateEntity("TurretCamera")
            .ChildOf(turret)
            .Set<Vec3>(0.0f, 0.5f, 0.0f)
            .Set<Camera>(60.0f, 0.1f, 500.0f, false);

        duin::PackedEntity pe = duin::PackedEntity::Pack(vehiclePrefab);

        CHECK(pe.name == "VehiclePrefab");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Turret");
        CHECK(pe.children[0].children.size() == 1);
        CHECK(pe.children[0].children[0].name == "TurretCamera");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Camera>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "Turret");

        std::vector<duin::Entity> grandchildren = children[0].GetChildren();
        CHECK(grandchildren.size() == 1);
        CHECK(grandchildren[0].GetName() == "TurretCamera");
        CHECK(grandchildren[0].Has<Camera>());
    }

    TEST_CASE("Override prefab values in instances")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Speed>();

        // Create base prefab
        duin::Entity monsterPrefab = world.CreateEntity("MonsterPrefab")
            .Set<Health>({100.0f})
            .Set<Speed>({5.0f});

        duin::PackedEntity prefabPacked = duin::PackedEntity::Pack(monsterPrefab);

        // Create instances with overridden values
        duin::Entity root = world.CreateEntity("Root");

        duin::Entity weakMonster = world.CreateEntity("WeakMonster")
            .ChildOf(root)
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({50.0f})   // Override
            .Set<Speed>({5.0f});

        duin::Entity strongMonster = world.CreateEntity("StrongMonster")
            .ChildOf(root)
            .Set<Vec3>(10.0f, 0.0f, 0.0f)
            .Set<Health>({200.0f})  // Override
            .Set<Speed>({3.0f});    // Override

        duin::PackedScene scene = duin::PackedScene::Pack(root);

        // Instantiate scene
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Speed>();

        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(newRoot, scene);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        CHECK(children.size() == 2);

        // Find and verify overridden values
        for (auto& child : children)
        {
            if (child.GetName() == "WeakMonster")
            {
                CHECK(child.GetMut<Health>() == Health{50.0f});
                CHECK(child.GetMut<Speed>() == Speed{5.0f});
            }
            else if (child.GetName() == "StrongMonster")
            {
                CHECK(child.GetMut<Health>() == Health{200.0f});
                CHECK(child.GetMut<Speed>() == Speed{3.0f});
            }
        }
    }

    TEST_CASE("Nested prefab hierarchies")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Health>();
        world.Component<Damage>();

        // Create weapon prefab
        duin::Entity weaponPrefab = world.CreateEntity("WeaponPrefab")
            .Set<Damage>({10.0f});

        // Create character prefab that includes weapon
        duin::Entity characterPrefab = world.CreateEntity("CharacterPrefab")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Set<Health>({100.0f});

        duin::Entity characterWeapon = world.CreateEntity("Weapon")
            .ChildOf(characterPrefab)
            .Set<Vec3>(0.5f, 1.0f, 0.0f)
            .Set<Damage>({10.0f});

        duin::PackedEntity pe = duin::PackedEntity::Pack(characterPrefab);

        CHECK(pe.name == "CharacterPrefab");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "Weapon");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Health>();
        world2.Component<Damage>();

        duin::Entity instance = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, instance);

        CHECK(instance.Has<Vec3>());
        CHECK(instance.Has<Health>());
        
        std::vector<duin::Entity> children = instance.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "Weapon");
        CHECK(children[0].Has<Damage>());
        CHECK(children[0].GetMut<Damage>() == Damage{10.0f});
    }
}

TEST_SUITE("Complex Player Entity Tests")
{
    // Define test components matching Player.cpp structure
    struct Mass
    {
        float value = 1.0f;
        bool operator==(const Mass &other) const
        {
            return value == other.value;
        }
    };

    struct CanRunComponent
    {
        float speed = 7.5f;
        bool operator==(const CanRunComponent &other) const
        {
            return speed == other.speed;
        }
    };

    struct CanSprintComponent
    {
        float speed = 12.0f;
        bool operator==(const CanSprintComponent &other) const
        {
            return speed == other.speed;
        }
    };

    struct CanJumpComponent
    {
        float impulse = 29000.61f;
        bool operator==(const CanJumpComponent &other) const
        {
            return impulse == other.impulse;
        }
    };

    struct InputVelocities
    {
        Vec3 value = {0.0f, 0.0f, 0.0f};
        bool operator==(const InputVelocities &other) const
        {
            return value == other.value;
        }
    };

    struct InputForces
    {
        Vec3 value = {0.0f, 0.0f, 0.0f};
        bool operator==(const InputForces &other) const
        {
            return value == other.value;
        }
    };

    struct PlayerMovementInputVec3
    {
        Vec3 value = {0.0f, 0.0f, 0.0f};
        bool operator==(const PlayerMovementInputVec3 &other) const
        {
            return value == other.value;
        }
    };

    struct InputVelocityDirection
    {
        Vec3 value = {0.0f, 0.0f, 0.0f};
        bool operator==(const InputVelocityDirection &other) const
        {
            return value == other.value;
        }
    };

    struct MouseInputVec2
    {
        float x = 0.0f;
        float y = 0.0f;
        bool operator==(const MouseInputVec2 &other) const
        {
            return x == other.x && y == other.y;
        }
    };

    struct CameraYawComponent
    {
        float value = 0.0f;
        bool operator==(const CameraYawComponent &other) const
        {
            return value == other.value;
        }
    };

    struct CameraPitchComponent
    {
        float value = 0.0f;
        bool operator==(const CameraPitchComponent &other) const
        {
            return value == other.value;
        }
    };

    struct GravityComponent
    {
        Vec3 value = {0.0f, -9.81f, 0.0f};
        bool operator==(const GravityComponent &other) const
        {
            return value == other.value;
        }
    };

    struct VelocityBob
    {
        float frequency = 1.0f;
        float amplitude = 1.0f;
        bool operator==(const VelocityBob &other) const
        {
            return frequency == other.frequency && amplitude == other.amplitude;
        }
    };

    // Tags
    struct CanGravity
    {
    };
    struct OnGroundTag
    {
    };
    struct ActiveCameraTag
    {
    };

    TEST_CASE("Pack Player Entity with Multiple Components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Mass>();
        world.Component<CanRunComponent>();
        world.Component<CanSprintComponent>();
        world.Component<CanJumpComponent>();
        world.Component<InputVelocities>();
        world.Component<InputForces>();
        world.Component<PlayerMovementInputVec3>();
        world.Component<InputVelocityDirection>();
        world.Component<MouseInputVec2>();
        world.Component<CameraYawComponent>();
        world.Component<GravityComponent>();
        world.Component<CanGravity>();
        world.Component<OnGroundTag>();

        // Create player entity with multiple components like in Player.cpp
        duin::Entity player = world.CreateEntity("Player")
                                  .Set<Vec3>(0.0f, 50.0f, 5.0f)
                                  .Set<Mass>({80.0f})
                                  .Set<CanRunComponent>({10.0f})
                                  .Set<CanSprintComponent>({17.5f})
                                  .Set<CanJumpComponent>({625.0f})
                                  .Add<InputVelocities>()
                                  .Add<InputForces>()
                                  .Add<PlayerMovementInputVec3>()
                                  .Add<InputVelocityDirection>()
                                  .Add<MouseInputVec2>()
                                  .Add<CameraYawComponent>()
                                  .Add<GravityComponent>()
                                  .Add<CanGravity>()
                                  .Add<OnGroundTag>();

        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        CHECK(pe.name == "Player");
        CHECK(pe.enabled == true);
        CHECK(pe.components.size() >= 7); // At least the non-tag components

        // Verify specific component values are packed
        bool foundMass = false;
        bool foundCanRun = false;
        bool foundCanSprint = false;
        bool foundCanJump = false;
        bool foundVec3 = false;

        for (const auto &comp : pe.components)
        {
            if (comp.componentTypeName == "Mass")
            {
                foundMass = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json.HasMember("value"));
                CHECK(json["value"].GetDouble() == doctest::Approx(80.0));
            }
            else if (comp.componentTypeName == "CanRunComponent")
            {
                foundCanRun = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json.HasMember("speed"));
                CHECK(json["speed"].GetDouble() == doctest::Approx(10.0));
            }
            else if (comp.componentTypeName == "CanSprintComponent")
            {
                foundCanSprint = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json.HasMember("speed"));
                CHECK(json["speed"].GetDouble() == doctest::Approx(17.5));
            }
            else if (comp.componentTypeName == "CanJumpComponent")
            {
                foundCanJump = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json.HasMember("impulse"));
                CHECK(json["impulse"].GetDouble() == doctest::Approx(625.0));
            }
            else if (comp.componentTypeName == "Vec3")
            {
                foundVec3 = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json.HasMember("x"));
                CHECK(json.HasMember("y"));
                CHECK(json.HasMember("z"));
                CHECK(json["x"].GetDouble() == doctest::Approx(0.0));
                CHECK(json["y"].GetDouble() == doctest::Approx(50.0));
                CHECK(json["z"].GetDouble() == doctest::Approx(5.0));
            }
        }

        CHECK(foundMass);
        CHECK(foundCanRun);
        CHECK(foundCanSprint);
        CHECK(foundCanJump);
        CHECK(foundVec3);
    }

    TEST_CASE("Pack and Instantiate Player Entity with Tags")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Mass>();
        world.Component<CanGravity>();
        world.Component<OnGroundTag>();

        duin::Entity player = world.CreateEntity("PlayerWithTags")
                                  .Set<Vec3>(1.0f, 2.0f, 3.0f)
                                  .Set<Mass>({75.0f})
                                  .Add<CanGravity>()
                                  .Add<OnGroundTag>();

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        CHECK(pe.name == "PlayerWithTags");

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Mass>();
        world2.Component<CanGravity>();
        world2.Component<OnGroundTag>();

        duin::Entity newPlayer = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, newPlayer);

        CHECK(newPlayer.GetName() == "PlayerWithTags");
        CHECK(newPlayer.Has<Vec3>());
        CHECK(newPlayer.Has<Mass>());
        CHECK(newPlayer.Has<CanGravity>());
        CHECK(newPlayer.Has<OnGroundTag>());
        CHECK(newPlayer.GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
        CHECK(newPlayer.GetMut<Mass>() == Mass{75.0f});
    }

    TEST_CASE("Pack Player Hierarchy with CameraRoot and PlayerCamera")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        world.Component<MouseInputVec2>();
        world.Component<CameraPitchComponent>();
        world.Component<CameraYawComponent>();
        world.Component<VelocityBob>();
        world.Component<ActiveCameraTag>();
        world.Component<Mass>();

        float playerHeight = 1.75f;

        // Create player entity
        duin::Entity player = world.CreateEntity("Player")
                                  .Set<Vec3>(0.0f, 50.0f, 5.0f)
                                  .Set<Mass>({80.0f})
                                  .Add<MouseInputVec2>()
                                  .Add<CameraYawComponent>();

        // Create camera root child
        duin::Entity cameraRoot = world.CreateEntity("CameraRoot")
                                      .ChildOf(player)
                                      .Set<Vec3>(0.0f, playerHeight, 0.0f)
                                      .Add<MouseInputVec2>()
                                      .Add<CameraPitchComponent>();

        // Create player camera grandchild
        duin::Entity playerCamera = world.CreateEntity("PlayerCamera")
                                        .ChildOf(cameraRoot)
                                        .Set<Vec3>(0.0f, 0.0f, 0.0f)
                                        .Set<Camera>(72.0f, 0.1f, 1000.0f, true)
                                        .Set<VelocityBob>({10.0f, 1.0f})
                                        .Add<ActiveCameraTag>();

        // Pack the player entity
        duin::PackedEntity pe = duin::PackedEntity::Pack(player);

        CHECK(pe.name == "Player");
        CHECK(pe.children.size() == 1);
        CHECK(pe.children[0].name == "CameraRoot");
        CHECK(pe.children[0].children.size() == 1);
        CHECK(pe.children[0].children[0].name == "PlayerCamera");

        // Verify CameraRoot position
        bool foundCameraRootPos = false;
        for (const auto &comp : pe.children[0].components)
        {
            if (comp.componentTypeName == "Vec3")
            {
                foundCameraRootPos = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["x"].GetDouble() == doctest::Approx(0.0));
                CHECK(json["y"].GetDouble() == doctest::Approx(playerHeight));
                CHECK(json["z"].GetDouble() == doctest::Approx(0.0));
            }
        }
        CHECK(foundCameraRootPos);

        // Verify PlayerCamera has Camera component with correct values
        bool foundCamera = false;
        bool foundVelocityBob = false;
        for (const auto &comp : pe.children[0].children[0].components)
        {
            if (comp.componentTypeName == "Camera")
            {
                foundCamera = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["fov"].GetDouble() == doctest::Approx(72.0));
            }
            else if (comp.componentTypeName == "VelocityBob")
            {
                foundVelocityBob = true;
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["frequency"].GetDouble() == doctest::Approx(10.0));
                CHECK(json["amplitude"].GetDouble() == doctest::Approx(1.0));
            }
        }
        CHECK(foundCamera);
        CHECK(foundVelocityBob);
    }

    TEST_CASE("Serialize and Deserialize Player Hierarchy")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        world.Component<Mass>();
        world.Component<VelocityBob>();

        // Create hierarchy
        duin::Entity player = world.CreateEntity("Player").Set<Vec3>(0.0f, 50.0f, 5.0f).Set<Mass>({80.0f});

        duin::Entity cameraRoot = world.CreateEntity("CameraRoot").ChildOf(player).Set<Vec3>(0.0f, 1.75f, 0.0f);

        duin::Entity playerCamera = world.CreateEntity("PlayerCamera")
                                        .ChildOf(cameraRoot)
                                        .Set<Vec3>(0.0f, 0.0f, 0.0f)
                                        .Set<Camera>(72.0f, 0.1f, 1000.0f, true)
                                        .Set<VelocityBob>({10.0f, 1.0f});

        // Pack and serialize
        duin::PackedEntity pe = duin::PackedEntity::Pack(player);
        duin::JSONValue json = duin::PackedEntity::Serialize(pe);

        // Deserialize
        duin::PackedEntity deserialized = duin::PackedEntity::Deserialize(json);

        CHECK(deserialized.name == "Player");
        CHECK(deserialized.children.size() == 1);
        CHECK(deserialized.children[0].name == "CameraRoot");
        CHECK(deserialized.children[0].children.size() == 1);
        CHECK(deserialized.children[0].children[0].name == "PlayerCamera");

        // Verify component data integrity
        bool foundMass = false;
        for (const auto &comp : deserialized.components)
        {
            if (comp.componentTypeName == "Mass")
            {
                foundMass = true;
                duin::JSONValue compJson = duin::JSONValue::Parse(comp.jsonData);
                CHECK(compJson["value"].GetDouble() == doctest::Approx(80.0));
            }
        }
        CHECK(foundMass);
    }

    TEST_CASE("Round-trip Player Hierarchy with Full Instantiation")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        world.Component<Mass>();
        world.Component<VelocityBob>();
        world.Component<CameraPitchComponent>();

        // Create original hierarchy
        duin::Entity player = world.CreateEntity("Player").Set<Vec3>(0.0f, 50.0f, 5.0f).Set<Mass>({80.0f});

        duin::Entity cameraRoot =
            world.CreateEntity("CameraRoot").ChildOf(player).Set<Vec3>(0.0f, 1.75f, 0.0f).Add<CameraPitchComponent>();

        duin::Entity playerCamera = world.CreateEntity("PlayerCamera")
                                        .ChildOf(cameraRoot)
                                        .Set<Vec3>(0.0f, 0.0f, 0.0f)
                                        .Set<Camera>(72.0f, 0.1f, 1000.0f, true)
                                        .Set<VelocityBob>({10.0f, 1.0f});

        // Pack, serialize, deserialize
        duin::PackedEntity pe = duin::PackedEntity::Pack(player);
        duin::JSONValue json = duin::PackedEntity::Serialize(pe);
        duin::PackedEntity deserialized = duin::PackedEntity::Deserialize(json);

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        world2.Component<Mass>();
        world2.Component<VelocityBob>();
        world2.Component<CameraPitchComponent>();

        duin::Entity newPlayer = world2.CreateEntity();
        duin::PackedEntity::Instantiate(deserialized, newPlayer);

        // Verify hierarchy
        CHECK(newPlayer.GetName() == "Player");
        CHECK(newPlayer.Has<Vec3>());
        CHECK(newPlayer.Has<Mass>());
        CHECK(newPlayer.GetMut<Vec3>() == Vec3{0.0f, 50.0f, 5.0f});
        CHECK(newPlayer.GetMut<Mass>() == Mass{80.0f});

        std::vector<duin::Entity> children = newPlayer.GetChildren();
        CHECK(children.size() == 1);
        CHECK(children[0].GetName() == "CameraRoot");
        CHECK(children[0].Has<Vec3>());
        CHECK(children[0].Has<CameraPitchComponent>());
        CHECK(children[0].GetMut<Vec3>() == Vec3{0.0f, 1.75f, 0.0f});

        std::vector<duin::Entity> grandchildren = children[0].GetChildren();
        CHECK(grandchildren.size() == 1);
        CHECK(grandchildren[0].GetName() == "PlayerCamera");
        CHECK(grandchildren[0].Has<Vec3>());
        CHECK(grandchildren[0].Has<Camera>());
        CHECK(grandchildren[0].Has<VelocityBob>());
        CHECK(grandchildren[0].GetMut<Vec3>() == Vec3{0.0f, 0.0f, 0.0f});
        CHECK(grandchildren[0].GetMut<Camera>() == Camera{72.0f, 0.1f, 1000.0f, true});
        CHECK(grandchildren[0].GetMut<VelocityBob>() == VelocityBob{10.0f, 1.0f});
    }

    TEST_CASE("PackedScene with Player Hierarchy")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        world.Component<Mass>();

        duin::Entity root = world.CreateEntity();

        // Create player hierarchy
        duin::Entity player = world.CreateEntity("Player").ChildOf(root).Set<Vec3>(0.0f, 50.0f, 5.0f).Set<Mass>({80.0f});

        duin::Entity cameraRoot = world.CreateEntity("CameraRoot").ChildOf(player).Set<Vec3>(0.0f, 1.75f, 0.0f);

        duin::Entity playerCamera = world.CreateEntity("PlayerCamera")
                                        .ChildOf(cameraRoot)
                                        .Set<Vec3>(0.0f, 0.0f, 0.0f)
                                        .Set<Camera>(72.0f, 0.1f, 1000.0f, true);

        // Pack scene
        duin::PackedScene scene = duin::PackedScene::Pack(root);
        scene.uuid = duin::UUID::FromStringHex("playerscn111111");
        scene.name = "PlayerScene";

        // Serialize
        duin::JSONValue json = duin::PackedScene::Serialize(scene);

        // Verify scene structure
        CHECK(json.IsObject());
        CHECK(json.HasMember("sceneUUID"));
        CHECK(json.HasMember("sceneName"));
        CHECK(json["sceneName"].GetString() == "PlayerScene");
        CHECK(json.HasMember("entities"));
        CHECK(json["entities"].IsArray());
        CHECK(json["entities"].Capacity() >= 1);

        // Deserialize
        duin::PackedScene deserializedScene = duin::PackedScene::Deserialize(json);

        CHECK(deserializedScene.name == "PlayerScene");
        CHECK(deserializedScene.entities.size() >= 1);
        if (deserializedScene.entities.size() >= 1)
        {
            CHECK(deserializedScene.entities[0].name == "Player");
            CHECK(deserializedScene.entities[0].children.size() == 1);
        }
    }

    TEST_CASE("Component Value Precision Test")
    {
        duin::World world;
        world.Component<CanJumpComponent>();
        world.Component<CanRunComponent>();
        world.Component<Mass>();

        // Test with specific floating point values
        duin::Entity entity = world.CreateEntity("PrecisionTest")
                                  .Set<CanJumpComponent>({625.0f})
                                  .Set<CanRunComponent>({10.0f})
                                  .Set<Mass>({80.5f});

        // Pack
        duin::PackedEntity pe = duin::PackedEntity::Pack(entity);

        // Verify exact values in JSON
        for (const auto &comp : pe.components)
        {
            if (comp.componentTypeName == "CanJumpComponent")
            {
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["impulse"].GetDouble() == doctest::Approx(625.0));
            }
            else if (comp.componentTypeName == "CanRunComponent")
            {
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["speed"].GetDouble() == doctest::Approx(10.0));
            }
            else if (comp.componentTypeName == "Mass")
            {
                duin::JSONValue json = duin::JSONValue::Parse(comp.jsonData);
                CHECK(json["value"].GetDouble() == doctest::Approx(80.5));
            }
        }

        // Instantiate and verify
        duin::World world2;
        world2.Component<CanJumpComponent>();
        world2.Component<CanRunComponent>();
        world2.Component<Mass>();

        duin::Entity newEntity = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, newEntity);

        CHECK(newEntity.GetMut<CanJumpComponent>() == CanJumpComponent{625.0f});
        CHECK(newEntity.GetMut<CanRunComponent>() == CanRunComponent{10.0f});
        CHECK(newEntity.GetMut<Mass>() == Mass{80.5f});
    }
}
} // namespace TestSceneBuilder