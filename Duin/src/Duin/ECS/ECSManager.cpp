#include "dnpch.h"
#include "ECSManager.h"
#include "Duin/Core/Application.h"



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
            world.component<ECSComponent::Position3D>();
            world.component<ECSComponent::Rotation3D>();
            world.component<ECSComponent::Scale3D>();
            world.component<ECSComponent::Velocity3D>();
            world.component<ECSComponent::CharacterBody3DComponent>();
            world.component<::Camera3D>();

            world.component<ECSComponent::DebugCapsuleDesc>();
        }
    }

    namespace ECSPrefab {
        /* --- Prefabs --- */
        flecs::entity Node;
        flecs::entity Node2D;
        flecs::entity Node3D;
        flecs::entity CharacterBody3D;
        flecs::entity Camera3D;
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
                .set<ECSComponent::Position3D,  ECSTag::Local>({ 0.0f, 0.0f, 0.0f })
                .set<ECSComponent::Rotation3D,  ECSTag::Local>({ 0.0f, 0.0f, 0.0f, 1.0f })
                .set<ECSComponent::Scale3D,     ECSTag::Local>({ 1.0f, 1.0f, 1.0f })

                .set<ECSComponent::Position3D,  ECSTag::Global>({ 0.0f, 0.0f, 0.0f })
                .set<ECSComponent::Rotation3D,  ECSTag::Global>({ 0.0f, 0.0f, 0.0f, 1.0f })
                .set<ECSComponent::Scale3D,     ECSTag::Global>({ 1.0f, 1.0f, 1.0f })
                ;



            CharacterBody3D = world.prefab("CharacterBody3D")
                .is_a(ECSPrefab::Node3D)
                .set<ECSComponent::Velocity3D>({ 0.0f, 0.0f, 0.0f })
                .set<ECSComponent::CharacterBody3DComponent>({ nullptr });
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

            

            DebugCapsule = world.prefab("DebugCapsule")
                .is_a(ECSPrefab::Node3D)
                .set<ECSComponent::DebugCapsuleDesc>({
                    1.75f,
                    1.0f,
                    8, 16,
                    ::GREEN
                    })
                ;
                
        }
    }


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

	void ECSManager::InitializeRemoteExplorer()
	{}

    void ECSManager::ExecuteQuerySetCameraAsActive(flecs::world& world)
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

    void ECSManager::ExecuteCharacterBody3DCreation(PhysXManager& manager)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::CharacterBody3DComponent,
            const ECSComponent::Position3D
        >()
        .term_at(1).second<ECSTag::Global>()
        .cached()
        .build();
         
        q.each([&manager](
            ECSComponent::CharacterBody3DComponent& cb,
            const ECSComponent::Position3D& p
        ) {
        });
    }

    void ECSManager::ExecuteQueryUpdatePosition3D(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::Position3D,
            const ECSComponent::Velocity3D
        >()
        .term_at(0).second<ECSTag::Local>()
        .cached()
        .build();

        q.each([](
            ECSComponent::Position3D& p,
            const ECSComponent::Velocity3D& v
            ) {
                duin::Vector3 vDelta = duin::Vector3Scale(v.value, ::GetFrameTime());
                p.value = duin::Vector3Add(p.value, vDelta);
                //p.value = duin::Vector3Add(p.value, v.value);
            });
    }

    void ECSManager::ExecuteQueryHierarchicalUpdatePosition3D(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::Position3D,
            const ECSComponent::Position3D, 
            const ECSComponent::Position3D * 
        >()
        .term_at(0).second<ECSTag::Global>()
        .term_at(1).second<ECSTag::Local>()
        .term_at(2).second<ECSTag::Global>()
        .term_at(2).parent().cascade()
        .cached()
        .build();

        q.each([](
               ECSComponent::Position3D& global_pos,
               const ECSComponent::Position3D& local_pos, 
               const ECSComponent::Position3D* parent_global_pos
            ) {
                global_pos.value = local_pos.value;
                if (parent_global_pos) {
                    global_pos.value += parent_global_pos->value;
                }
            });
    }

    void ECSManager::ExecuteQueryUpdateRotation3D(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::Rotation3D,
            const ECSComponent::Rotation3D
        >()
            .term_at(0).second<ECSTag::Local>()
            .cached()
            .build();

        q.each([](
            ECSComponent::Rotation3D& p,
            const ECSComponent::Rotation3D& v
            ) {
 
            });
    }

    void ECSManager::ExecuteQueryHierarchicalUpdateRotation3D(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::Rotation3D,
            const ECSComponent::Rotation3D,
            const ECSComponent::Rotation3D*
        >()
            .term_at(0).second<ECSTag::Local>()
            .term_at(1).second<ECSTag::Global>()
            .term_at(2).second<ECSTag::Global>()
            .term_at(1).parent().cascade()
            .cached()
            .build();

        q.each([](
            ECSComponent::Rotation3D& global_pos,
            const ECSComponent::Rotation3D& local_pos,
            const ECSComponent::Rotation3D* parent_global_pos
            ) {

                if (parent_global_pos) {

                }
            });
    }

    void ECSManager::ExecuteQueryUpdateCharacterBodyPosition(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ECSComponent::CharacterBody3DComponent,
            ECSComponent::Position3D,
            const ECSComponent::Velocity3D
        >()
        .term_at(1).second<ECSTag::Local>()
        .cached()
        .build();

        world.defer_begin();
            q.each([this](
                flecs::entity e,
                ECSComponent::CharacterBody3DComponent& cb,
                ECSComponent::Position3D& global_pos,
                const ECSComponent::Velocity3D& v
                ) {
                    Vector3 vDelta = Vector3Scale(v.value, ::GetFrameTime());
                    cb.characterBody->Move(vDelta, (double)GetFrameTime());
                    Vector3 bodyPos = duin::Vector3(cb.characterBody->controller->getPosition());
                    SetGlobalPosition3D(e, bodyPos);
                });
        world.defer_end();
    }

    void ECSManager::ExecuteQueryUpdateCameraPosition(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            ::Camera3D,
            const ECSComponent::Position3D
        >()
        .term_at(1).second<ECSTag::Global>()
        .cached()
        .build();

        q.each([](
                ::Camera3D& camera,
                const ECSComponent::Position3D& pos
            ) {
                camera.position = pos.value.ToRaylib();
            });
    }

    void ECSManager::ExecuteQueryDrawDebugCapsule(flecs::world& world)
    {
        static flecs::query q = world.query_builder<
            const ECSComponent::DebugCapsuleDesc,
            const ECSComponent::Position3D
        >()
            .term_at(1).second<ECSTag::Global>()
            .cached()
            .build();

        q.each([](
            const ECSComponent::DebugCapsuleDesc& capsule,
            const ECSComponent::Position3D& pos
            ) {
                ::DrawCapsuleWires(pos.value.ToRaylib(),
                    Vector3Add(pos.value, { 0.0f, capsule.height, 0.0f }).ToRaylib(),
                    capsule.radius,
                    capsule.slices,
                    capsule.rings,
                    capsule.color
                );
            });
    }
}
