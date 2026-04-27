options gen2
options rtti

require daslib/rtti
require daslib/random
require daslib/class_boost
require math

require dn_live_host // NOT ACTIVE

require flecs_boost
require imgui_boost

require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera
require dn_input
require dn_physics
require dn_application

require ecs_components
require ecs_systems

require ./GameObjects/Player/player.das
require ./GameObjects/Frogger/frogger.das
require ./GameObjects/Zeus/zeus.das

var player : Player?
var game_world : dn_GameWorld?
var zeus : Zeus?
var zeus_mode : bool = false

[export]
def game_ready() {
    if (is_reload()) {
        dn_info("Game Hot-Reloaded!")
    } else {
        dn_info("Game Ready!")
    }

    dn_capture_mouse(true)

    // GameWorld always re-wraps (smart constructor reuses existing C++ world on reload).
    game_world = new dn_GameWorld()

    // Entities and game state are only created on first load.
    if (!is_reload()) {
        var world = game_world.dn_world()

        world|>register_components()
        world|>construct_queries()

        player = dn_gameobject_add_root_child(new Player(game_world))
        player._init()

        create_cube_crowd()

        dn_physics_server_create_plane(float3(0.0, 1.0, 0.0), 0.0)
        dn_physics_server_create_box(float3(0.0, 15.0, 0.0), float3(1.0, 1.0, 1.0))
    }

    zeus = dn_gameobject_add_root_child(new Zeus())
    zeus._init()

    register_keybinds()
}


[export]
def game_update(delta : double) {

}

[export]
def game_physics_update(delta : double) {
    if (dn_is_key_pressed(DN_Scancode.DN_SCANCODE_SPACE)) {
        spawn_block()
    }

    if (dn_is_input_action_triggered("ToggleZeus")) {
        dn_info("ToggleZeus Pressed")
        if (zeus_mode) {
            zeus_mode = false
            dn_info("Disabling Zeus Mode...")
            player.set_camera_active()
            player.enable_physics_update(true)

            zeus.enable_physics_update(false)
        } else {
            zeus_mode = true
            dn_info("Enabling Zeus Mode...")
            zeus.set_camera_active()
            zeus.enable_physics_update(true)

            player.enable_physics_update(false)
        }
    }

    if (game_world != null) {
        // system_integrate_velocity(game_world.ecs_world(), delta)
    }

    // process_input(delta)
}

[export]
def game_draw() {
    if (player != null && player.player_entity != null) {
        let pos = game_world.get_global_position(player.player_entity)
        dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
    }
    draw_cube_crowd()


    dn_begin_debug_draw()

    dn_draw_debug_grid(50.0)
    dn_physics_server_debug_draw()
    dn_end_debug_draw()

}

[export]
def game_draw_ui() {
    var active_cam : dn_Camera?# = dn_get_active_camera()
    let impl : dn_CameraImpl = active_cam.get_impl()
    imgui_boost::Begin("Daslang Dashboard!")
    imgui_boost::Text("Render  FPS: {dn_get_render_frame_time() * 1000f}")
    imgui_boost::Text("Physics FPS: {dn_get_physics_frame_time() * 1000f}")
    imgui_boost::Text("Camera:")
    imgui_boost::Text("Position: \{{impl.position.x}, {impl.position.y}, {impl.position.z}\}")
    imgui_boost::Text("Target: \{{impl.target.x}, {impl.target.y}, {impl.target.z}\}")
    imgui_boost::End()
}

def register_keybinds() : void {
    // Zeus keybinds
    dn_add_keyboard_binding(
        "ToggleZeus",
        DN_Scancode.DN_SCANCODE_P, 
        KeyEvent.PRESSED, 
        DN_KEY_MOD_LCTRL
    )

    dn_add_keyboard_binding(
        "Zeus_MoveCameraUp",
        DN_Scancode.DN_SCANCODE_Q, 
        KeyEvent.HELD, 
        DN_KEY_MOD_NONE
    )

    dn_add_keyboard_binding(
        "Zeus_MoveCameraDown",
        DN_Scancode.DN_SCANCODE_Z, 
        KeyEvent.HELD, 
        DN_KEY_MOD_NONE
    )

    dn_add_keyboard_binding(
        "Zeus_MoveCameraFast",
        DN_Scancode.DN_SCANCODE_LSHIFT, 
        KeyEvent.HELD, 
        DN_KEY_MOD_NONE
    )

    dn_add_keyboard_binding(
        "Zeus_ResetCameraPos",
        DN_Scancode.DN_SCANCODE_R, 
        KeyEvent.PRESSED, 
        DN_KEY_MOD_LCTRL
    )

    // Player keybinds
}

let crowd_size = 1000
var cubes : ecs_entity_t[crowd_size]
def create_cube_crowd() : void {
    var seed = random_seed(43)
    for (i in range(0, crowd_size)) {
        let world = game_world.ecs_world()
        var e : ecs_entity_t = game_world.create_entity("{i}").ecs_entity()
        ecs_set(world, e, RenderCube(size=float3(1.0, 1.0, 1.0)))
        dn_entity_set_transform3d(world, e, dn_make_transform3d(random_float(seed) * 1000.0 % 100.0 - 50.0, 0.0, random_float(seed) * 1000.0 % 100.0 - 50.0, 1.0, 1.0, 1.0, 0.0, 0.0, 0.0, 1.0))
        dn_entity_set_velocity3d(world, e, dn_make_velocity3d(0.0, 0.0, 0.0))        
        cubes[i] = e
    }

    dn_info("Crowd of {length(cubes)} cubes created.")
}

def draw_cube_crowd() : void {
    var seed = random_seed(43)
    system_draw_rendercube(game_world)
    for (i in range(0, crowd_size)) {
        // dn_draw_box(float3(random_float(seed) * 1000.0 % 100.0 - 50.0, 0, random_float(seed) * 1000.0 % 100.0 - 50.0), float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 1.0))
    }
}

def spawn_block() : void {
    dn_physics_server_create_box(float3(0.0, 10.0, 0.0), float3(1.0, 1.0, 1.0))
}