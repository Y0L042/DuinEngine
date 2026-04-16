options gen2

module dn_ecs_components shared

struct Position2D {
    value : float2
}

struct Rotation2D {
    value : float
}

struct Scale2D {
    value : float2
}

struct Velocity2D {
    value : float2
}

struct Transform3D {
    position : float3
    rotation : float3
    scale : float3
}

struct Position3D {
    value : float3
}

struct Rotation3D {
    value : float3
}

struct Scale3D {
    value : float3
}

struct Velocity3D {
    value : float3
}

// Marker components
struct CubeComponent {}

struct StaticBodyComponent {}

struct KinematicBodyComponent {}

struct DynamicBodyComponent {}

struct CharacterBodyComponent {}

struct PhysicsStaticCubeComponent {}

struct DebugCapsuleComponent {}

struct DebugCubeComponent {}

struct Camera {}