#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include "Defines.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>
#include <vector>

namespace TestSceneBuilder
{
    // TODO Tags are still seen as components, and are packed and serialized as components (this works, but a bit confusing).

TEST_SUITE("Tag Component Tests")
{
    // Define tag components (zero-size components used as markers)
    struct PlayerTag {};
    struct EnemyTag {};
    struct CollectibleTag {};
    struct ActiveTag {};
    struct DestroyableTag {};

    static std::string BaseName(const std::string &name)
    {
        auto pos = name.find('#');
        return pos != std::string::npos ? name.substr(0, pos) : name;
    }

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

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({entity});

        MSG_CHECK(ps.entities.size(), ps.entities.size() == 1);
        MSG_CHECK(ps.entities[0].name, ps.entities[0].name == "TaggedEntity");
        MSG_CHECK(ps.entities[0].components.size(), ps.entities[0].components.size() >= 1); // At least Vec3

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<PlayerTag>();
        world2.Component<ActiveTag>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(ps, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        MSG_CHECK(worldEntities.size(), worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        MSG_CHECK(restored.GetName(), BaseName(restored.GetName()) == "TaggedEntity");
        MSG_CHECK(restored.Has<Vec3>(), restored.Has<Vec3>());
        MSG_CHECK(restored.Has<PlayerTag>(), restored.Has<PlayerTag>());
        MSG_CHECK(restored.Has<ActiveTag>(), restored.Has<ActiveTag>());
        MSG_CHECK(restored.GetMut<Vec3>(), (restored.GetMut<Vec3>() == Vec3{1.0f, 2.0f, 3.0f}));
    }

    TEST_CASE("Pack and Instantiate multiple entities with different tag combinations")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<PlayerTag>();
        world.Component<EnemyTag>();
        world.Component<CollectibleTag>();
        world.Component<ActiveTag>();

        duin::Entity player = world.CreateEntity("Player")
            .Set<Vec3>(0.0f, 0.0f, 0.0f)
            .Add<PlayerTag>()
            .Add<ActiveTag>();

        duin::Entity enemy = world.CreateEntity("Enemy")
            .Set<Vec3>(10.0f, 0.0f, 0.0f)
            .Add<EnemyTag>()
            .Add<ActiveTag>();

        duin::Entity item = world.CreateEntity("HealthPack")
            .Set<Vec3>(5.0f, 0.0f, 5.0f)
            .Add<CollectibleTag>();

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene(&world);
        scene.name = "TaggedScene";

        // Instantiate in new world
        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<PlayerTag>();
        world2.Component<EnemyTag>();
        world2.Component<CollectibleTag>();
        world2.Component<ActiveTag>();

        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(scene, &world2);

        std::vector<duin::Entity> children = world2.GetChildren();
        MSG_CHECK(children.size(), children.size() == 3);

        // Verify tags are preserved
        int activeCount = 0;
        for (auto &child : children)
        {
            if (child.Has<ActiveTag>()) activeCount++;
        }
        MSG_CHECK(activeCount, activeCount == 2);
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

        duin::SceneBuilder sb(&world);
        duin::PackedScene ps = sb.PackScene({entity});
        duin::JSONValue json = sb.SerializeScene(ps);
        duin::PackedScene deserialized = sb.DeserializeScene(json);

        MSG_CHECK(deserialized.entities.size(), deserialized.entities.size() == 1);
        if (deserialized.entities.empty())
            return;
        MSG_CHECK(deserialized.entities[0].name, deserialized.entities[0].name == "TagOnlyEntity");

        // Instantiate
        duin::World world2;
        world2.Component<PlayerTag>();
        world2.Component<EnemyTag>();
        world2.Component<DestroyableTag>();

        duin::SceneBuilder sb2(&world2);
        sb2.InstantiateScene(deserialized, &world2);

        std::vector<duin::Entity> worldEntities = world2.GetChildren();
        MSG_CHECK(worldEntities.size(), worldEntities.size() == 1);
        if (worldEntities.empty())
            return;

        duin::Entity &restored = worldEntities[0];
        MSG_CHECK(restored.GetName(), BaseName(restored.GetName()) == "TagOnlyEntity");
        MSG_CHECK(restored.Has<PlayerTag>(), restored.Has<PlayerTag>());
        MSG_CHECK(restored.Has<DestroyableTag>(), restored.Has<DestroyableTag>());
        MSG_CHECK_FALSE(restored.Has<EnemyTag>(), restored.Has<EnemyTag>());
    }

    TEST_CASE("Tag components persist through scene serialization")
    {
        duin::World world;
        world.Component<Vec3>();
        world.Component<ActiveTag>();
        world.Component<DestroyableTag>();

        duin::Entity entity = world.CreateEntity("TaggedEntity")
            .Set<Vec3>(1.0f, 2.0f, 3.0f)
            .Add<ActiveTag>()
            .Add<DestroyableTag>();

        duin::SceneBuilder builder(&world);
        duin::PackedScene scene = builder.PackScene(&world);
        scene.name = "TagPersistenceTest";
        scene.uuid = duin::UUID::FromStringHex("tagtest11111111");

        duin::JSONValue json = builder.SerializeScene(scene);
        std::string jsonStr = json.Write();

        duin::JSONValue json2 = duin::JSONValue::Parse(jsonStr);
        duin::PackedScene scene2 = builder.DeserializeScene(json2);

        duin::World world2;
        world2.Component<Vec3>();
        world2.Component<ActiveTag>();
        world2.Component<DestroyableTag>();

        duin::SceneBuilder builder2(&world2);
        builder2.InstantiateScene(scene2, &world2);

        std::vector<duin::Entity> children = world2.GetChildren();
        MSG_CHECK(children.size(), children.size() == 1);
        if (children.size() >= 1)
        {
            MSG_CHECK(children[0].Has<Vec3>(), children[0].Has<Vec3>());
            MSG_CHECK(children[0].Has<ActiveTag>(), children[0].Has<ActiveTag>());
            MSG_CHECK(children[0].Has<DestroyableTag>(), children[0].Has<DestroyableTag>());
        }
    }
}

} // namespace TestSceneBuilder
