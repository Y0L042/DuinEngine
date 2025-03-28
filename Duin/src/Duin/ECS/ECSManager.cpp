#include "dnpch.h"
#include "ECSManager.h"
#include "Duin/Core/Application.h"
#include "Duin/Core/Utils/SerialisationManager.h"

#include <functional>



namespace duin {

/*----------------------------------------------------------------------------*/
// ECS Structures
/*----------------------------------------------------------------------------*/
namespace ECSTag {
    void RegisterTags(flecs::world& world)
    {
        world.component<ECSTag::Local>();
        world.component<ECSTag::Global>();

        world.component<ECSTag::CreateExternalRef>();
        world.component<ECSTag::ActiveExternalRef>();
        world.component<ECSTag::DeleteExternalRef>();

        world.component<ECSTag::PxKinematic>();
        world.component<ECSTag::PxDynamic>();
        world.component<ECSTag::PxStatic>();
        world.component<ECSTag::NonPx>();

        world.component<ECSTag::SetCameraAsActive>();
        world.component<ECSTag::CameraIsActive>();
        world.component<ECSTag::ActiveCamera>();
    }
}

namespace ECSComponent {
    void RegisterComponents(flecs::world& world)
    {
        world.component<ECSComponent::Position2D>();
        world.component<ECSComponent::Rotation2D>();
        world.component<ECSComponent::Scale2D>();
        world.component<ECSComponent::Velocity2D>();

        world.component<ECSComponent::Transform3D>();
        world.component<ECSComponent::Position3D>();
        world.component<ECSComponent::Rotation3D>();
        world.component<ECSComponent::Scale3D>();
        world.component<ECSComponent::Velocity3D>();

        world.component<ECSComponent::CubeComponent>();
        world.component<ECSComponent::StaticBodyComponent>();
        world.component<ECSComponent::KinematicBodyComponent>();
        world.component<ECSComponent::DynamicBodyComponent>();
        world.component<ECSComponent::CharacterBodyComponent>();
        world.component<ECSComponent::PhysicsStaticCubeComponent>();

        world.component<ECSComponent::DebugCapsuleComponent>();
        world.component<ECSComponent::DebugCubeComponent>();

        // Raylib components
        world.component<::Camera3D>();

    }
}

namespace ECSPrefab {
    /* --- Prefabs --- */
    flecs::entity Node;
    flecs::entity Node2D;
    flecs::entity Node3D;

    flecs::entity PhysicsStaticBody;
    flecs::entity PhysicsKinematicBody;
    flecs::entity PhysicsDynamicBody;
    flecs::entity PhysicsCharacterBody;

    flecs::entity Camera3D;
    flecs::entity Cube;
    flecs::entity DebugCapsule;


    void RegisterPrefabs(flecs::world& world)
    {
        Node = world.prefab("Node")
            .add<UUID>()
            ;



        Node2D = world.prefab("Node2D")
            .is_a(ECSPrefab::Node)
            .set<ECSComponent::Position2D, ECSTag::Local>({ 0.0f, 0.0f })
            .set<ECSComponent::Rotation2D, ECSTag::Local>(0.0f)
            .set<ECSComponent::Scale2D, ECSTag::Local>({ 0.0f, 0.0f })

            .set<ECSComponent::Position2D, ECSTag::Global>({ 0.0f, 0.0f })
            .set<ECSComponent::Rotation2D, ECSTag::Global>(0.0f)
            .set<ECSComponent::Scale2D, ECSTag::Global>({ 0.0f, 0.0f })
            ;



        Node3D = world.prefab("Node3D")
            .is_a(ECSPrefab::Node)
            .set<ECSComponent::Transform3D>({})
            ;

        PhysicsStaticBody = world.prefab("StaticBody")
            .is_a(ECSPrefab::Node3D)
            .add<ECSTag::PxStatic>()
            .set<ECSComponent::Velocity3D>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::StaticBodyComponent>({ nullptr });
            ;

        PhysicsKinematicBody = world.prefab("KinematicBody")
            .is_a(ECSPrefab::Node3D)
            .add<ECSTag::PxKinematic>()
            .set<ECSComponent::Velocity3D>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::KinematicBodyComponent>({ nullptr });
            ;

        PhysicsDynamicBody = world.prefab("DynamicBody")
            .is_a(ECSPrefab::Node3D)
            .add<ECSTag::PxDynamic>()
            .set<ECSComponent::Velocity3D>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::DynamicBodyComponent>({ nullptr });
            ;

        PhysicsCharacterBody = world.prefab("CharacterBody")
            .is_a(ECSPrefab::Node3D)
            .add<ECSTag::PxKinematic>()
            .set<ECSComponent::Velocity3D>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::CharacterBodyComponent>({ nullptr });
            ;



        Camera3D = world.prefab("Camera3D")
            .is_a(ECSPrefab::Node3D)
            .set<::Camera3D>({
                                 .position = { 0.0f, 0.0f, 0.0f },
                                 .target = { 0.0f, 0.0f, 0.0f },
                                 .up = { 0.0f, 1.0f, 0.0f },
                                 .fovy = 72.0f,
                                 .projection = ::CAMERA_PERSPECTIVE
                             })
            ;

        Cube = world.prefab("Cube")
            .is_a(ECSPrefab::Node3D)
            .set<ECSComponent::CubeComponent>({
                                              .width = 1.0f,
                                              .height = 1.0f,
                                              .length = 1.0f,
                                              .color = ::GRAY
                                              })
            ;

        

        DebugCapsule = world.prefab("DebugCapsule")
            .is_a(ECSPrefab::Node3D)
            .set<ECSComponent::DebugCapsuleComponent>({
                1.75f,
                1.0f,
                8, 16,
                ::GREEN
                })
            ;
            
    }
}

namespace ECSObserver {
        void RegisterObservers(flecs::world& world)
        {

            ECSComponent::Transform3D::RegisterOnAddObserver(world);
            ECSComponent::Transform3D::RegisterOnSetObserver(world);

            flecs::observer localObserver;
            flecs::observer globalObserver;

            // On .set<Position3D, Local>
            localObserver = world.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Local>>()
                .event(flecs::OnSet)
                .each(
                      [globalObserver](
                            flecs::iter& it, size_t i,
                            ECSComponent::Position3D& lPos
                        ) {
                          // DN_CORE_INFO(".set<Position3D, Local>");
                          if (globalObserver.is_valid()) {
                              globalObserver.disable();
                              // DN_CORE_INFO("syncinc global");
                              Vector3 gPos = it.entity(i).get<ECSComponent::Position3D, ECSTag::Global>()->value;
                              it.entity(i).set<ECSComponent::Position3D, ECSTag::Global>({{0.0f, 0.0f, 0.0f}});
                              globalObserver.enable();
                          }
                      });

            // On .set<Position3D, Global>
            globalObserver = world.observer<flecs::pair<ECSComponent::Position3D, ECSTag::Global>>()
                .event(flecs::OnSet)
                .each(
                      [localObserver](
                            flecs::iter& it, size_t i,
                            ECSComponent::Position3D& gPos
                        ) {
                          // DN_CORE_INFO(".set<Position3D, Global>");
                          if (localObserver.is_valid()) {
                              localObserver.disable();
                              // DN_CORE_INFO("syncinc local");
                              it.entity(i).set<ECSComponent::Position3D, ECSTag::Local>({{0.0f, 0.0f, 0.0f}});
                              localObserver.enable();
                          }
                      });
        }
};


    /*----------------------------------------------------------------------------*/
    //  ECSManager Functions
    /*----------------------------------------------------------------------------*/
    void ECSManager::Initialize()
    {
        ECSTag::RegisterTags(world);
        DN_CORE_INFO("Tags registered.");
        ECSComponent::RegisterComponents(world);
        DN_CORE_INFO("Components registered.");
        ECSPrefab::RegisterPrefabs(world);
        DN_CORE_INFO("Prefabs registered.");
        ECSObserver::RegisterObservers(world);
        DN_CORE_INFO("Observers registered.");

        QueuePostUpdateCallback(std::function<void(double)>([this](double delta) { PostUpdateQueryExecution(delta); }));
        QueuePostPhysicsUpdateCallback(std::function<void(double)>([this](double delta) { PostPhysicsUpdateQueryExecution(delta); }));
        QueuePostDrawCallback(std::function<void()>([this]() { PostDrawQueryExecution(); }));
        QueuePostDrawUICallback(std::function<void()>([this]() { PostDrawUIQueryExecution(); }));
    }

    flecs::entity ECSManager::CreateEntityFromJSON(JSONMember& member)
    {
        std::string ecsData = member.GetMemberDataAsString();
        flecs::entity entity = world.entity();
        entity.from_json(ecsData.c_str());
        return flecs::entity();
    }

    void ECSManager::ActivateCameraEntity(flecs::entity entity)
    {
        if (entity.has<::Camera3D>()) {
            world.defer_begin();
                world.each([](flecs::entity e, ECSTag::ActiveCamera) {
                    e.remove<ECSTag::ActiveCamera>();
                });
                entity.add<ECSTag::ActiveCamera>();
            world.defer_end();
        }
    }

    void ECSManager::SetGlobalPosition3D(flecs::entity entity, Vector3 newPos)
    {
        Vector3 deltaPos = newPos - entity.get<ECSComponent::Position3D, ECSTag::Global>()->value;
        Vector3 localPos = entity.get<ECSComponent::Position3D, ECSTag::Local>()->value;
        entity.set<ECSComponent::Position3D, ECSTag::Local>({ { localPos + deltaPos } });
        entity.set<ECSComponent::Position3D, ECSTag::Global>({ { newPos } });
    }

    void ECSManager::PostUpdateQueryExecution(double delta)
    {
        
    }

    void ECSManager::PostPhysicsUpdateQueryExecution(double delta)
    {
        /* Update physics-objects' positions first */
        ExecuteQueryCharacterBody3DUpdateTransform();
        ExecuteQuerySyncDynamicBody3DTransform();
        ExecuteQueryTransform3DHierarchicalUpdate();

        ExecuteQueryControlCamera();
        ExecuteQuerySetCameraAsActive();
    }

    void ECSManager::PostDrawQueryExecution()
    {
        ExecuteQueryDrawCube();
        ExecuteQueryDrawDebugCapsule();
        ExecuteQueryDrawDebugCube();
    }

    void ECSManager::PostDrawUIQueryExecution()
    {
    }

    void ECSManager::InitializeRemoteExplorer()
    {}


    /*----------------------------------------------------------------------  
     * Transform Updates
    ----------------------------------------------------------------------*/  
    void ECSManager::ExecuteQueryCharacterBody3DUpdateTransform()
    {
        static flecs::query q = world.query_builder<
            ECSComponent::CharacterBodyComponent,
            ECSComponent::Transform3D,
            ECSComponent::Velocity3D
        >()
            .with<ECSTag::PxKinematic>()
            .cached()
            .build();
        q.each([](
                flecs::entity e,
                ECSComponent::CharacterBodyComponent& cb,
                ECSComponent::Transform3D& tx,
                ECSComponent::Velocity3D& velocity
            ) {
                // Move CharacterBody3D and get physics-resolved global position
                double delta = GetPhysicsFrameTime();
                Vector3 vDelta = Vector3Scale(velocity.value, GetPhysicsFrameTime());

                Vector3 oldPos = cb.characterBody->GetFootPosition();
                cb.characterBody->Move(vDelta, delta);
                Vector3 newPos = cb.characterBody->GetFootPosition();

                // add distance between current globalPos (old) and pxBody global pos (new) to localPos
                Vector3 globalDelta = Vector3Subtract(newPos, ECSComponent::Transform3D::GetGlobalPosition(e));
                Vector3 newLocalPos = Vector3Add(globalDelta, tx.GetPosition()); 
                tx.SetPosition(newLocalPos);

                velocity.value = cb.characterBody->GetCurrentVelocity();

                // If autostep has happened, cap Y velocity
                Vector3 distanceMoved = Vector3Subtract(newPos, oldPos);
                static const float AUTOSTEP_FACTOR = 5.0f;
                if (std::abs(distanceMoved.y) > std::abs(vDelta.y * AUTOSTEP_FACTOR)) {
                    velocity.value.y = vDelta.y;
                } 
            });
    }

    void ECSManager::ExecuteQuerySyncDynamicBody3DTransform()
    {
        static flecs::query q = world.query_builder<
            const ECSComponent::DynamicBodyComponent,
            ECSComponent::Transform3D
        >()
        .cached()
        .build();

        q.each([](
                flecs::entity e,
                const ECSComponent::DynamicBodyComponent& dynamicBodyComponent,
                ECSComponent::Transform3D& tx
            ) {
               Vector3 newGPos = dynamicBodyComponent.dynamicBody->GetPosition();
               ECSComponent::Transform3D::SetGlobalPosition(e, newGPos);
               Quaternion newGRot = dynamicBodyComponent.dynamicBody->GetRotation();
               ECSComponent::Transform3D::SetGlobalRotation(e, newGRot);
            });
    }

    void ECSManager::ExecuteQueryTransform3DHierarchicalUpdate()
    {
        static flecs::query q = world.query_builder<
            ECSComponent::Transform3D,
            const ECSComponent::Transform3D * 
        >()
        .term_at(1).parent().cascade()
        .cached()
        .build();

        q.each([](
               flecs::entity e,
               ECSComponent::Transform3D& tx,
               const ECSComponent::Transform3D *parent_tx
            ) {
                 // ECSComponent::Transform3D::SetGlobalPosition(e, tx.GetPosition());
                //tx.SetGlobalPosition(tx.GetPosition());
                if (parent_tx) {
                    //Vector3 sum = Vector3Add(tx.GetPosition(), parent_tx->GetGlobalPosition());
                    //tx.SetGlobalPosition(sum);
                    // Vector3 sum = Vector3Add(ECSComponent::Transform3D::GetGlobalPosition(e), parent_tx->GetPosition());
                    //ECSComponent::Transform3D::SetGlobalPosition(e, sum);

                    tx.InvalidateCacheFlags();

                    // Force a recalculation by accessing the global position:
                    ECSComponent::Transform3D::GetGlobalPosition(e);
                }
            });
    }

    void ECSManager::ExecuteQueryUpdateCameraPosition()
    {
        static flecs::query q = world.query_builder<
            ::Camera3D,
            const ECSComponent::Transform3D
        >()
        .cached()
        .build();

        q.each([](
                flecs::entity e,
                ::Camera3D& camera,
                const ECSComponent::Transform3D& tx
            ) {
                Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
                camera.position = gPos.ToRaylib();
            });
    }

    void ECSManager::ExecuteQueryControlCamera()
    {
        static flecs::query q = world.query_builder<
            ::Camera3D, 
            const ECSComponent::Transform3D 
        >()
        .cached()
        .build();

        q.each([](
                flecs::entity e,
                ::Camera3D& c, 
                const ECSComponent::Transform3D& tx 
            ) {
                Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
                Quaternion gRot = ECSComponent::Transform3D::GetGlobalRotation(e);

                c.position = gPos.ToRaylib();

                // Define the default forward vector (0, 0, -1) in Raylib's coordinate system
                Vector3 defaultForward = { 0.0f, 0.0f, -1.0f };

                // Rotate the default forward vector by the Rotation3D quaternion to get the actual forward direction
                Vector3 forward = Vector3RotateByQuaternion(defaultForward, gRot);

                // Set the camera's target to position + forward direction
                c.target = Vector3Add(gPos, forward).ToRaylib();

                // Set the up vector to keep the camera upright (0, 1, 0)
                c.up = { 0.0f, 1.0f, 0.0f };
            }
        );
    }

    void ECSManager::ExecuteQueryDrawCube()
    {
        static flecs::query q = world.query_builder<
            const ECSComponent::CubeComponent,
            const ECSComponent::Transform3D
        >()
        .cached()
        .build();

        q.each([](
            flecs::entity e,
            const ECSComponent::CubeComponent& cube,
            const ECSComponent::Transform3D& tx
            ) {
               Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
               ::DrawCube(gPos.ToRaylib(),
                          cube.width,
                          cube.height,
                          cube.length,
                          cube.color);
            });
    }

    void ECSManager::ExecuteQueryDrawDebugCapsule()
    {
        static flecs::query q = world.query_builder<
            const ECSComponent::DebugCapsuleComponent,
            const ECSComponent::Transform3D
        >()
        .cached()
        .build();

        q.each([](
            flecs::entity e,
            const ECSComponent::DebugCapsuleComponent& capsule,
            const ECSComponent::Transform3D& tx
            ) {
                Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
                ::DrawCapsuleWires(gPos.ToRaylib(),
                    Vector3Add(gPos, { 0.0f, capsule.height, 0.0f }).ToRaylib(),
                    capsule.radius,
                    capsule.slices,
                    capsule.rings,
                    capsule.color
                );
            });
    }

    void ECSManager::ExecuteQueryDrawDebugCube()
    {
        static flecs::query q = world.query_builder<
            const ECSComponent::DebugCubeComponent,
            const ECSComponent::Transform3D
        >()
        .cached()
        .build();

        q.each([](
            flecs::entity e,
            const ECSComponent::DebugCubeComponent& cube,
            const ECSComponent::Transform3D& tx
            ) {
                Vector3 gPos = ECSComponent::Transform3D::GetGlobalPosition(e);
                ::DrawCubeWires(
                    Vector3Add(gPos, { cube.width / 2.0f, cube.height / 2.0f, cube.length / 2.0f }).ToRaylib(),
                    cube.width,
                    cube.height,
                    cube.length,
                    cube.color
                );
            });
    }

    void ECSManager::ExecuteQuerySetCameraAsActive()
    {
        static flecs::query q = world.query_builder<
            ::Camera3D
        >()
        .with<ECSTag::ActiveCamera>()
        .build();

        q.each([](
                ::Camera3D& camera
            ) {
                SetActiveCamera3D(camera);
            });
    }

}
