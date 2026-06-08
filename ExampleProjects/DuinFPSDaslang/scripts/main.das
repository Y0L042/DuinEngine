options gen2
options rtti
options string_heap_size_limit = 0

// require daslib/debug
// require daslib/profiler
// require daslib/profiler_boost

require daslib/rtti
require daslib/random
require daslib/class_boost
require math


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
var game_world : DnGameWorld?
var zeus : Zeus?
var zeus_mode : bool = false

[export]
def game_ready() {
    // wait_for_debugger()

    dn_info("Game Ready!")
    dn_capture_mouse(true)

    // GameWorld always re-wraps (smart constructor reuses existing C++ world on reload).
    game_world = new DnGameWorld()

    // Entities and game state are only created on first load.
    assume world = game_world.dn_world()

    // destruct_queries()
    world |> register_components()
    world |> construct_queries()

    player = dn_application_add_child(new Player(game_world))
    player._init()

    dn_physics_server_create_plane(float3(0.0, 1.0, 0.0), 0.0)
    dn_physics_server_create_box(float3(0.0, 15.0, 0.0), float3(1.0, 1.0, 1.0))

    zeus = dn_application_add_child(new Zeus())
    zeus._init()
 
    register_keybinds()
}


[export]
def game_update(delta : double) {

}

[export]
def game_physics_update(delta : double) {
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
}

[export]
def game_draw() {

    dn_begin_debug_draw()

    dn_draw_debug_grid(50.0)
    dn_physics_server_debug_draw()
    dn_end_debug_draw()

}

[export]
def game_draw_ui() {
    imgui_boost::Begin("Daslang Dashboard!")
    imgui_boost::Text("Render  Frametime: {dn_get_render_frame_time() * 1000f}")
    imgui_boost::Text("Physics Frametime: {dn_get_physics_frame_time() * 1000f}")
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
    dn_create_input_action("Player_MoveForward")
    dn_add_keyboard_binding("Player_MoveForward", DN_Scancode.DN_SCANCODE_W, KeyEvent.HELD, DN_KEY_MOD_NONE)

    dn_create_input_action("Player_MoveBackward")
    dn_add_keyboard_binding("Player_MoveBackward", DN_Scancode.DN_SCANCODE_S, KeyEvent.HELD, DN_KEY_MOD_NONE)

    dn_create_input_action("Player_StrafeLeft")
    dn_add_keyboard_binding("Player_StrafeLeft", DN_Scancode.DN_SCANCODE_A, KeyEvent.HELD, DN_KEY_MOD_NONE)

    dn_create_input_action("Player_StrafeRight")
    dn_add_keyboard_binding("Player_StrafeRight", DN_Scancode.DN_SCANCODE_D, KeyEvent.HELD, DN_KEY_MOD_NONE)

    dn_create_input_action("Player_Sprint")
    dn_add_keyboard_binding("Player_Sprint", DN_Scancode.DN_SCANCODE_LSHIFT, KeyEvent.HELD, DN_KEY_MOD_NONE)
}
