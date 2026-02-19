#include "SandboxScene.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/ECS/ECSModule.h>
#include <Duin/IO/JSONValue.h>

duin::ECSManager ecs;

struct TagA
{
};
struct TagB
{
};
struct TagC
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
    ecs.Initialize();
    duin::World &world = *ecs.world;
    world.Component<TagA>();
    world.Component<TagB>();
    world.Component<TagC>();
    world.Component<X>();
    world.Component<Y>();

    #if 0
    duin::Entity entityA = world.CreateEntity();
    duin::Entity entityB = world.CreateEntity().ChildOf(entityA);

    duin::Entity entityZ = world.CreateEntity();

    entityB.ForEachComponent([&](duin::Entity::ID componentID) {
        if (componentID.IsPair())
        {
            duin::Entity r = componentID.First();
            duin::Entity t = componentID.Second();
            DN_INFO("FIRSTY {}", static_cast<std::string>(r.GetName()));
            DN_INFO("SECONDY {}", t.GetID());

            if (r.GetName() == "ChildOf")
            {
                duin::Entity e(t.GetID());
                entityZ.ChildOf(e);
            }
        }
    });

    DN_INFO("EntityA ID {}", entityA.GetID());
    DN_INFO("EntityB ID {}", entityB.GetID());
    DN_INFO("EntityZ ID {}", entityZ.GetID());

    entityA.ForEachChild([&](duin::Entity e) { DN_INFO("EntityA Child's ID {}", e.GetID()); });
    #endif

#if 1
    duin::Entity prefab1 = world.CreatePrefab("Prefab1").Set<X>(3).Add<TagC>();

    duin::Entity e = world.CreateEntity("Player1").IsA(prefab1).Add<TagA>();
    e.Add<TagB>();
    e.Set<Y>(1);

    e.ForEachComponent([&](duin::Entity::ID componentID) {
        if (componentID.IsPair())
        {
            duin::Entity r = componentID.First();
            duin::Entity t = componentID.Second();
            DN_INFO("FIRSTY {}", static_cast<std::string>(r.GetName()));
            DN_INFO("SECONDY {}", static_cast<std::string>(t.GetName()));
        }
    });

    std::vector<std::string> sercomVec;
    e.ForEachComponent([&](duin::Entity::ID &entID) {
        if (entID.IsEntity() && e.Has(entID.GetEntity()))
        {
            std::string s = duin::ComponentSerializer::Get().Serialize(entID.GetEntity().GetName(), nullptr);
            sercomVec.push_back(s);
            DN_INFO("Serialized Tag {}", s);
        }
    });

    duin::World world2;
    world2.Component<TagA>();
    world2.Component<TagB>();
    world2.Component<TagC>();
    world2.Component<X>();
    world2.Component<Y>();

    duin::Entity prefab2 = world2.CreatePrefab("Prefab2").Set<X>(3).Add<TagC>();

    duin::Entity e2 = world2.CreateEntity("Player2");
    for (std::string &s : sercomVec)
    {
        duin::JSONValue v = duin::JSONValue::Parse(s);
        duin::ComponentSerializer::Get().Deserialize(e2, v["type"].GetString(), nullptr, s);
    }
    e2.ForEachComponent([&](duin::Entity::ID &entID) {
        if (entID.IsEntity() && e.Has(entID.GetEntity()))
        {
            duin::Entity ent = entID.GetEntity();
            DN_INFO("Entity 2 {} has tag {} ({})!", e2.GetID(), ent.GetName(), ent.GetID());
            std::string s = duin::ComponentSerializer::Get().Serialize(ent.GetName(), nullptr);
            DN_INFO("Entity 2 has Tag {}", s);
        }
    });

    std::string flecsJSON1 = static_cast<std::string>(e.GetFlecsEntity().to_json());
    DN_INFO("Entity 1 flecsJSON \n{}", flecsJSON1);
    std::string flecsJSON2 = static_cast<std::string>(e2.GetFlecsEntity().to_json());
    DN_INFO("Entity 2 flecsJSON \n{}", flecsJSON2);
#endif

#if 0
    ecs.Initialize();
    duin::World& world = *ecs.world;

    world.Component<Vec3>();
    world.Component<Camera>();

    duin::Entity original =
        world.CreateEntity("Player").Set<Vec3>(10.0f, 20.0f, 30.0f).Set<Camera>(90.0f, 0.1f, 1000.0f, true);

    duin::PackedEntity pe = duin::PackedEntity::Pack(original);

    duin::Entity restored = world.CreateEntity();
    duin::PackedEntity::Instantiate(pe, restored);
#endif

#if 0    
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
        duin::JSONValue v = duin::JSONValue::Parse(str);
        void *data = nullptr;
        duin::ComponentSerializer::Get().Deserialize(e2, v["type"].GetString(), data, str);

    }


    e2.ForEachComponent([&](duin::Entity &ent) {
        std::string desercom = duin::ComponentSerializer::Get().Serialize(ent.GetName(), e2.Get(ent));
        DN_INFO("DESERCOM: {}", desercom);
    });
#endif
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
