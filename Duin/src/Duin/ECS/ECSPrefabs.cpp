#include "dnpch.h"
#include "ECSPrefabs.h"
#include "ECSComponents.h"
#include "ECSPhysics.h"
#include "ECSTag.h"
#include "Duin/ECS/PrefabRegistry.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Render/Camera.h"

namespace duin
{

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
    Node = world.Prefab("Node").Add<UUID>();

    Node2D = world.Prefab("Node2D");
    Node2D.IsA(Node)
        .SetPair<ECSComponent::Position2D, ECSTag::Local>(ECSComponent::Position2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Rotation2D, ECSTag::Local>(ECSComponent::Rotation2D{0.0f})
        .SetPair<ECSComponent::Scale2D, ECSTag::Local>(ECSComponent::Scale2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Position2D, ECSTag::Global>(ECSComponent::Position2D{0.0f, 0.0f})
        .SetPair<ECSComponent::Rotation2D, ECSTag::Global>(ECSComponent::Rotation2D{0.0f})
        .SetPair<ECSComponent::Scale2D, ECSTag::Global>(ECSComponent::Scale2D{0.0f, 0.0f});

    Node3D = world.Prefab("Node3D").IsA(Node).Set<ECSComponent::Transform3D>(ECSComponent::Transform3D{});

    PhysicsStaticBody = world.Prefab("PhysicsStaticBody")
                            .IsA(Node3D)
                            .Add<ECSTag::PxStatic>()
                            .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f});

    PhysicsKinematicBody = world.Prefab("PhysicsKinematicBody")
                               .IsA(Node3D)
                               .Add<ECSTag::PxKinematic>()
                               .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f});

    PhysicsDynamicBody = world.Prefab("PhysicsDynamicBody")
                             .IsA(Node3D)
                             .Add<ECSTag::PxDynamic>()
                             .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f});

    PhysicsCharacterBody = world.Prefab("PhysicsCharacterBody")
                               .IsA(Node3D)
                               .Add<ECSTag::PxKinematic>()
                               .Set<ECSComponent::Velocity3D>(ECSComponent::Velocity3D{0.0f, 0.0f, 0.0f});

    Camera3D = world.Prefab("Camera3D")
                   .IsA(Node3D)
                   .Set<Camera>(Camera{
                       /* .uuid =  */ UUID(),
                       /* .position =  */ {0.0f, 0.0f, 0.0f},
                       /* .target =  */ {0.0f, 0.0f, 0.0f},
                       /* .up =  */ {0.0f, 1.0f, 0.0f},
                       /* .fovy =  */ 72.0f
                       // .projection = ::CAMERA_PERSPECTIVE
                   });

    Cube = world.Prefab("Cube").IsA(Node3D);
    // TODO REPLACE_RAYLIB
    // .Set<ECSComponent::CubeComponent>({...})

    DebugCapsule = world.Prefab("DebugCapsule").IsA(Node3D);
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

} // namespace duin
