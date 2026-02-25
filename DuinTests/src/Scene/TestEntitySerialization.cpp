#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>
#include <vector>
#include "Defines.h"

namespace TestSceneBuilder
{

TEST_SUITE("Simple Entity Serialization")
{
    TEST_CASE("Deserialize Entity")
    {
        duin::JSONValue v = duin::JSONValue::Parse(SIMPLE_ENTITY_JSON);

        duin::SceneBuilder builder(nullptr);
        duin::PackedEntity p = builder.DeserializeEntity(v);

        CHECK(p.uuid == duin::UUID::FromStringHex("1a2b3c4d5e6f7890"));
        CHECK(p.name == "Camera");
        CHECK(p.enabled == true);
        CHECK(p.tags.size() == 1);
        if (p.tags.size() >= 1)
        {
            CHECK(p.tags[0].componentTypeName == "camera");
        }
        CHECK(p.components.size() == 2);
        CHECK(p.children.size() == 0);

        if (p.components.size() >= 2)
        {
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
    }

    TEST_CASE("Serialize Entity")
    {
        // Create a PackedEntity with expected values
        duin::PackedEntity p;
        p.uuid = duin::UUID::FromStringHex("1a2b3c4d5e6f7890");
        p.name = "Camera";
        p.enabled = true;

        // Add tag as PackedComponent
        duin::PackedComponent tagComp;
        tagComp.componentTypeName = "camera";
        tagComp.jsonData = R"({"type":"camera"})";
        p.tags.push_back(tagComp);

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
        duin::SceneBuilder builder(nullptr);
        duin::JSONValue json = builder.SerializeEntity(p);

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
        CHECK(json["tags"][0].Write() == R"({"type":"camera"})");
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
            world.Entity("TestCamera").Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(75.0f, 0.1f, 1000.0f, true);

        duin::SceneBuilder builder(&world);
        duin::PackedEntity pe = builder.PackEntity(e);

        CHECK(pe.name == "TestCamera");
        CHECK(pe.enabled == true);
        CHECK(pe.components.size() == 2);

        if (pe.components.size() >= 2)
        {
            int vecIdx = pe.components[0].componentTypeName == "Vec3" ? 0 : 1;
            int camIdx = pe.components[1].componentTypeName == "Camera" ? 1 : 0;

            CHECK(pe.components[vecIdx].componentTypeName == "Vec3");
            CHECK(pe.components[camIdx].componentTypeName == "Camera");
        }
    }

    TEST_CASE("Pack Entity with children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity child = world.Entity("Child").Set<Camera>(60.0f, 0.1f, 500.0f, false);
        child.ChildOf(parent);

        duin::SceneBuilder builder(&world);
        duin::PackedEntity pe = builder.PackEntity(parent);

        CHECK(pe.name == "Parent");
        CHECK(pe.components.size() >= 1);
        CHECK(pe.children.size() == 1);
        if (pe.children.size() >= 1)
        {
            CHECK(pe.children[0].name == "Child");
            CHECK(pe.children[0].components.size() >= 1);
        }
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

        duin::Entity e = world.Entity();
        duin::SceneBuilder builder(&world);
        builder.InstantiateEntity(pe, e);

        MSG_CHECK(e.GetName(), e.GetName() == "InstantiatedEntity");
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

        duin::Entity e = world.Entity();
        duin::SceneBuilder builder(&world);
        builder.InstantiateEntity(pe, e);

        CHECK(e.GetName().starts_with("Parent"));
        CHECK(e.Has<Vec3>());

        std::vector<duin::Entity> children = e.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].GetName() == "Child");
            CHECK(children[0].Has<Camera>());
            CHECK(children[0].GetMut<Camera>() == Camera{60.0f, 0.1f, 500.0f, true});
        }
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

        duin::Entity e = world.Entity();
        duin::SceneBuilder builder(&world);
        builder.InstantiateEntity(pe, e);

        CHECK(e.GetName() == "DisabledEntity");
    }
}

} // namespace TestSceneBuilder
