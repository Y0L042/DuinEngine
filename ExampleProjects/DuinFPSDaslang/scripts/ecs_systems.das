options gen2

module ecs_systems shared public

require math

require dn_ecs
require dn_ecs_core
require dn_ecs_components
require dn_renderer
require dn_gameworld
require dn_log
require dn_input
require dn_camera

require ecs_components

// ---------------------------------------------------------------------------
// Queries
// ---------------------------------------------------------------------------

var query_integrate_velocity : DnQuery = DnQuery()
var query_render_cube        : DnQuery = DnQuery()
var query_player_input       : DnQuery = DnQuery()
var query_player_camera      : DnQuery = DnQuery()
var query_player_gravity     : DnQuery = DnQuery()
var query_player_jump        : DnQuery = DnQuery()

def construct_queries(var world : DnWorld&) : void {
    query_integrate_velocity = (world.query_builder()
        .with_term(type<Transform3D>)
        .with_term(type<Velocity3D>)
        .cached()
        .build())

    query_render_cube = (world.query_builder()
        .with_term(type<RenderCube>)
        .with_term(type<Transform3D>)
        .cached()
        .build())

    query_player_input = (world.query_builder()
        .with_term(type<IsPlayer>)
        .with_term(type<PlayerMoveInput>)
        .with_term(type<MoveSpeed>)
        .with_term(type<CameraYaw>)
        .with_term(type<CameraPitch>)
        .cached()
        .build())

    query_player_jump = (world.query_builder()
        .with_term(type<IsPlayer>)
        .with_term(type<Velocity3D>)
        .with_term(type<JumpImpulse>)
        .cached()
        .build())

    query_player_gravity = (world.query_builder()
        .with_term(type<IsPlayer>)
        .with_term(type<Velocity3D>)
        .with_term(type<GravityScale>)
        .cached()
        .build())
}

def destruct_queries() : void {
    query_integrate_velocity.fini()
    query_render_cube.fini()
    query_player_input.fini()
    query_player_gravity.fini()
}

// ---------------------------------------------------------------------------
// System: integrate velocity into transform
// ---------------------------------------------------------------------------

def system_integrate_velocity(world : ecs_world_t?; delta : double) : void {
    var it = ecs_query_iter(world, query_integrate_velocity.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            var tx = ecs_field_get(it, type<Transform3D>, 0)
            var v  = ecs_field_get(it, type<Velocity3D>,  1)
            for (i in range(it.count)) {
                tx[i].pos += v[i].val * float(delta)
            }
        }
    }
}

// ---------------------------------------------------------------------------
// System: render cubes
// ---------------------------------------------------------------------------

def system_draw_rendercube(var game_world : DnGameWorld?) : void {
    assume world = game_world.ecs_world()
    var it = ecs_query_iter(world, query_render_cube.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            for (i in range(it.count)) {
                let eid = it.entities[i]
                let ent = DnEntity(world = world, entity = eid)
                let pos : float3 = game_world.get_global_position(ent)
                dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
            }
        }
    }
}

// ---------------------------------------------------------------------------
// System: player input → PlayerMoveInput + CameraYaw/Pitch
// ---------------------------------------------------------------------------

def system_player_input(world : ecs_world_t?; var camera : DnCamera?) : void {
    let mouse_sensitivity = 0.0015f
    let MAX_PITCH = 87.0f * (PI / 180.0f)

    let md = dn_get_mouse_delta()

    var it = ecs_query_iter(world, query_player_input.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            var input = ecs_field_get(it, type<PlayerMoveInput>, 1)
            var speed = ecs_field_get(it, type<MoveSpeed>,       2)
            var yaw   = ecs_field_get(it, type<CameraYaw>,       3)
            var pitch = ecs_field_get(it, type<CameraPitch>,     4)

            for (i in range(it.count)) {
                // --- Mouse look ---
                yaw[i].radians += md.x * mouse_sensitivity

                let new_pitch = pitch[i].radians + (-md.y * mouse_sensitivity)
                pitch[i].radians = clamp(new_pitch, pitch[i].min_rad, pitch[i].max_rad)

                if (camera != null) {
                    camera.yaw(md.x * mouse_sensitivity)
                    camera.pitch(-md.y * mouse_sensitivity)
                }

                // --- WASD wish-dir ---
                var move = float3(0.0f, 0.0f, 0.0f)
                if (camera != null) {
                    let fwd     = camera.forward()
                    let rgt     = camera.right()
                    let h_fwd   = normalize(float3(fwd.x, 0.0f, fwd.z))
                    let h_right = normalize(float3(rgt.x, 0.0f, rgt.z))

                    if (dn_is_input_action_triggered("Player_MoveForward"))  { move += h_fwd; }
                    if (dn_is_input_action_triggered("Player_MoveBackward")) { move -= h_fwd; }
                    if (dn_is_input_action_triggered("Player_StrafeRight"))  { move -= h_right; }
                    if (dn_is_input_action_triggered("Player_StrafeLeft"))   { move += h_right; }
                }

                let is_sprinting = dn_is_input_action_triggered("Player_Sprint")
                let move_speed = is_sprinting ? speed[i].sprint : speed[i].walk
                input[i].dir = move * move_speed
            }
        }
    }
}

// ---------------------------------------------------------------------------
// System: gravity accumulation into Velocity.y
// ---------------------------------------------------------------------------

def system_player_gravity(world : ecs_world_t?; delta : double) : void {
    var it = ecs_query_iter(world, query_player_gravity.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            var vel     = ecs_field_get(it, type<Velocity3D>,     1)
            var gravity = ecs_field_get(it, type<GravityScale>, 2)
            for (i in range(it.count)) {
                vel[i].val.y += gravity[i].value * float(delta)
            }
        }
    }
}
