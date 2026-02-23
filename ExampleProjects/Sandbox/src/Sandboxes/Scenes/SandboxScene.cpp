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
    // Test_FlecsJSON();
    Test_SceneBuilder_01();
}

void SandboxScene::Test_Misc()
{
}

void SandboxScene::Test_FlecsJSON()
{
    /*
        fatal: observable.c: 758: assert(cr != tgt_cr): (ChildOf,#595) (CYCLE_DETECTED)

        flecs saves relationships per-instance. Serialization/deserialization does not work cross-instance
    */

    world = std::make_unique<duin::GameWorld>();
    world->Initialize();
    world->Component<TagA>();
    world->Component<TagB>();
    world->Component<TagC>();
    world->Component<X>();
    world->Component<Y>();

    duin::Entity e1 = world->CreateEntity().Add<TagA>();
    duin::Entity e2 = world->CreateEntity().ChildOf(e1).Add<TagB>();

    std::string e1JSON = static_cast<std::string>(e1.GetFlecsEntity().to_json());
    std::string e2JSON = static_cast<std::string>(e2.GetFlecsEntity().to_json());

    DN_INFO("e1JSON: \n{}\n", e1JSON);
    DN_INFO("e2JSON: \n{}\n", e2JSON);

    duin::GameWorld world2;
    world2.Initialize();
    world2.Component<TagA>();
    world2.Component<TagB>();
    world2.Component<TagC>();
    world2.Component<X>();
    world2.Component<Y>();

    duin::Entity eZ = world2.CreateEntity();
    duin::Entity eA = world2.CreateEntity();
    duin::Entity eB = world2.CreateEntity();
    eA.GetFlecsEntity().from_json(e1JSON.c_str());
    eB.GetFlecsEntity().from_json(e2JSON.c_str());

    DN_INFO("eAJSON: \n{}\n", static_cast<std::string>(eA.GetFlecsEntity().to_json()));
    DN_INFO("eBJSON: \n{}\n", static_cast<std::string>(eB.GetFlecsEntity().to_json()));
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

    duin::Entity e1 = world.CreateEntity("Parent").Add<TagA>();
    duin::Entity e2 = world.CreateEntity("Child").ChildOf(e1).Add<TagB>().Add<Target>(e1);
    duin::Entity e3 = world.CreateEntity("Baddy").Set<X>(10).Add<Target>(e1);
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
