#include "SandboxScene.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/IO/JSONValue.h>

#include <memory>

std::unique_ptr<duin::GameWorld> world;

struct TagA
{
};
struct TagB
{
};
struct TagC
{
};
struct Target
{
};

struct X
{
    int x;
};
struct Y
{
    int y;
};
struct Z
{
    int z;
};
struct Vec3
{
    float x, y, z;

    bool operator==(const Vec3 &other)
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

SandboxScene::SandboxScene(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

SandboxScene::~SandboxScene()
{
}

void SandboxScene::Enter()
{
    Tests();
}

void SandboxScene::OnEvent(duin::Event e)
{
}

void SandboxScene::Update(double delta)
{
}

void SandboxScene::PhysicsUpdate(double delta)
{
}

void SandboxScene::Draw()
{
}

void SandboxScene::DrawUI()
{
}

void SandboxScene::Exit()
{
}

void SandboxScene::SetPause()
{
}

void SandboxScene::SetUnpause()
{
}

void SandboxScene::Tests()
{
    // TestMisc();
    //Test_FlecsJSON();
    //Test_SceneBuilder_01();
    Test_FlecsNames();
}

void SandboxScene::Test_Misc()
{
}

void SandboxScene::Test_FlecsJSON()
{
    flecs::world world;
    world.component<TagA>();
    world.component<TagB>();
    world.component<TagC>();
    world.component<X>();
    world.component<Y>();

    flecs::entity e1 = world.entity().add<TagA>();
    flecs::entity e2 = world.entity().child_of(e1).add<TagB>();

    std::string e1JSON = static_cast<std::string>(e1.to_json());
    std::string e2JSON = static_cast<std::string>(e2.to_json());

    DN_INFO("e1JSON {}: \n{}\n", e1.raw_id(), e1JSON);
    DN_INFO("e2JSON {}: \n{}\n", e2.raw_id(), e2JSON);

    flecs::world world2;
    world2.component<TagA>();
    world2.component<TagB>();
    world2.component<TagC>();
    world2.component<X>();
    world2.component<Y>();

    flecs::entity eZ = world2.entity();
    flecs::entity eA = world2.entity();
    flecs::entity eB = world2.entity();
    eA.from_json(e1JSON.c_str());
    eB.from_json(e2JSON.c_str());

    std::string eZJSON = static_cast<std::string>(eZ.to_json());
    std::string eAJSON = static_cast<std::string>(eA.to_json());
    std::string eBJSON = static_cast<std::string>(eB.to_json());

    DN_INFO("eZJSON {}: \n{}\n", eZ.raw_id(), eZJSON),
    DN_INFO("eAJSON {}: \n{}\n", eA.raw_id(), eAJSON);
    DN_INFO("eBJSON {}: \n{}\n", eB.raw_id(), eBJSON);
}

// Testing Packing and Instantiation of Entities with Pairs
void SandboxScene::Test_SceneBuilder_01()
{
    duin::World world;
    duin::SceneBuilder sceneBuilder(&world);

    world.Component<TagA>();
    world.Component<TagB>();
    world.Component<TagC>();
    world.Component<X>();
    world.Component<Y>();
    world.Component<Target>();

    duin::Entity e1 = world.Entity("Parent").Add<TagA>();
    duin::Entity e2 = world.Entity("Child").ChildOf(e1).Add<TagB>().Add<Target>(e1);
    duin::Entity e3 = world.Entity("Baddy").Set<X>(10).Add<Target>(e1);
    e1.Add<Target>(e3);

    duin::PackedScene pscn = sceneBuilder.PackScene(&world);

    DN_INFO("pscn:\n{}\n", sceneBuilder.SerializeScene(pscn).Write());

    duin::World world2;
    duin::SceneBuilder sceneBuilder2(&world2);

    world2.Component<TagA>();
    world2.Component<TagB>();
    world2.Component<TagC>();
    world2.Component<X>();
    world2.Component<Y>();
    world2.Component<Target>();

    sceneBuilder2.InstantiateScene(pscn, &world2);

    duin::PackedScene pscn2 = sceneBuilder2.PackScene(&world2);

    DN_INFO("pscn:\n{}\n", sceneBuilder2.SerializeScene(pscn2).Write());
}

void SandboxScene::Test_FlecsNames()
{
    struct Target
    {
    };

            world.Component<Targets>();

    duin::Entity parent = world.CreateEntity("Parent").Set<Vec3>(0.0f, 0.0f, 0.0f);
    duin::Entity child = world.CreateEntity("Child").ChildOf(parent).Set<Vec3>(1.0f, 0.0f, 0.0f);
    duin::Entity target = world.CreateEntity("TargetMan").Set<Vec3>(5.0f, 0.0f, 0.0f);
    child.Add<Targets>(target);

    flecs::world world;
    world.component<Target>();
    auto t1 = world.entity("Twin");
    auto t2 = world.entity("Twin").add<Target>();

    auto t3 = world.entity("Target");

    DN_INFO("T1: {}", static_cast<std::string>(t1.to_json()));
    DN_INFO("T2: {}", static_cast<std::string>(t2.to_json()));
    DN_INFO("t3 {} vs {} lookup(t3)", t3.raw_id(), world.lookup("Target").raw_id());
}
