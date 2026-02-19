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
    TEST_CASE("Pack empty scene root")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.CreateEntity("EmptyRoot");
        duin::PackedScene ps = duin::PackedScene::Pack({root});

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

        duin::PackedScene ps = duin::PackedScene::Pack({root});

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
        duin::PackedScene::Instantiate(scene, &world);

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
        duin::PackedScene ps = duin::PackedScene::Pack({root});
        ps.uuid = duin::UUID::FromStringHex("packinstrt111111");
        ps.name = "PackInstantiateRoundTrip";

        CHECK(ps.entities.size() == 2);


        // Instantiate into new root
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(ps, &world2);

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

} // namespace TestSceneBuilder
