options gen2
options rtti

module ecs_components shared public

require daslib/rtti
require dn_ecs

// --- Rendering ---

struct RenderCube {
    size : float3
}

// --- Player movement ---

struct PlayerMoveInput {
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

// --- Tags ---

struct OnGround {}      // present when character is on the ground
struct CanSprint {}     // entity is allowed to sprint
struct CanJump {}       // entity is allowed to jump
struct IsPlayer {}      // marks the player entity for query targeting

// --- Registration ---
 
def register_components(var world : DnWorld&) : void {
    world.dn_register_component(type<RenderCube>)

    world.dn_register_component(type<PlayerMoveInput>)
    world.dn_register_component(type<MoveSpeed>)
    world.dn_register_component(type<JumpImpulse>)
    world.dn_register_component(type<GravityScale>)

    world.dn_register_component(type<CameraYaw>)
    world.dn_register_component(type<CameraPitch>)

    world.dn_register_component(type<CharacterBodyDesc>)

    world.dn_register_component(type<OnGround>)
    world.dn_register_component(type<CanSprint>)
    world.dn_register_component(type<CanJump>)
    world.dn_register_component(type<IsPlayer>)
}
