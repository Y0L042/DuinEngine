#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>
#include <vector>

namespace TestSceneBuilder
{

TEST_SUITE("Component Serialization")
{
    TEST_CASE("Deserialize Component")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        std::string jsonStr = R"(
        {   
            "type" : "TestStructX",
            "x" : "3"
        })";
        std::string normalizedJsonStr = duin::JSONValue::Parse(jsonStr).Write();
        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);

        duin::PackedComponent p = sb.DeserializeComponent(v);

        CHECK(p.jsonData == normalizedJsonStr);
    }

    TEST_CASE("Serialize Component")
    {
        duin::World world;
        duin::SceneBuilder sb(&world);

        std::string jsonStr = R"({"type":"TestStructX","x":"3"})";
        duin::PackedComponent p{.jsonData = jsonStr};

        duin::JSONValue v = duin::JSONValue::Parse(jsonStr);

        std::string pstr = sb.SerializeComponent(p).Write();

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
        duin::SceneBuilder sb(&world);
        duin::Entity e = world.CreateEntity().Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(3.0f, 2.0f, 4.0f, true);
        std::vector<duin::PackedComponent> cmpList;
        e.ForEachComponent([&](duin::Entity::ID cmpID) {
            if (cmpID.IsEntity())
            {
                duin::Entity cmp = cmpID.GetEntity();
                duin::PackedComponent pc = sb.PackComponent(e, cmp);
                cmpList.push_back(pc);
            }
        });

        CHECK(cmpList.size() == 2);

        if (cmpList.size() >= 2)
        {
            // Iteration order of entity's components are not guaranteed
            int vecIdx = cmpList[0].componentTypeName == "Vec3" ? 0 : 1;
            int camIdx = cmpList[1].componentTypeName == "Camera" ? 1 : 0;

            CHECK(cmpList[vecIdx].componentTypeName == "Vec3");
            CHECK(cmpList[vecIdx].jsonData == duin::JSONValue::Parse(R"({"type":"Vec3","x":1.0,"y":2.0,"z":3.0})").Write());
            CHECK(cmpList[camIdx].componentTypeName == "Camera");
            CHECK(cmpList[camIdx].jsonData ==
                  duin::JSONValue::Parse(R"({"type":"Camera","fov":3.0,"nearPlane":2.0,"farPlane":4.0,"isPrimary":true})")
                      .Write());
        }
    }

    TEST_CASE("Instantiate Component")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();
        duin::SceneBuilder sb(&world);
        std::vector<duin::PackedComponent> cmpList{2};
        cmpList[0].componentTypeName = "Vec3";
        cmpList[0].jsonData = R"({"type":"Vec3","x":1.0,"y":2.0,"z":3.0})";
        cmpList[1].componentTypeName = "Camera";
        cmpList[1].jsonData = R"({"type":"Camera","fov":3.0,"nearPlane":2.0,"farPlane":4.0,"isPrimary":true})";

        duin::Entity e = world.CreateEntity();
        for (duin::PackedComponent &cmp : cmpList)
        {
            sb.InstantiateComponent(cmp, e);
        }

        CHECK(e.GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
        CHECK(e.GetMut<Camera>() == Camera{3.0f, 2.0f, 4.0f, true});
    }
}

} // namespace TestSceneBuilder
