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

        duin::PackedScene scene = duin::PackedScene::Pack({root});
        scene.name = "TaggedScene";

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<PlayerTag>();
        world2.Component<EnemyTag>();
        world2.Component<CollectibleTag>();
        world2.Component<ActiveTag>();

        duin::Entity newRoot = world2.CreateEntity("NewRoot");
        duin::PackedScene::Instantiate(scene, &world2);

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

        duin::PackedScene scene = duin::PackedScene::Pack({root});
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
        duin::PackedScene::Instantiate(scene2, &world2);

        std::vector<duin::Entity> children = newRoot.GetChildren();
        REQUIRE(children.size() == 1);
        if (children.size() >= 1)
        {
            CHECK(children[0].Has<Vec3>());
            CHECK(children[0].Has<ActiveTag>());
            CHECK(children[0].Has<DestroyableTag>());
        }
    }
}

} // namespace TestSceneBuilder
