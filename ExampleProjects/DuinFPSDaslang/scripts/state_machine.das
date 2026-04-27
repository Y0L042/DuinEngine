options gen2
options rtti

require Headers public

module state_machine shared public

require dn_gameobject


class StateMachine : dn_GameObject {
    current_state : State?

    def StateMachine() : void {}

    def StateMachine(var state : State?) : void {
        self.change_state(state)
    }

    [class_method]
    def static change_state(var state : State?) : void {
        if (current_state != null) {
            current_state._exit()
        }
        current_state = state
        if (current_state != null) {
            current_state._enter()
        }
    }

    def get_current_state() : State? {
        return current_state
    }

    def override _init() : void {}

    def override _ready() : void {}

    def override _update(delta : double) : void {
        if (current_state != null) {
            current_state._update(delta)
        }
    }

    def override _physics_update(delta : double) : void {
        if (current_state != null) {
            current_state._physics_update(delta)
        }
    }

    def override _draw() : void {
        if (current_state != null) {
            current_state._draw()
        }
    }

    def override _draw_ui() : void {
        if (current_state != null) {
            current_state._draw_ui()
        }
    }

    def override _debug() : void {}
}



class State {

    def _enter() : void {}

    def _update(delta : double) : void {}
    def _physics_update(delta : double) : void {}
    def _draw() : void {}
    def _draw_ui() : void {}

    def _exit() : void {}

}
