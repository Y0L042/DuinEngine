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

var world : dn_World?
var player : Player?
var child : Frogger?

var q1 : dn_Query?

var counter : float = 0.0

[export]
def game_ready() {
    dn_info("Game Ready!")

    world = new dn_World()

    if (!world.is_valid()) {
        dn_warn("World is invalid!")
    } else {
        dn_info("World is valid.")
    }

    var t1 : dn_Term = dn_Term(id = 2ul)
    var desc : dn_QueryDesc = dn_QueryDesc()
    desc.terms = [t1]

    //var desc : dn_QueryDesc? = new dn_QueryDesc()
    //desc.terms = [dn_Term(id=1u), dn_Term(id=2u)]
    //q1 = dn_query_builder(world, desc)

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
