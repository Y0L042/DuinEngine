options gen2
options rtti

require ../../Headers.das

require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera
require dn_input
require dn_physics

require ../../state_machine.das
require ./StateMachine/player_on_ground_idle.das

class Player : dn_GameObject {
    sm : StateMachine?

    inverse : int = 1
    cnt : int
    speed : float = 6.0
    v_offset : float3 = float3(0.0, 2.0, 0.0)
    position : float3 = float3(5.0, 2.0, 5.0)


    game_world : dn_GameWorld?
    player_entity : dn_Entity?
    characterbody : dn_CharacterBody?
    camera : dn_Camera?

    def Player(var game_world_ : dn_GameWorld?) : void {
        sm = new StateMachine()
        sm.change_state(new Player_OnGround_Idle())
        game_world = game_world_
        player_entity = game_world.create_entity("Player")
        characterbody = new dn_CharacterBody()
        characterbody.create_capsule(
            70.0,
            1.0,
            60.0,
            true,
            true,
            true,
            true,
            1.8,
            0.25
        )

        camera = dn_camera_init(new dn_Camera())
        camera.set_impl(
            position + v_offset,
            float3(0.0, 0.0, 0.0),
            float3(0.0, 1.0, 0.0),
            80.0
        )
        dn_set_active_camera(camera)
        if (dn_get_active_camera() == null) {
            dn_fatal("Failed to set player camera as active camera!")
        }
    }

    def set_camera_active() : void {
        dn_set_active_camera(camera)
    }

    def override _init() : void {
        dn_info("Player init!")
        characterbody.initialize(position)
    }

    def override _ready() : void {
        dn_info("Player ready!")

    }

    def override _update(delta : double) : void {}

    def override _physics_update(delta : double) : void {     
        if ((player_entity == null) || (game_world == null)) {
            dn_warn("player_entity \{{player_entity}\} | game_world \{{game_world}\}")
            return
        }
        let pi = process_player_input()
        characterbody.move(pi, delta)
        position = characterbody.get_position()
        camera.move_to(position + v_offset)

    }

    def override _draw() : void {}

    def override _draw_ui() : void {}

    def override _debug() : void {}

    def process_player_input() : float3 {
        var move_speed = 7.0f
        let mouse_sensitivity = 0.0015f 

        // Mouse look
        let md = dn_get_mouse_delta()
        camera.yaw(md.x * mouse_sensitivity)

        let MAX_PITCH : float = 87.0 * (PI / 180.0)

        if ((camera.get_pitch() > MAX_PITCH) && (-md.y > 0.0)) {
            // Don't pitch
        } elif ((camera.get_pitch() < -MAX_PITCH) && (-md.y < 0.0)) {
            // Don't pitch
        } else {
            camera.pitch(-md.y * mouse_sensitivity)
        }
            
        // Horizontal movement relative to camera yaw (WASD), vertical with Q/Z
        let fwd = camera.forward()
        let rgt = camera.right()
        let h_fwd   = float3(fwd.x, 0.0f, fwd.z)
        let h_right = float3(rgt.x, 0.0f, rgt.z)
        
        var move = float3(0.0f, 0.0f, 0.0f)
        if (dn_is_key_down(DN_Scancode.DN_SCANCODE_W)) {
            move += h_fwd
        }
        if (dn_is_key_down(DN_Scancode.DN_SCANCODE_S)) {
            move -= h_fwd
        }
        if (dn_is_key_down(DN_Scancode.DN_SCANCODE_D)) {
            move -= h_right
        }
        if (dn_is_key_down(DN_Scancode.DN_SCANCODE_A)) {
            move += h_right
        }
        if (dn_is_key_pressed(DN_Scancode.DN_SCANCODE_SPACE)) {
            // move.y += 1.0f
        }

        return move * move_speed
    }
}
