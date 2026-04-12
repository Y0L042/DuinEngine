options gen2
options rtti=true

require daslib/rtti

require dn_gameobject
require dn_log
require dn_ecs

class Frogger : dn_GameObject {
    x : double;
    cnt : int;
    entity : ecs_entity_t

    def override _init() {}

    def override _ready() {
        dn_info("Frogeger ready!");
    }

    def override _update(delta : double) {}

    def override _physics_update(delta : double) {
        x = x + delta;
        if (x > 1.0d) {
            dn_info("Frogger CNT: {cnt++}");
            x = double(0.0);
        }
    }

    def override _draw() {}

    def override _draw_ui() {}

    def override _debug() {}
}

