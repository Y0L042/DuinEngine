#include "dnpch.h"
#include "GameWorld.h"
#include "Duin/Core/Application.h"
#include "Duin/Core/Utils/SerialisationManager.h"
#include "Duin/Render/Camera.h"
#include "PrefabRegistry.h"

#include <functional>

namespace duin
{

/*----------------------------------------------------------------------------*/
// ECS Structures
/*----------------------------------------------------------------------------*/
namespace ECSTag
{
void RegisterTags(duin::World &world)
{
    world.Component<ECSTag::Local>();
    world.Component<ECSTag::Global>();

    world.Component<ECSTag::CreateExternalRef>();
    world.Component<ECSTag::ActiveExternalRef>();
    world.Component<ECSTag::DeleteExternalRef>();

    world.Component<ECSTag::PxKinematic>();
    world.Component<ECSTag::PxDynamic>();
    world.Component<ECSTag::PxStatic>();
    world.Component<ECSTag::NonPx>();

    world.Component<ECSTag::SetCameraAsActive>();
    world.Component<ECSTag::CameraIsActive>();
    world.Component<ECSTag::ActiveCamera>();
}
} // namespace ECSTag

namespace ECSComponent
{
void RegisterComponents(duin::World &world)
{
    world.Component<ECSComponent::Position2D>();
    world.Component<ECSComponent::Rotation2D>();
    world.Component<ECSComponent::Scale2D>();
    world.Component<ECSComponent::Velocity2D>();

    world.Component<ECSComponent::Transform3D>();
    world.Component<ECSComponent::Position3D>();
    world.Component<ECSComponent::Rotation3D>();
    world.Component<ECSComponent::Scale3D>();
    world.Component<ECSComponent::Velocity3D>();

    world.Component<ECSComponent::CubeComponent>();
    world.Component<ECSComponent::StaticBodyComponent>();
    world.Component<ECSComponent::KinematicBodyComponent>();
    world.Component<ECSComponent::DynamicBodyComponent>();
    world.Component<ECSComponent::CharacterBodyComponent>();
    world.Component<ECSComponent::PhysicsStaticCubeComponent>();

    world.Component<ECSComponent::DebugCapsuleComponent>();
    world.Component<ECSComponent::DebugCubeComponent>();

    world.Component<Camera>();
}
} // namespace ECSComponent

namespace ECSPrefab
{
/* --- Prefabs --- */
duin::Entity Node;
duin::Entity Node2D;
duin::Entity Node3D;

duin::Entity PhysicsStaticBody;
duin::Entity PhysicsKinematicBody;
duin::Entity PhysicsDynamicBody;
duin::Entity PhysicsCharacterBody;

duin::Entity Camera3D;
duin::Entity Cube;
duin::Entity DebugCapsule;

void RegisterPrefabs(duin::World &world)
{
    Node = world.CreatePrefab("Node").Add<UUID>();

    Node2D = world.CreatePrefab("Node2D");
    Node2D.IsA(Node)
        .SetPair<ECSComponent::Position2D, ECSTag::Local>(ECSComponent::Position2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Rotation2D, ECSTag::Local>(ECSComponent::Rotation2D{0.0f})
        .SetPair<ECSComponent::Scale2D, ECSTag::Local>(ECSComponent::Scale2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Position2D, ECSTag::Global>(ECSComponent::Position2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Rotation2D, ECSTag::Global>(ECSComponent::Rotation2D{0.0f})
        .SetPair<ECSComponent::Scale2D, ECSTag::Global>(ECSComponent::Scale2D{0.0f, 0.0f});

    Node3D = world.CreatePrefab("Node3D").IsA(Node).Set<ECSComponent::Transform3D>(ECSComponent::Transform3D{});

    PhysicsStaticBody = world.CreatePrefab("StaticBody")
                            .IsA(Node3D)
                            .Add<ECSTag::PxStatic>()
                            .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f})
                            .Set<ECSComponent::StaticBodyComponent>(ECSComponent::StaticBodyComponent{nullptr});

    PhysicsKinematicBody =
        world.CreatePrefab("KinematicBody")
            .IsA(Node3D)
            .Add<ECSTag::PxKinematic>()
            .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f})
            .Set<ECSComponent::KinematicBodyComponent>(ECSComponent::KinematicBodyComponent{nullptr});

    PhysicsDynamicBody = world.CreatePrefab("DynamicBody")
                             .IsA(Node3D)
                             .Add<ECSTag::PxDynamic>()
                             .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f})
                             .Set<ECSComponent::DynamicBodyComponent>(ECSComponent::DynamicBodyComponent{nullptr});

    PhysicsCharacterBody =
        world.CreatePrefab("CharacterBody")
            .IsA(Node3D)
            .Add<ECSTag::PxKinematic>()
            .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f})
            .Set<ECSComponent::CharacterBodyComponent>(ECSComponent::CharacterBodyComponent{nullptr});

    Camera3D = world.CreatePrefab("Camera3D")
                   .IsA(Node3D)
                   .Set<Camera>(Camera{
                       /* .uuid =  */ UUID(),
                       /* .position =  */ {0.0f, 0.0f, 0.0f},
                       /* .target =  */ {0.0f, 0.0f, 0.0f},
                       /* .up =  */ {0.0f, 1.0f, 0.0f},
                       /* .fovy =  */ 72.0f
                       // .projection = ::CAMERA_PERSPECTIVE
                   });

    Cube = world.CreatePrefab("Cube").IsA(Node3D);
    // TODO REPLACE_RAYLIB
    // .Set<ECSComponent::CubeComponent>({...})

    DebugCapsule = world.CreatePrefab("DebugCapsule").IsA(Node3D);
    // TODO REPLACE_RAYLIB
    // .Set<ECSComponent::DebugCapsuleComponent>({...})

    PrefabRegistry::Get().RegisterPrefabEntity("Node", Node);
    PrefabRegistry::Get().RegisterPrefabEntity("Node2D", Node2D);
    PrefabRegistry::Get().RegisterPrefabEntity("Node3D", Node3D);
    PrefabRegistry::Get().RegisterPrefabEntity("PhysicsStaticBody", PhysicsStaticBody);
    PrefabRegistry::Get().RegisterPrefabEntity("PhysicsKinematicBody", PhysicsKinematicBody);
    PrefabRegistry::Get().RegisterPrefabEntity("PhysicsDynamicBody", PhysicsDynamicBody);
    PrefabRegistry::Get().RegisterPrefabEntity("PhysicsCharacterBody", PhysicsCharacterBody);
    PrefabRegistry::Get().RegisterPrefabEntity("Camera3D", Camera3D);
    PrefabRegistry::Get().RegisterPrefabEntity("Cube", Cube);
    PrefabRegistry::Get().RegisterPrefabEntity("DebugCapsule", DebugCapsule);
}
} // namespace ECSPrefab

namespace ECSObserver
{
void RegisterObservers(duin::World &world)
{
    flecs::world &flecsWorld = world.GetFlecsWorld();

    ECSComponent::Transform3D::RegisterOnAddObserver(flecsWorld);
    ECSComponent::Transform3D::RegisterOnSetObserver(flecsWorld);

    flecs::observer localObserver;
    flecs::observer globalObserver;

    // On .set<Position3D, Local>
    localObserver = flecsWorld.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Local>>()
                        .event(flecs::OnSet)
                        .each([globalObserver](flecs::iter &it, size_t i, ECSComponent::Position3D &lPos) {
                            // DN_CORE_INFO(".set<Position3D, Local>");
                            if (globalObserver.is_valid())
                            {
                                globalObserver.disable();
                                // DN_CORE_INFO("syncinc global");
                                Vector3 gPos = it.entity(i).try_get<ECSComponent::Position3D, ECSTag::Global>()->value;
                                it.entity(i).set<ECSComponent::Position3D, ECSTag::Global>({{0.0f, 0.0f, 0.0f}});
                                globalObserver.enable();
                            }
                        });

    // On .set<Position3D, Global>
    globalObserver = flecsWorld.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Global>>()
                         .event(flecs::OnSet)
                         .each([localObserver](flecs::iter &it, size_t i, ECSComponent::Position3D &gPos) {
                             // DN_CORE_INFO(".set<Position3D, Global>");
                             if (localObserver.is_valid())
                             {
                                 localObserver.disable();
                                 // DN_CORE_INFO("syncinc local");
                                 it.entity(i).set<ECSComponent::Position3D, ECSTag::Local>({{0.0f, 0.0f, 0.0f}});
                                 localObserver.enable();
                             }
                         });
}
}; // namespace ECSObserver


/*----------------------------------------------------------------------------*/
//  GameWorld Functions
/*----------------------------------------------------------------------------*/
GameWorld::GameWorld()
{

}

GameWorld::~GameWorld()
{
}

void GameWorld::Initialize()
{
    ECSTag::RegisterTags(*this);
    DN_CORE_INFO("Tags registered.");
    ECSComponent::RegisterComponents(*this);
    DN_CORE_INFO("Components registered.");
    ECSPrefab::RegisterPrefabs(*this);
    DN_CORE_INFO("Prefabs registered.");
    ECSObserver::RegisterObservers(*this);
    DN_CORE_INFO("Observers registered.");

    QueuePostUpdateCallback(std::function<void(double)>([this](double delta) { PostUpdateQueryExecution(delta); }));
    QueuePostPhysicsUpdateCallback(
        std::function<void(double)>([this](double delta) { PostPhysicsUpdateQueryExecution(delta); }));
    QueuePostDrawCallback(std::function<void()>([this]() { PostDrawQueryExecution(); }));
    QueuePostDrawUICallback(std::function<void()>([this]() { PostDrawUIQueryExecution(); }));
}

duin::Entity GameWorld::CreateEntityFromJSON(JSONMember &member)
{
    std::string ecsData = member.GetMemberDataAsString();
    duin::Entity entity = this->CreateEntity();
    entity.GetFlecsEntity().from_json(ecsData.c_str());
    return entity;
}

void GameWorld::ActivateCameraEntity(duin::Entity entity)
{
    if (entity.Has<Camera>())
    {
        this->DeferBegin();
        this->GetFlecsWorld().each([](flecs::entity e, ECSTag::ActiveCamera) { e.remove<ECSTag::ActiveCamera>(); });
        entity.Add<ECSTag::ActiveCamera>();
        this->DeferEnd();
    }
}

void GameWorld::PostUpdateQueryExecution(double delta)
{
}

void GameWorld::PostPhysicsUpdateQueryExecution(double delta)
{
    /* Update physics-objects' positions first */
    ExecuteQueryCharacterBody3DUpdateTransform();
    ExecuteQuerySyncDynamicBody3DTransform();
    ExecuteQueryTransform3DHierarchicalUpdate();

    ExecuteQueryControlCamera();
    ExecuteQuerySetCameraAsActive();
}

void GameWorld::PostDrawQueryExecution()
{
    ExecuteQueryDrawCube();
    ExecuteQueryDrawDebugCapsule();
    ExecuteQueryDrawDebugCube();
}

void GameWorld::PostDrawUIQueryExecution()
{
}

void GameWorld::Clear()
{
    this->GetFlecsWorld().reset();
}

void GameWorld::InitializeRemoteExplorer()
{
}

/*----------------------------------------------------------------------
 * Transform Updates
----------------------------------------------------------------------*/
void GameWorld::ExecuteQueryCharacterBody3DUpdateTransform()
{
    static auto q =
        this->QueryBuilder<ECSComponent::CharacterBodyComponent, ECSComponent::Transform3D, ECSComponent::Velocity3D>()
            .With<ECSTag::PxKinematic>()
            .Cached()
            .Build();
    q.Each([](duin::Entity e, ECSComponent::CharacterBodyComponent &cb, ECSComponent::Transform3D &tx,
              ECSComponent::Velocity3D &velocity) {
        // Move CharacterBody3D and get physics-resolved global position
        double delta = GetPhysicsFrameTime();
        Vector3 vDelta = Vector3Scale(velocity.value, GetPhysicsFrameTime());

        if (!cb.body)
        {
            DN_WARN("CharacterBody pointer is nullptr!");
            return;
        }

        Vector3 oldPos = cb.body->GetFootPosition();
        cb.body->Move(vDelta, delta);
        Vector3 newPos = cb.body->GetFootPosition();

        // add distance between current globalPos (old) and pxBody global pos (new) to localPos
        Vector3 globalDelta = Vector3Subtract(newPos, ECSComponent::Transform3D::GetGlobalPosition(e.GetFlecsEntity()));
        Vector3 newLocalPos = Vector3Add(globalDelta, tx.GetPosition());
        tx.SetPosition(newLocalPos);

        velocity.value = cb.body->GetCurrentVelocity();

        // If autostep has happened, cap Y velocity
        Vector3 distanceMoved = Vector3Subtract(newPos, oldPos);
        static const float AUTOSTEP_FACTOR = 5.0f;
        if (std::abs(distanceMoved.y) > std::abs(vDelta.y * AUTOSTEP_FACTOR))
        {
            velocity.value.y = vDelta.y;
        }
    });
}

void GameWorld::ExecuteQuerySyncDynamicBody3DTransform()
{
    static auto q =
        this->QueryBuilder<const ECSComponent::DynamicBodyComponent, ECSComponent::Transform3D>().Cached().Build();

    q.Each([](duin::Entity e, const ECSComponent::DynamicBodyComponent &dynamicBodyComponent,
              ECSComponent::Transform3D &tx) {
        Vector3 newGPos = dynamicBodyComponent.body->GetPosition();
        ECSComponent::Transform3D::SetGlobalPosition(e.GetFlecsEntity(), newGPos);
        Quaternion newGRot = dynamicBodyComponent.body->GetRotation();
        ECSComponent::Transform3D::SetGlobalRotation(e.GetFlecsEntity(), newGRot);
    });
}

void GameWorld::ExecuteQueryTransform3DHierarchicalUpdate()
{
    flecs::world &flecsWorld = this->GetFlecsWorld();
    static flecs::query q = flecsWorld.query_builder<ECSComponent::Transform3D, const ECSComponent::Transform3D *>()
                                .term_at(1)
                                .parent()
                                .cascade()
                                .cached()
                                .build();

    q.each([](flecs::entity e, ECSComponent::Transform3D &tx, const ECSComponent::Transform3D *parent_tx) {
        // ECSComponent::Transform3D::SetGlobalPosition(e, tx.GetPosition());
        // tx.SetGlobalPosition(tx.GetPosition());
        if (parent_tx)
        {
            // Vector3 sum = Vector3Add(tx.GetPosition(), parent_tx->GetGlobalPosition());
            // tx.SetGlobalPosition(sum);
            //  Vector3 sum = Vector3Add(ECSComponent::Transform3D::GetGlobalPosition(e), parent_tx->GetPosition());
            // ECSComponent::Transform3D::SetGlobalPosition(e, sum);

            tx.InvalidateCacheFlags();

            // Force a recalculation by accessing the global position:
            ECSComponent::Transform3D::GetGlobalPosition(e);
        }
    });
}

void GameWorld::ExecuteQueryControlCamera()
{
    static auto q = this->QueryBuilder<Camera, const ECSComponent::Transform3D>().Cached().Build();

    q.Each([](duin::Entity e, Camera &c, const ECSComponent::Transform3D &tx) {
        Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e.GetFlecsEntity());
        Quaternion gRot = ECSComponent::Transform3D::GetGlobalRotation(e.GetFlecsEntity());

        // Rotate the default forward by gRot to get the new target position of the camera
        Vector3 defaultForward = {0.0f, 0.0f, -1.0f};
        Vector3 forward = Vector3RotateByQuaternion(defaultForward, gRot);

        c.position = gPos;
        c.target = Vector3Add(gPos, forward);
        c.up = {0.0f, 1.0f, 0.0f};
    });
}

void GameWorld::ExecuteQueryDrawCube()
{

    // TODO REPLACE_RAYLIB
    // static flecs::query q = world.query_builder<
    //     const ECSComponent::CubeComponent,
    //     const ECSComponent::Transform3D
    // >()
    // .cached()
    // .build();
    //
    // q.each([](
    //     flecs::entity e,
    //     const ECSComponent::CubeComponent& cube,
    //     const ECSComponent::Transform3D& tx
    //     ) {
    //        Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
    //        ::DrawCube(gPos.ToRaylib(),
    //                   cube.width,
    //                   cube.height,
    //                   cube.length,
    //                   cube.color);
    //     });
}

void GameWorld::ExecuteQueryDrawDebugCapsule()
{
    // TODO REPLACE_RAYLIB
    // static flecs::query q = world.query_builder<
    //     const ECSComponent::DebugCapsuleComponent,
    //     const ECSComponent::Transform3D
    // >()
    // .cached()
    // .build();
    //
    // q.each([](
    //     flecs::entity e,
    //     const ECSComponent::DebugCapsuleComponent& capsule,
    //     const ECSComponent::Transform3D& tx
    //     ) {
    //         Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
    //         ::DrawCapsuleWires(gPos.ToRaylib(),
    //             Vector3Add(gPos, { 0.0f, capsule.height, 0.0f }).ToRaylib(),
    //             capsule.radius,
    //             capsule.slices,
    //             capsule.rings,
    //             capsule.color
    //         );
    //     });
}

void GameWorld::ExecuteQueryDrawDebugCube()
{
    // TODO REPLACE_RAYLIB
    // static flecs::query q = world.query_builder<
    //     const ECSComponent::DebugCubeComponent,
    //     const ECSComponent::Transform3D
    // >()
    // .cached()
    // .build();
    //
    // q.each([](
    //     flecs::entity e,
    //     const ECSComponent::DebugCubeComponent& cube,
    //     const ECSComponent::Transform3D& tx
    //     ) {
    //         Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
    //         ::DrawCubeWires(
    //             Vector3Add(gPos, { cube.width / 2.0f, cube.height / 2.0f, cube.length / 2.0f }).ToRaylib(),
    //             cube.width,
    //             cube.height,
    //             cube.length,
    //             cube.color
    //         );
    //     });
}

void GameWorld::ExecuteQuerySetCameraAsActive()
{
    static auto q = this->QueryBuilder<Camera>().With<ECSTag::ActiveCamera>().Build();

    q.Each([](duin::Entity e, Camera &camera) { SetActiveCamera(&camera); });
}

} // namespace duin
