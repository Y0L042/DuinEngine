options gen2
options rtti

require daslib/rtti

require dn_gameobject
require dn_log

class Player : dn_GameObject {
    x : double;
    cnt : int;

    def override _init() {}

    def override _ready() {
        dn_info("Player ready!");
    }

    def override _update(delta : double) {}

    def override _physics_update(delta : double) {
        x = x + delta;
        if (x > double(1.0f)) {
            dn_info("Player  CNT: {cnt++}");
            x = double(0.0);
        }
    }

    def override _draw() {}

    def override _draw_ui() {}

    def override _debug() {}
}

