#include "dnpch.h"
#include "ECSComponents.h"
#include "ECSPhysics.h"
#include "Duin/ECS/ComponentInspector.h"
#include "Duin/Render/Camera.h"

namespace duin
{

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


    // TODO Delete this! This is all handled editor-side
    // Register components with the inspector (idempotent — skips if already registered)
    auto &inspector = duin::ComponentInspector::Get();
    inspector.RegisterComponent<ECSComponent::Position2D>("Position2D");
    inspector.RegisterComponent<ECSComponent::Rotation2D>("Rotation2D");
    inspector.RegisterComponent<ECSComponent::Scale2D>("Scale2D");
    inspector.RegisterComponent<ECSComponent::Velocity2D>("Velocity2D");
    inspector.RegisterComponent<ECSComponent::Transform3D>("Transform3D");
    inspector.RegisterComponent<ECSComponent::Position3D>("Position3D");
    inspector.RegisterComponent<ECSComponent::Rotation3D>("Rotation3D");
    inspector.RegisterComponent<ECSComponent::Scale3D>("Scale3D");
    inspector.RegisterComponent<ECSComponent::Velocity3D>("Velocity3D");
    inspector.RegisterComponent<ECSComponent::CubeComponent>("CubeComponent");
    inspector.RegisterComponent<ECSComponent::StaticBodyComponent>("StaticBodyComponent");
    inspector.RegisterComponent<ECSComponent::KinematicBodyComponent>("KinematicBodyComponent");
    inspector.RegisterComponent<ECSComponent::DynamicBodyComponent>("DynamicBodyComponent");
    inspector.RegisterComponent<ECSComponent::CharacterBodyComponent>("CharacterBodyComponent");
    inspector.RegisterComponent<ECSComponent::DebugCapsuleComponent>("DebugCapsuleComponent");
    inspector.RegisterComponent<ECSComponent::DebugCubeComponent>("DebugCubeComponent");
    inspector.RegisterComponent<Camera>("Camera");
}
} // namespace ECSComponent

} // namespace duin
