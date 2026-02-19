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
        if (pe.children.size() >= 1)
        {
            CHECK(pe.children[0].name == "Child");
        }

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<Camera>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Parent");
        CHECK(restored.Has<Vec3>());

        std::vector<duin::Entity> children = restored.GetChildren();
        CHECK(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].GetName() == "Child");
            CHECK(children[0].Has<Camera>());
            CHECK(children[0].GetMut<Camera>() == Camera{60.0f, 0.5f, 500.0f, false});
        }
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
        if (pe.children.size() >= 1)
        {
            CHECK(pe.children[0].children.size() == 1);
            if (pe.children[0].children.size() >= 1)
            {
                CHECK(pe.children[0].children[0].name == "Leaf");
            }
        }

        duin::World world2;
        world2.Component<Vec3>();
        duin::Entity restored = world2.CreateEntity();
        duin::PackedEntity::Instantiate(pe, restored);

        CHECK(restored.GetName() == "Root");
        std::vector<duin::Entity> midChildren = restored.GetChildren();
        CHECK(midChildren.size() == 1);
        if (midChildren.size() >= 1)
        {
            CHECK(midChildren[0].GetName() == "Middle");

            std::vector<duin::Entity> leafChildren = midChildren[0].GetChildren();
            CHECK(leafChildren.size() == 1);
            if (leafChildren.size() >= 1)
            {
                CHECK(leafChildren[0].GetName() == "Leaf");
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

} // namespace TestSceneBuilder
