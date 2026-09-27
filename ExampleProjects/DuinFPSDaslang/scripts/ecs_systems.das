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

// Syncs the OnGround tag from the character body so gravity and jump can branch on it.
def system_update_ground_state(var world : DnWorld, delta : double) : void {
    var wp : ecs_world_t? = world.ecs_world()
    var gid = world.dn_register_component(type<OnGround>)
    ecs_query(world) $ [CACHED] (e : ecs_entity_t, var cbh : CharacterBodyHandle) {
        if (cbh.cb == null) {
            return
        }
        let grounded = cbh.cb.is_on_floor()
        wp.ecs_defer_begin()
        if (grounded) {
            ecs_add(wp, e, gid)
        } else {
            ecs_remove(wp, e, gid)
        }
        wp.ecs_defer_end()
    }
}

// Jump goes through ExternVelocity rather than writing DnVelocity3D directly:
// system_set_input_velocity assigns the horizontal components wholesale, and
// ExternVelocity is resolved after that, so the impulse survives.
def system_player_jump(var world : DnWorld, delta : double) : void {
    if (!dn_is_input_action_triggered("Player_Jump")) {
        return
    }

    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer), REQUIRE(OnGround)] (var ji : JumpImpulse, var vel : DnVelocity3D, var exvel : ExternVelocity) {
        // Cancel the small downward velocity gravity accumulated while grounded, so
        // jump height does not depend on how long the player stood still.
        vel.lin.y = 0f
        exvel.val.y += ji.value
    }
}

// Only the horizontal plane is driven directly by input. vel.lin.y is left alone so
// gravity can integrate across frames -- overwriting it here would reset the fall
// speed every tick and make the character drift down at a constant crawl.
def system_set_input_velocity(var world : DnWorld, delta : double) : void {
    world.ecs_query() $ [CACHED, REQUIRE(IsPlayer)] (var input : MoveInput, speed : MoveSpeed, var vel : DnVelocity3D) {
        // normalize() on a zero vector divides by zero; guard the idle case.
        var wish = float3(0f, 0f, 0f)
        if (dot(input.dir, input.dir) > 0.0001f) {
            wish = normalize(input.dir) * speed.walk
        }
        vel.lin.x = wish.x
        vel.lin.z = wish.z
        input.dir = float3(0f, 0f, 0f)
    }
}

// Integrates downward acceleration into the persistent vel.lin.y. Clamped to a terminal
// velocity so a long fall cannot run the value away.
def system_gravity(var world : DnWorld; delta : double) : void {
    let TERMINAL_VELOCITY = -50.0f
    ecs_query(world) $ [CACHED, REQUIRE_NOT(OnGround)] (grav : GravityScale; var vel : DnVelocity3D) {
        vel.lin.y = max(vel.lin.y + grav.value * float(delta), TERMINAL_VELOCITY)
    }
}

// While grounded, hold a small constant downward bias instead of integrating. This keeps
// the character pinned to the floor without letting vel.lin.y run away to a large
// negative that would cancel out the next jump.
def system_ground_stick(var world : DnWorld, delta : double) : void {
    ecs_query(world) $ [CACHED, REQUIRE(OnGround)] (var vel : DnVelocity3D) {
        if (vel.lin.y < 0f) {
            vel.lin.y = -1.0f
        }
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
                    // dn_info("Parent gpos: {p_gpos.unwrap()}")
                } else {
                    var ptx : DnTransform3D? = parent.get_mut(type<DnTransform3D>)
                    if (ptx != null) {
                        // dn_info("ptx {ptx.pos}")
                    } else {
                        dn_warn("Camera parent has no local TX")
                    }
                    dn_warn("Camera parent has NO POSITION")
                }
            } else {
                dn_warn("Camera has NO PARENT!")
            }
            let pos = get_global_position(e).move_unwrap()
            // dn_info("GPOS: {pos.x}, {pos.y}, {pos.z}")
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

