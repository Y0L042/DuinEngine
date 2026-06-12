options gen2

module ecs_systems shared public

require math

require dn_ecs
require dn_ecs_core
require dn_ecs_boost
require dn_ecs_components
require dn_renderer
require dn_gameworld
require dn_log
require dn_input
require dn_camera

require ecs_components

// Queries are created lazily per call site by the ecs_query macro and cached
// in dn_ecs; DnGameWorld.fini destroys them. No construct/destruct needed.

// ---------------------------------------------------------------------------
// System: integrate velocity into transform
// ---------------------------------------------------------------------------

def system_integrate_velocity(world : ecs_world_t?; delta : double) : void {
    ecs_query(world) $ [CACHED] (vel : Velocity3D; var tx : Transform3D) {
        tx.pos += vel.val * float(delta)
    }
}

// ---------------------------------------------------------------------------
// System: render cubes
// ---------------------------------------------------------------------------

def system_draw_rendercube(var game_world : DnGameWorld?) : void {
    var world = game_world.ecs_world()
    ecs_query(world) $ [CACHED, REQUIRE(RenderCube)] (eid : ecs_entity_t; tx : Transform3D) {
        let ent = DnEntity(world = world, entity = eid)
        let pos : float3 = game_world.get_global_position(ent)
        dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
    }
}

// ---------------------------------------------------------------------------
// System: player input → PlayerMoveInput + CameraYaw/Pitch
// ---------------------------------------------------------------------------

def system_player_input(world : ecs_world_t?; var camera : DnCamera?) : void {
    let mouse_sensitivity = 0.0015f
    let MAX_PITCH = 87.0f * (PI / 180.0f)

    let md = dn_get_mouse_delta()

    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer)] (speed : MoveSpeed; var input : PlayerMoveInput; var yaw : CameraYaw; var pitch : CameraPitch) {
        // --- Mouse look ---
        yaw.radians += md.x * mouse_sensitivity

        let new_pitch = pitch.radians + (-md.y * mouse_sensitivity)
        pitch.radians = clamp(new_pitch, pitch.min_rad, pitch.max_rad)

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
        let move_speed = is_sprinting ? speed.sprint : speed.walk
        input.dir = move * move_speed
    }
}

// ---------------------------------------------------------------------------
// System: gravity accumulation into Velocity.y
// ---------------------------------------------------------------------------

def system_player_gravity(world : ecs_world_t?; delta : double) : void {
    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer)] (grav : GravityScale; var vel : Velocity3D) {
        vel.val.y += grav.value * float(delta)
    }
}
