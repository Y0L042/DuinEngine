options gen2
options rtti

require dn_gameobject
require dn_log

class Player : dn_GameObject {
    x : int

    def override _init() {}
    def override _ready() {
        dn_info("Player ready!")
    }
    def override _update(delta : double) {}
    def override _physics_update(delta : double) {
        //x += 1
        //dn_info("x = {x}\n")
    }
    def override _draw() {}
    def override _draw_ui() {}
    def override _debug() {}
}

