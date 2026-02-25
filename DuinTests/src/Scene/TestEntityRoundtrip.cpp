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

// Helper: strip the '#uuid' suffix that SceneBuilder::InstantiateEntity appends.
static std::string BaseName(const std::string &name)
{
    auto pos = name.find('#');
    return pos != std::string::npos ? name.substr(0, pos) : name;
}

TEST_SUITE("Entity Pack-Instantiate Round-trip")
{
    TEST_CASE("Pack then Instantiate single entity with components")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity original =
            world.Entity("Player").Set<Vec3>(10.0f, 20.0f, 30.0f).Set<Camera>(90.0f, 0.1f, 1000.0f, true);

        // Pack as scene entity; original is ps.entities[0]
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({original});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Player");
        CHECK(ps.entities[0].components.size() == 2);

        // Instantiate into new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "Player");
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

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(1.0f, 2.0f, 3.0f);
        duin::Entity child = world.Entity("Child").Set<Camera>(60.0f, 0.5f, 500.0f, false);
        child.ChildOf(parent);

        // Pack parent as scene entity; child is nested inside
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].name == "Parent");
        CHECK(ps.entities[0].children.size() == 1);
        if (ps.entities[0].children.size() >= 1)
            CHECK(ps.entities[0].children[0].name == "Child");

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "Parent");
        CHECK(restored.Has<Vec3>());

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(BaseName(children[0].GetName()) == "Child");
            CHECK(children[0].Has<Camera>());
            CHECK(children[0].GetMut<Camera>() == Camera{60.0f, 0.5f, 500.0f, false});
        }
    }

    TEST_CASE("Pack then Instantiate deep hierarchy")
    {
        duin::World world;
        world.Component<Vec3>();

        duin::Entity root = world.Entity("Root").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity mid = world.Entity("Middle").Set<Vec3>(1.0f, 1.0f, 1.0f);
        duin::Entity leaf = world.Entity("Leaf").Set<Vec3>(2.0f, 2.0f, 2.0f);
        mid.ChildOf(root);
        leaf.ChildOf(mid);

        // Pack root as scene entity
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({root});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 1);
        if (ps.entities[0].children.size() >= 1)
        {
            CHECK(ps.entities[0].children[0].children.size() == 1);
            if (ps.entities[0].children[0].children.size() >= 1)
                CHECK(ps.entities[0].children[0].children[0].name == "Leaf");
        }

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "Root");

        std::vector<duin::Entity> midChildren = restored.GetChildren();
        CHECK(midChildren.size() == 1);
        if (midChildren.size() >= 1)
        {
            CHECK(BaseName(midChildren[0].GetName()) == "Middle");

            std::vector<duin::Entity> leafChildren = midChildren[0].GetChildren();
            CHECK(leafChildren.size() == 1);
            if (leafChildren.size() >= 1)
            {
                CHECK(BaseName(leafChildren[0].GetName()) == "Leaf");
                CHECK(leafChildren[0].Has<Vec3>());
                CHECK(leafChildren[0].GetMut<Vec3>() == Vec3{2.0f, 2.0f, 2.0f});
            }
        }
    }

    TEST_CASE("Pack then Instantiate entity with multiple children")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<Camera>();

        duin::Entity parent = world.Entity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
        duin::Entity childA = world.Entity("ChildA").Set<Vec3>(1.0f, 0.0f, 0.0f);
        duin::Entity childB = world.Entity("ChildB").Set<Camera>(45.0f, 0.2f, 800.0f, true);
        childA.ChildOf(parent);
        childB.ChildOf(parent);

        // Pack parent as scene entity
        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({parent});

        CHECK(ps.entities.size() == 1);
        CHECK(ps.entities[0].children.size() == 2);

        // Instantiate
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        CHECK(worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        CHECK(BaseName(restored.GetName()) == "Parent");

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 2);
    }
}

} // namespace TestSceneBuilder
