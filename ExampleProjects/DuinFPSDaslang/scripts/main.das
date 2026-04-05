options gen2
options rtti

require dn_doctest

require dn_log

require dn_gameobject

require decs_world
require decs_entity
require decs_query

require player
require frogger

require flecs

var world : dn_World?
var player : Player?
var child : Frogger?

var q1 : dn_Query?

var counter : float = 0.0

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
    layer_2b();
    dn_info("Game Ready!")

    world = new dn_World()

    if (!world.is_valid()) {
        dn_warn("World is invalid!")
    } else {
        dn_info("World is valid.")
    }



    var entity1 : dn_Entity? = world.create_entity("Player")
    dn_info("Is entity {entity1.get_name()} {entity1._id} alive? {entity1.is_alive()}")

    //var e2 : dn_Entity? = new dn_Entity().set_name("peter")

    player = dn_gameobject_add_root_child(new Player())
    child = dn_gameobject_add_child(player, new Frogger())
}

[export]
def game_update() {
  
}

[export]
def game_physics_update() { 
    counter = counter + (1.0 / 60.0)
    if (counter > 2.0) {
        counter = 0.0
        //dn_info("PhysicsUpdate!")
    }
}

[export]
def game_draw() {}

[export]
def game_draw_ui() {}



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
            // var p : iterator<Position> <- ecs_field_iter(it, type<Position>, 0)
            var p : Position? = ecs_field_get(it, type<Position>, 0)
            var v : Velocity? = ecs_field_get(it, type<Velocity>, 1)
            // for (i, pi, vi in range(it.count), each(p), each(v)) {
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