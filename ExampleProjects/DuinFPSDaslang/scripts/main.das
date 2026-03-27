options gen2
options rtti

require dn_doctest
require dn_gameobject
require dn_log
require player
require test

var player : Player?
var child : Player?

var counter : float = 0.0

[export]
def game_ready() {
    dn_info("Game Ready!")
    //player = new Player()
    //dn_gameobject_init(player)
    player = dn_gameobject_add_root_child(new Player()) //dn_gameobject_init(new Player())

    child = dn_gameobject_add_child(player, new Player())
}

[export]
def game_update() {
  
}

[export]
def game_physics_update() { 
    counter = counter + (1.0 / 60.0)
    if (counter > 2.0) {
        counter = 0.0
        dn_info("PhysicsUpdate !")

        if (player == null) {
            dn_warn("player is null!")
        } else {
            //player._physics_update(double(0.0))
            //child._physics_update(double(0.0))
        }
    }
}

[export]
def game_draw() {}

[export]
def game_draw_ui() {}
