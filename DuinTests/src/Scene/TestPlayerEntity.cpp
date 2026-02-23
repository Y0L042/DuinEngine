#include "TestConfig.h"
#include "TestSceneBuilderCommon.h"
#include <doctest.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/Core/Utils/UUID.h>
#include <Duin/IO/JSONValue.h>
#include <Duin/ECS/ECSModule.h>

namespace TestSceneBuilder
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

TEST_SUITE("Complex Player Entity Tests")
{
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
        duin::SceneBuilder sceneBuilder(&world);
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
        duin::PackedScene scene = sceneBuilder.PackScene({root});
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
