options gen2

module dn_ecs_components shared

require dn_ecs public

// 2D

struct DnTransform2D {
    pos : float2
    rot : float2
    scale : float2
}

struct DnVelocity2D {
    lin : float2
    rot : float2
}




// 3D

struct DnTransform3D {
    pos : float3
    rot : float4
    scale : float3
}

struct DnVelocity3D {
    lin : float3
    rot : float4
}

// Marker components
struct DnCubeComponent {}
struct DnStaticBodyComponent {}
struct DnKinematicBodyComponent {}
struct DnDynamicBodyComponent {}
struct DnCharacterBodyComponent {}
struct DnPhysicsStaticCubeComponent {}
struct DnDebugCapsuleComponent {}
struct DnDebugCubeComponent {}

// struct DnCamera {}

def dn_register_internal_components(var world: DnWorld&) : void {
    world.dn_register_component(type<DnTransform2D>)
    world.dn_register_component(type<DnVelocity2D>)

    world.dn_register_component(type<DnTransform3D>)
    world.dn_register_component(type<DnVelocity3D>)
}
