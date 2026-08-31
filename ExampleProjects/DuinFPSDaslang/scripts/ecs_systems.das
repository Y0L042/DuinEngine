options gen2

module ecs_systems shared public

// Daslib
require math
require daslib/option

// Bindings
require flecs
require dn_ecs
require dn_renderer
require dn_log
require dn_input
require dn_camera
require dn_physics

// Game
require ecs_components
require dn_ecs_components

// Binding Macros
require dn_ecs_boost


def system_player_input(var world : DnWorld; var camera : DnCamera?) : void {
    let mouse_sensitivity = 0.0015f
    let MAX_PITCH = 87.0f * (PI / 180.0f)

    let md = dn_get_mouse_delta()

    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer)] (speed : MoveSpeed; var input : MoveInput; var yaw : CameraYaw; var pitch : CameraPitch) {
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

        input.dir = move
    }
}

// def system_player_sprint()

def system_player_jump(var world : DnWorld, delta : double) : void {
    var wp : ecs_world_t? = world.ecs_world()
    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer)] (e : ecs_entity_t, var ji : JumpImpulse, var vel : DnVelocity3D) {
        vel.lin.y += ji.value
        wp.ecs_defer_begin()
        var jid = world.dn_register_component(type<JumpImpulse>)
        ecs_remove(wp, e, jid)
        wp.ecs_defer_end()
    }
}

def system_set_input_velocity(var world : DnWorld, delta : double) : void {
    world.ecs_query() $ [CACHED, REQUIRE(IsPlayer)] (var input : MoveInput, speed : MoveSpeed, var vel : DnVelocity3D) {
        vel.lin = normalize(input.dir) * speed.walk * float(delta)
        input.dir = float3(0f, 0f, 0f)
    }
}

def system_gravity(var world : DnWorld; delta : double) : void {
    ecs_query(world) $ [CACHED] (grav : GravityScale; var vel : DnVelocity3D) {
        vel.lin.y += grav.value * float(delta)
    }
}

def system_ground_friction(var world : DnWorld, delta : double) : void {
    ecs_query(world) $ [CACHED] (fric : GroundFriction, var exvel : ExternVelocity) {
        exvel.val = lerp(exvel.val, float3(0f, 0f, 0f), fric.coef * float(delta))
    }
}

def system_air_friction(var world : DnWorld, delta : double) : void {
    ecs_query(world) $ [CACHED] (fric : AirFriction, var exvel : ExternVelocity) {
        exvel.val = lerp(exvel.val, float3(0f, 0f, 0f), fric.coef * float(delta))
    }
}

def system_resolve_external_velocities(var world : DnWorld, delta : double) : void {
    ecs_query(world) $ [CACHED] (var exvel : ExternVelocity, var vel : DnVelocity3D) {
        vel.lin += exvel.val
        exvel.val = float3(0f, 0f, 0f)
    }
}

def system_characterbody_move(var world : DnWorld, delta : double) :  void {
    ecs_query(world) $ [CACHED] (var cbh : CharacterBodyHandle, var vel : DnVelocity3D) {
        cbh.cb.move(vel.lin, delta)
    }
}

def system_update_position(var world : DnWorld, delta : double) :  void {
    ecs_query(world) $ [CACHED] (var cbh : CharacterBodyHandle, var tx : DnTransform3D) {
        tx.pos = cbh.cb.get_position()
    }
}

// Camera pos is global, so needs to sum with parents' positions
def system_update_active_camera_position(var world : DnWorld, delta : double) : void {
    ecs_query(world) $ [CACHED] (e: DnEntity, tx : DnTransform3D, var cam : CameraHandle) {
        if (cam.handle != null) {
            if (e.parent() |> is_some()) {
                var parent = e.parent().move_unwrap()
                var p_gpos = get_global_position(parent)
                if (p_gpos |> is_some()) {
                    dn_info("Parent gpos: {p_gpos.unwrap()}")
                } else {
                    var ptx : DnTransform3D? = parent.get_mut(type<DnTransform3D>)
                    if (ptx != null) {
                        dn_info("ptx {ptx.pos}")
                    } else {
                        dn_warn("Camera parent has no local TX")
                    }
                    dn_warn("Camera parent has NO POSITION")
                }
            } else {
                dn_warn("Camera has NO PARENT!")
            }
            let pos = get_global_position(e).move_unwrap()
            dn_info("GPOS: {pos.x}, {pos.y}, {pos.z}")
            cam.handle.move_to(pos)
        } else {
            dn_warn("Cam handle is NULL!")
        }
    }
}

def system_draw_rendercube(var world : DnWorld) : void {
    ecs_query(world) $ [CACHED, REQUIRE(RenderCube)] (ent : DnEntity, tx : DnTransform3D) {
        let pos : float3 = get_global_position(world, ent)
        dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
    }
}

// [ecs_system(PHYSICSUPDATE::POST)]
// def update_position(world : ecs_world_t?, delta : double) : void {
//     tx.pos = cbh.cb.get_position()
// }

