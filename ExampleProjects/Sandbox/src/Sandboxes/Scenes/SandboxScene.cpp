#include "SandboxScene.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/ECS/ECSModule.h>

duin::ECSManager ecs;
duin::World world;

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

SandboxScene::SandboxScene(duin::GameStateMachine &sm) : duin::GameState(sm)
{
}

SandboxScene::~SandboxScene()
{
}

void SandboxScene::Enter()
{
    DN_INFO("SandboxScene Entered.");

    world.Component<X>();
    world.Component<Y>();
    world.Component<Z>();

    duin::Entity e = world.CreateEntity().Set<X>(1).Set<Y>(2).Set<Z>(3);

    // duin::PackedScene pscn = duin::PackedScene::Pack(e);

    std::vector<std::string> sercomVec;
    e.ForEachComponent([&](duin::Entity& ent) {
        std::string sercom = duin::ComponentSerializer::Get().Serialize(ent.GetName(), e.Get(ent));
        sercomVec.push_back(sercom);
        DN_INFO("SERCOM: {}", sercom);
    });

    duin::Entity e2 = world.CreateEntity();
    for (auto& str : sercomVec)
    {
        //void *data = duin::ComponentSerializer::Get().Deserialize(rfl)
    }
    e2.ForEachComponent([&](duin::Entity &ent) {
        std::string sercom = duin::ComponentSerializer::Get().Serialize(ent.GetName(), e.Get(ent));
        sercomVec.push_back(sercom);
        DN_INFO("SERCOM: {}", sercom);
    });
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
