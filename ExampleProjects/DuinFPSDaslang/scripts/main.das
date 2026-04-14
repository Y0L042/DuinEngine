options gen2
options rtti

require daslib/rtti

require flecs
require imgui
require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera

require player
require frogger

var player : Player?
var child : Frogger?
var world : ecs_world_t?
var game_world : dn_GameWorld?
var counter : float = 0.0
var counter2 : float = 0.0
var counter_int : int = 0

var camera : dn_Camera?

var q_update_pos : ecs_query_t?

struct Position {
    x : double
    y : double
}

struct Velocity {
    x : double
    y : double
}


[export]
def game_ready() {
    dn_info("Game Ready!")

    camera = dn_camera_init(new dn_Camera())
    camera.set_impl_from_params(
        float3(6.0, 6.0, 6.0),
        float3(0.0, 0.0, 0.0),
        float3(0.0, 1.0, 0.0),
        80.0
    )
    dn_set_active_camera(camera)
    if (dn_get_active_camera() == null) {
        dn_fatal("Failed to set active camera!")
    }

    game_world = dn_gameworld_init(new dn_GameWorld())
    world = game_world.ecs_world()

    ecs_component_register(world, type<Position>);
    ecs_component_register(world, type<Velocity>);

    player = dn_gameobject_add_root_child(new Player())
    child = dn_gameobject_add_child(player, new Frogger())

    player.entity = ecs_entity_new(world, "Player")
    child.entity = ecs_entity_new(world, "Frogger")
    ecs_add_pair(world, child.entity, EcsChildOf, player.entity)

    ecs_set(world, player.entity, Position(x=10.0d, y=20.0d))
    ecs_set(world, player.entity, Velocity(x=1.0d, y=2.0d))

    q_update_pos = ecs_query_builder(
        world, 
        [
            ecs_term(world, type<Position>),
            ecs_term(world, type<Velocity>, EcsIn)
        ], 
        EcsQueryCacheAuto
    )
}

[export]
def game_update(delta : double) {
}

[export]
def game_physics_update(delta : double) { 
    counter = counter + (1.0 / 60.0)
    if (counter > 2.0) {
        counter = 0.0
        //dn_info("PhysicsUpdate!")
    }

    var it = ecs_query_iter(world, q_update_pos)
    unsafe {
        while (ecs_query_next_iter(it)) {
            var p : Position? = ecs_field_get(it, type<Position>, 0)
            var v : Velocity? = ecs_field_get(it, type<Velocity>, 1)
            for (i in range(it.count)) {
                p[i].x += v[i].x
                p[i].y += v[i].y
                let name = ecs_get_name(world, it.entities[i])
                // dn_info("{name}: \{ {p[i].x}, {p[i].y} \}")
            }
        }
    }
}

[export]
def game_draw() {
    dn_draw_box(float3(0.0, 0.0, 0.0), float4(0.0, 0.0, 0.0, 1.0), float3(1.0, 1.0, 1.0))
}

[export]
def game_draw_ui() {
    counter2 = counter2 + (1.0 / 60.0)
    if (counter2 > 1.0) {
        counter2 = 0.0

        counter_int = counter_int + 1
    }
    var active_cam : dn_Camera?# = dn_get_active_camera()
    let impl : dn_CameraImpl = active_cam.get_impl()
    imgui_boost::Begin("Daslang Dashboard!")
    imgui_boost::Text("Counter: {counter_int}")
    imgui_boost::Text("Camera:")
    imgui_boost::Text("Position: \{{impl.position.x}, {impl.position.y}, {impl.position.z}\}")
    imgui_boost::Text("Target: \{{impl.target.x}, {impl.target.y}, {impl.target.z}\}")
    imgui_boost::End()
}



def layer_2b() : void {
    var world : ecs_world_t? = ecs_init()

    // --- ECS_COMPONENT equivalent ---
    let pos_id = ecs_component_register(world, type<Position>)
    let vel_id = ecs_component_register(world, type<Velocity>)

    // --- Build query via expression string ---
    let q = ecs_query_str(world, "Position, [in] Velocity") 

    // --- Create entities with ecs_entity_new + ecs_set ---
    let e1 = ecs_entity_new(world, "e1")
    ecs_set(world, e1, Position(x=10.0d, y=20.0d))
    ecs_set(world, e1, Velocity(x=1.0d, y=2.0d))

    let e2 = ecs_entity_new(world, "e2")
    ecs_set(world, e2, Position(x=10.0d, y=20.0d))
    ecs_set(world, e2, Velocity(x=3.0d, y=4.0d))

    // e3: Position only — will not match the query
    let e3 = ecs_entity_new(world, "e3")
    ecs_set(world, e3, Position(x=10.0d, y=20.0d))

    // --- Iterate using ecs_field ---
    var it = ecs_query_iter(world, q)
    unsafe {
        while (ecs_query_next_iter(it)) {
            var p : Position? = ecs_field_get(it, type<Position>, 0)
            var v : Velocity? = ecs_field_get(it, type<Velocity>, 1)
            for (i in range(it.count)) {
                p[i].x += v[i].x
                p[i].y += v[i].y
                let name = ecs_get_name(world, it.entities[i])
                print("{name}: \{\{ {p[i].x}, {p[i].y} \}\}\n")
            }
        }
    }

    ecs_query_fini(q)
    ecs_fini(world)
}
