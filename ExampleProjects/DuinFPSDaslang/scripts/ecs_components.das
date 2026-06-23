options gen2
options rtti

module ecs_components shared public

require daslib/rtti

require dn_ecs
require dn_camera
require dn_physics

// --- Rendering ---

struct RenderCube {
    size : float3
}

// --- Movement ---

struct MoveInput {
    dir : float3        // normalised horizontal wish-dir, set each frame by input system
}

struct MoveSpeed {
    walk   : float
    sprint : float
}

struct JumpImpulse {
    value : float
}
 
struct GravityScale {
    value : float       // m/s^2, negative = downward (e.g. -9.81)
}

struct ExternVelocity {
    val : float3
}

struct GroundFriction {
    coef : float = 0.2f
}

struct AirFriction {
    coef : float = 0.05f
}

// --- Camera state stored on the player entity ---

struct CameraYaw {
    radians : float
}
 
struct CameraPitch {
    radians : float
    min_rad : float
    max_rad : float
}

struct CharacterBodyDesc {
    x : int
}

struct CharacterBodyHandle {
    cb : DnCharacterBody?
}

struct CameraHandle {
    handle : DnCamera?
}


// --- Tags ---

struct OnGround {}      // present when character is on the ground
struct CanSprint {}     // entity is allowed to sprint
struct CanJump {}       // entity is allowed to jump
struct IsPlayer {}      // marks the player entity for query targeting
struct ActiveCamera {}

// --- Registration ---
 
def register_components(var world : DnWorld&) : void {
    world.dn_register_component(type<RenderCube>)

    world.dn_register_component(type<MoveInput>)
    world.dn_register_component(type<MoveSpeed>)
    world.dn_register_component(type<JumpImpulse>)
    world.dn_register_component(type<GravityScale>)
    world.dn_register_component(type<ExternVelocity>)
    world.dn_register_component(type<GroundFriction>)
    world.dn_register_component(type<AirFriction>)

    world.dn_register_component(type<CameraYaw>)
    world.dn_register_component(type<CameraPitch>)
    world.dn_register_component(type<CameraHandle>)
    world.dn_register_component(type<ActiveCamera>)

    world.dn_register_component(type<CharacterBodyDesc>)
    world.dn_register_component(type<CharacterBodyHandle>)
 
    world.dn_register_component(type<OnGround>)
    world.dn_register_component(type<CanSprint>)
    world.dn_register_component(type<CanJump>)
    world.dn_register_component(type<IsPlayer>)
}
