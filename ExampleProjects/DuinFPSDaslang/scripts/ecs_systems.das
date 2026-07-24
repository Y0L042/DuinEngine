options gen2
options log

module ecs_systems shared public

// Daslib
require math

// Bindings
require flecs
require dn_ecs
require dn_renderer
require dn_gameworld
require dn_log
require dn_input
require dn_camera
require dn_physics

// Game
require ecs_components

// Binding Macros
require dn_ecs_boost


// Queries are created lazily per call site by the ecs_query macro and cached
// in dn_ecs; DnGameWorld.fini destroys them. No construct/destruct needed.
def system_player_input(world : ecs_world_t?; var camera : DnCamera?) : void {
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

def system_player_jump(var world : ecs_world_t?, delta : double) : void {
    ecs_query(world) $ [CACHED, REQUIRE(IsPlayer)] (e : ecs_entity_t, var ji : JumpImpulse, var vel : Velocity3D) {
        vel.val.y += ji.value
        world.ecs_defer_begin()
        var jid = DnWorld(world=world).dn_register_component(type<JumpImpulse>)
        ecs_remove(world, e, jid)
        world.ecs_defer_end()
    }
}

def system_set_input_velocity(world : ecs_world_t?, delta : double) : void {
    world.ecs_query() $ [CACHED, REQUIRE(IsPlayer)] (var input : MoveInput, speed : MoveSpeed, var vel : Velocity3D) {
        vel.val = normalize(input.dir) * speed.walk * float(delta)
        input.dir = float3(0f, 0f, 0f)
    }
}

def system_gravity(world : ecs_world_t?; delta : double) : void {
    ecs_query(world) $ [CACHED] (grav : GravityScale; var vel : Velocity3D) {
        vel.val.y += grav.value * float(delta)
    }
}

def system_ground_friction(world : ecs_world_t?, delta : double) : void {
    ecs_query(world) $ [CACHED] (fric : GroundFriction, var exvel : ExternVelocity) {
        exvel.val = lerp(exvel.val, float3(0f, 0f, 0f), fric.coef * float(delta))
    }
}

def system_air_friction(world : ecs_world_t?, delta : double) : void {
    ecs_query(world) $ [CACHED] (fric : AirFriction, var exvel : ExternVelocity) {
        exvel.val = lerp(exvel.val, float3(0f, 0f, 0f), fric.coef * float(delta))
    }
}

def system_resolve_external_velocities(world : ecs_world_t?, delta : double) : void {
    ecs_query(world) $ [CACHED] (var exvel : ExternVelocity, var vel : Velocity3D) {
        vel.val += exvel.val
        exvel.val = float3(0f, 0f, 0f)
    }
}

def system_characterbody_move(world : ecs_world_t?, delta : double) :  void {
    ecs_query(world) $ [CACHED] (var cbh : CharacterBodyHandle, var vel : Velocity3D) {
        cbh.cb.move(vel.val, delta)
    }
}

def system_update_position(world : ecs_world_t?, delta : double) :  void {
    ecs_query(world) $ [CACHED] (var cbh : CharacterBodyHandle, var tx : Transform3D) {
        tx.pos = cbh.cb.get_position()
    }
}

def system_update_active_camera_position(world : ecs_world_t?, delta : double) : void {
    ecs_query(world) $ [CACHED] (var tx : Transform3D) {

    }
}

def system_draw_rendercube(var game_world : DnGameWorld?) : void {
    var world : ecs_world_t? = game_world.ecs_world()
    ecs_query(world) $ [CACHED, REQUIRE(RenderCube)] (eid : ecs_entity_t, tx : Transform3D) {
        let ent = DnEntity(world=world, entity=eid)
        let pos : float3 = game_world.get_global_position(ent)
        dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
    }
}

[ecs_system(PHYSICSUPDATE::POST)]
def update_position(world : ecs_world_t?, delta : double) : void {
    tx.pos = cbh.cb.get_position()
}

