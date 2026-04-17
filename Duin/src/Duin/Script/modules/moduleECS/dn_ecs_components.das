options gen2

module dn_ecs_components shared

struct Position2D {
    val : float2
}

struct Rotation2D {
    val : float
}

struct Scale2D {
    val : float2
}

struct Velocity2D {
    val : float2
}

struct Transform3D {
    pos : float3
    rot : float3
    scale : float3
}

struct Position3D {
    val : float3
}

struct Rotation3D {
    val : float3
}

struct Scale3D {
    val : float3
}

struct Velocity3D {
    val : float3
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