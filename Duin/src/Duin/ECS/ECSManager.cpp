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
        world.component<ECSComponent::Position3D>();
        world.component<ECSComponent::Rotation3D>();
        world.component<ECSComponent::Scale3D>();
        world.component<ECSComponent::Velocity3D>();

        world.component<ECSComponent::CubeComponent>();
        world.component<ECSComponent::CharacterBody3DComponent>();
        world.component<ECSComponent::PhysicsStaticCubeComponent>();
        world.component<::Camera3D>();

        world.component<ECSComponent::DebugCapsuleComponent>();
        world.component<ECSComponent::DebugCubeComponent>();
    }
}

namespace ECSPrefab {
    /* --- Prefabs --- */
    flecs::entity Node;
    flecs::entity Node2D;
    flecs::entity Node3D;
    flecs::entity PhysicsCharacterBody3D;
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
            .set<ECSComponent::Position3D,  ECSTag::Local>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::Rotation3D,  ECSTag::Local>({ 0.0f, 0.0f, 0.0f, 1.0f })
            .set<ECSComponent::Scale3D,     ECSTag::Local>({ 1.0f, 1.0f, 1.0f })

            .set<ECSComponent::Position3D,  ECSTag::Global>({ 0.0f, 0.0f, 0.0f })
            .set<ECSComponent::Rotation3D,  ECSTag::Global>({ 0.0f, 0.0f, 0.0f, 1.0f })
            .set<ECSComponent::Scale3D,     ECSTag::Global>({ 1.0f, 1.0f, 1.0f })
            ;



        PhysicsCharacterBody3D = world.prefab("CharacterBody3D")
            .is_a(ECSPrefab::Node3D)
            .add<ECSTag::PxKinematic>()
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
    ExecuteQueryUpdateCharacterBody3DPosition();
    //ExecuteQueryUpdatePosition3D();
    ExecuteQueryHierarchicalUpdatePosition3D();

    ExecuteQueryUpdateRotation3D();
    ExecuteQueryHierarchicalUpdateRotation3D();


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

void ECSManager::ExecuteCharacterBody3DCreation(Physics3DServer& server)
{
    static flecs::query q = world.query_builder<
        ECSComponent::CharacterBody3DComponent,
        const ECSComponent::Position3D
    >()
    .term_at(1).second<ECSTag::Global>()
    .cached()
    .build();
     
    q.each([&server](
        ECSComponent::CharacterBody3DComponent& cb,
        const ECSComponent::Position3D& p
    ) {
    });
}

void ECSManager::ExecuteQueryUpdatePosition3D()
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
            duin::Vector3 vDelta = duin::Vector3Scale(v.value, GetPhysicsFrameTime());
            p.value = duin::Vector3Add(p.value, vDelta);
        });
}

void ECSManager::ExecuteQueryHierarchicalUpdatePosition3D()
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

void ECSManager::ExecuteQueryUpdateRotation3D()
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
            // TODO
        });
}

void ECSManager::ExecuteQueryHierarchicalUpdateRotation3D()
{
    static flecs::query q = world.query_builder<
        ECSComponent::Rotation3D,
        const ECSComponent::Rotation3D,
        const ECSComponent::Rotation3D*
    >()
        .term_at(0).second<ECSTag::Global>()
        .term_at(1).second<ECSTag::Local>()
        .term_at(2).second<ECSTag::Global>()
        .term_at(2).parent().cascade()
        .cached()
        .build();

    q.each([](
            ECSComponent::Rotation3D& global_rot,
            const ECSComponent::Rotation3D& local_rot,
            const ECSComponent::Rotation3D *parent_global_rot
        ) {
            global_rot.value = local_rot.value;
            if (parent_global_rot) {
                global_rot.value = QuaternionMultiply(parent_global_rot->value, local_rot.value);
                global_rot.value = QuaternionNormalize(global_rot.value);
            }
        });
}

void ECSManager::ExecuteQueryUpdateCharacterBody3DPosition()
{
    static flecs::query q = world.query_builder<
        ECSComponent::CharacterBody3DComponent,
        ECSComponent::Position3D,
        const ECSComponent::Position3D,
        ECSComponent::Velocity3D
    >()
        .term_at(1).second<ECSTag::Local>()
        .term_at(2).second<ECSTag::Global>()
        .with<ECSTag::PxKinematic>()
        .cached()
        .build();
    q.each([](
            flecs::entity e,
            ECSComponent::CharacterBody3DComponent& cb,
            ECSComponent::Position3D& localPos,
            const ECSComponent::Position3D& globalPos,
            ECSComponent::Velocity3D& velocity
        ) {
            // Move CharacterBody3D and get physics-resolved global position
            double delta = GetPhysicsFrameTime();
            Vector3 vDelta = Vector3Scale(velocity.value, GetPhysicsFrameTime());

            Vector3 oldPos = cb.characterBody->GetFootPosition();
            cb.characterBody->Move(vDelta, delta);
            Vector3 newPos = cb.characterBody->GetFootPosition();

            // add distance between current globalPos (old) and pxBody global pos (new) to localPos
            localPos.value = Vector3Add(Vector3Subtract(newPos, globalPos.value), localPos.value);

            velocity.value = cb.characterBody->GetCurrentVelocity();

            // If autostep has happened, cap Y velocity
            Vector3 distanceMoved = Vector3Subtract(newPos, oldPos);
            static const float AUTOSTEP_FACTOR = 5.0f;
            if (std::abs(distanceMoved.y) > std::abs(vDelta.y * AUTOSTEP_FACTOR)) {
                velocity.value.y = vDelta.y;
            } 
        });
}

void ECSManager::ExecuteQueryUpdateCameraPosition()
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

void ECSManager::ExecuteQueryControlCamera()
{
    static flecs::query q = world.query_builder<
        ::Camera3D, 
        const ECSComponent::Position3D, 
        const ECSComponent::Rotation3D
    >()
    .term_at(1).second<ECSTag::Global>()
    .term_at(2).second<ECSTag::Global>()
    .cached()
    .build();

    q.each([](
            flecs::iter& it, 
            size_t index,
            Camera3D& c, 
            const ECSComponent::Position3D& p, 
            const ECSComponent::Rotation3D& r
        ) {
            c.position = p.value.ToRaylib();

            // Define the default forward vector (0, 0, -1) in Raylib's coordinate system
            Vector3 defaultForward = { 0.0f, 0.0f, -1.0f };

            // Rotate the default forward vector by the Rotation3D quaternion to get the actual forward direction
            Vector3 forward = Vector3RotateByQuaternion(defaultForward, r.value);

            // Set the camera's target to position + forward direction
            c.target = Vector3Add(p.value, forward).ToRaylib();

            // Set the up vector to keep the camera upright (0, 1, 0)
            c.up = { 0.0f, 1.0f, 0.0f };
        }
    );
}

void ECSManager::ExecuteQueryDrawCube()
{
    static flecs::query q = world.query_builder<
        const ECSComponent::CubeComponent,
        const ECSComponent::Position3D
    >()
    .term_at(1).second<ECSTag::Global>()
    .cached()
    .build();

    q.each([](
        const ECSComponent::CubeComponent& cube,
        const ECSComponent::Position3D& pos
        ) {
           ::DrawCube(pos.value.ToRaylib(),
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
        const ECSComponent::Position3D
    >()
    .term_at(1).second<ECSTag::Global>()
    .cached()
    .build();

    q.each([](
        const ECSComponent::DebugCapsuleComponent& capsule,
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

void ECSManager::ExecuteQueryDrawDebugCube()
{
    static flecs::query q = world.query_builder<
        const ECSComponent::DebugCubeComponent,
        const ECSComponent::Position3D
    >()
    .term_at(1).second<ECSTag::Global>()
    .cached()
    .build();

    q.each([](
        const ECSComponent::DebugCubeComponent& cube,
        const ECSComponent::Position3D& pos
        ) {
            ::DrawCubeWires(
                Vector3Add(pos.value, { cube.width / 2.0f, cube.height / 2.0f, cube.length / 2.0f }).ToRaylib(),
                cube.width,
                cube.height,
                cube.length,
                cube.color
            );
        });
}

}
