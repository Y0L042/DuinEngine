#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
{

TEST_SUITE("PackedScene Pack and Instantiate")
{
    TEST_CASE("Pack single entity as scene entity")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.Entity("EmptyRoot");
        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({root});

        // PackScene({entity}) packs the entity itself as ps.entities[0]
        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "EmptyRoot");
        CHECK(ps.entities[0].children.empty());
    }

    TEST_CASE("Pack scene with multiple root-level entities")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity a = world.Entity("A").Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity b = world.Entity("B").Set<Vec3>(0.0f, 1.0f, 0.0f);
        duin::Entity c = world.Entity("C").Set<Camera>(90.0f, 0.1f, 1000.0f, true);

        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({a, b, c});

        CHECK(ps.entities.size() == 3);
    }

    TEST_CASE("Instantiate scene into world")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::PackedScene scene;
        scene.uuid = duin::UUID::FromStringHex("ab11111111111111");
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

        // Instantiate into a fresh world; InstantiateScene creates entities at world root
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder builder(&world2);
        builder.InstantiateScene(scene, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 2);
    }

    TEST_CASE("Pack then Instantiate scene round-trip preserves data")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        // Build scene with two world-root entities
        duin::Entity player =
            world.Entity("Player").Set<Vec3>(1.0f, 2.0f, 3.0f).Set<Camera>(90.0f, 0.1f, 1000.0f, true);
        duin::Entity enemy = world.Entity("Enemy").Set<Vec3>(10.0f, 0.0f, 10.0f);

        // Pack the two entities as separate scene root entities
        duin::SceneBuilder builder(&world);
        duin::PackedScene ps = builder.PackScene({player, enemy});
        ps.uuid = duin::UUID::FromStringHex("0ac1111111111111");
        ps.name = "PackInstantiateRoundTrip";

        CHECK(ps.entities.size() == 2);

        // Instantiate into new world; entities appear at world root
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 2);

        // Find Player and Enemy by checking component values
        duin::Entity *restoredPlayer = nullptr;
        duin::Entity *restoredEnemy = nullptr;
        for (auto &e : worldEntities)
        {
            if (e.Has<Camera>())
                restoredPlayer = &e;
            else
                restoredEnemy = &e;
        }

        REQUIRE(restoredPlayer != nullptr);
        REQUIRE(restoredEnemy != nullptr);

        CHECK(restoredPlayer->Has<Vec3>());
        CHECK(restoredPlayer->Has<Camera>());
        CHECK(restoredPlayer->GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f});
        CHECK(restoredPlayer->GetMut<Camera>() == Camera{90.0f, 0.1f, 1000.0f, true});

        CHECK(restoredEnemy->Has<Vec3>());
        CHECK(restoredEnemy->GetMut<Vec3>() == Vec3{10.0f, 0.0f, 10.0f});
    }
}

} // namespace TestSceneBuilder
