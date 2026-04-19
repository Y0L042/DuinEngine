options gen2
options rtti

require daslib/rtti
require daslib/random
require daslib/class_boost
require math

require flecs_boost
require imgui
require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera
require dn_input
require dn_physics_server

require ecs_components
require ecs_systems
require player
require frogger

var player : Player?
var child : Frogger?
var game_world : dn_GameWorld?
var camera : dn_Camera?

[export]
def game_ready() {
    dn_info("Game Ready!")

    camera = dn_camera_init(new dn_Camera())
    camera.set_impl(
        float3(15.0, 8.0, 15.0),
        float3(0.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        80.0
    )
    dn_set_active_camera(camera)
    if (dn_get_active_camera() == null) {
        dn_fatal("Failed to set active camera!")
    }

    dn_capture_mouse(true)

    game_world = new dn_GameWorld()
    var world = game_world.dn_world()

    world|>register_components()
    world|>construct_queries()

    // register_components(world)
    // construct_queries(world)

    player = dn_gameobject_add_root_child(new Player(game_world))

    create_cube_crowd()

    dn_physics_server_create_plane(float3(0.0, 1.0, 0.0), 0.0)
    dn_physics_server_create_box(float3(0.0, 10.0, 0.0), float3(1.0, 1.0, 1.0))
}


[export]
def game_update(delta : double) {

}

[export]
def game_physics_update(delta : double) {
    if (game_world != null) {
        // system_integrate_velocity(game_world.ecs_world(), delta)
    }

    process_input(delta)
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
    imgui_boost::Text("Camera:")
    imgui_boost::Text("Position: \{{impl.position.x}, {impl.position.y}, {impl.position.z}\}")
    imgui_boost::Text("Target: \{{impl.target.x}, {impl.target.y}, {impl.target.z}\}")
    imgui_boost::End()
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


var move_speed = 7.0f
let mouse_sensitivity = 0.0015f
def process_input(delta : double) : void {
        if (camera == null) {
        return
    }
    let dt = float(delta)

    if (dn_is_key_down(DN_Scancode.DN_SCANCODE_LSHIFT)) {
        move_speed = 7.0f * 2.2f
    } else {
        move_speed = 7.0f
    }

    if (dn_is_key_down(DN_Scancode.DN_SCANCODE_R)) {
        camera.set_position(float3(5.0f, 5.0f, 5.0f))
    }

    // Mouse look
    let md = dn_get_mouse_delta()
    camera.yaw(md.x * mouse_sensitivity)
    camera.pitch(-md.y * mouse_sensitivity)

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
    if (dn_is_key_down(DN_Scancode.DN_SCANCODE_Q)) {
        move.y += 1.0f
    }
    if (dn_is_key_down(DN_Scancode.DN_SCANCODE_Z)) {
        move.y -= 1.0f
    }

    if (dn_is_key_pressed(DN_Scancode.DN_SCANCODE_SPACE)) {
        spawn_block()
    }

    let move_len = length(move)
    if (move_len > 0.001f) {
        let step = (move / move_len) * move_speed * dt
        let pos = camera.get_position()
        let tgt = camera.get_target()
        camera.set_position(pos + step)
        camera.set_target(tgt + step)
    }
}

def spawn_block() : void {
    dn_physics_server_create_box(float3(0.0, 10.0, 0.0), float3(1.0, 1.0, 1.0))
}