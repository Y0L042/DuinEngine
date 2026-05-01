options gen2
options remove_unused_symbols = false
options rtti

require daslib/class_boost public
require daslib/dynamic_cast_rtti
require daslib/rtti public
require rtti_core public

module dn_gamestatemachine shared public

require dn_log public
require dn_gamestate_class public
require dn_gamestatemachine_class public
require dn_gamestatemachine_core public

// ── GameState ─────────────────────────────────────────────────────────────

class dn_GameState : dn_GameState_Adapter {
    _alias : string = "dn_GameState"

    def dn_GameState() : void {}

    def finalize() : void {
        if (_handle != null) {
            dn_destroy_gamestate_impl(_handle)
            _handle = null
        }
    }

    def init(sm_handle : void?) : void {
        unsafe {
            _handle = dn_create_gamestate_impl(sm_handle, addr(self), class_info(self))
        }
    }

    def override _enter() : void {}
    def override _update(delta : double) : void {}
    def override _physics_update(delta : double) : void {}
    def override _draw() : void {}
    def override _draw_ui() : void {}
    def override _exit() : void {}
    def override _set_pause() : void {}
    def override _set_unpause() : void {}
}

// ── GameStateMachine ──────────────────────────────────────────────────────

class dn_GameStateMachine : dn_GameStateMachine_Adapter {
    current_state : dn_GameState?

    def dn_GameStateMachine() : void {
        unsafe {
            _handle = dn_create_gamestatemachine_impl(addr(self), class_info(self))
            dn_info("dn_gamestatemachine_init: _handle = {_handle}\n")
        }
    }

    def finalize() : void {
        if (_handle != null) {
            dn_destroy_gamestatemachine_impl(_handle)
            _handle = null
        }
    }

    def override _init() : void {}
    def override _ready() : void {}
    def override _update(delta : double) : void {}
    def override _physics_update(delta : double) : void {}
    def override _draw() : void {}
    def override _draw_ui() : void {}
    def override _debug() : void {}

    [class_method]
    def static switch_state(var state : auto ?) : auto ? {
        var prev_state = current_state
        current_state = state

        if (_handle != null && state != null) {
            if (state._handle == null) {
                state.init(_handle)
            }
            dn_gsm_switch_state_impl(_handle, state._handle)
        }
        dn_info("Switched States from {prev_state?._alias} to {current_state?._alias}")
        return state
    }

    [class_method]
    def static push_state(var state : auto ?) : auto ? {
        if (_handle != null && state != null) {
            if (state._handle == null) {
                state.init(_handle)
            }
            dn_gsm_push_state_impl(_handle, state._handle)
        }
        return state
    }

    [class_method]
    def static flush_and_switch_state(var state : auto ?) : auto ? {
        if (_handle != null && state != null) {
            if (state._handle == null) {
                state.init(_handle)
            }
            dn_gsm_flush_and_switch_state_impl(_handle, state._handle)
        }
        return state
    }

    def pop_state() : void {
        if (_handle != null) {
            dn_gsm_pop_state_impl(_handle)
        }
    }

    def flush_stack() : void {
        if (_handle != null) {
            dn_gsm_flush_stack_impl(_handle)
        }
    }

    [class_method]
    def static add_child(var child : auto ?) : auto ? {
        if ((_handle != null) && (child != null) && (child._handle != null)) {
            dn_gsm_add_child_impl(_handle, child._handle)
            return child
        }
        return null
    }

    [class_method]
    def static remove_child(var child : auto ?) : void {
        if ((_handle != null) && (child != null) && (child._handle != null)) {
            dn_gsm_remove_child_impl(_handle, child._handle)
        }
    }

    def get_children_count() : int {
        if (_handle != null) {
            return dn_gsm_get_children_count_impl(_handle)
        }
        return 0
    }

    def enable(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_impl(_handle, e)
        }
    }

    def enable_on_event(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_on_event_impl(_handle, e)
        }
    }

    def enable_update(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_update_impl(_handle, e)
        }
    }

    def enable_physics_update(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_physics_update_impl(_handle, e)
        }
    }

    def enable_draw(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_draw_impl(_handle, e)
        }
    }

    def enable_draw_ui(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_draw_ui_impl(_handle, e)
        }
    }

    def enable_debug(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_debug_impl(_handle, e)
        }
    }

    def enable_children(e : bool) : void {
        if (_handle != null) {
            dn_gsm_enable_children_impl(_handle, e)
        }
    }

    def is_on_event_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_on_event_enabled_impl(_handle)
        }
        return false
    }

    def is_update_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_update_enabled_impl(_handle)
        }
        return false
    }

    def is_physics_update_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_physics_update_enabled_impl(_handle)
        }
        return false
    }

    def is_draw_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_draw_enabled_impl(_handle)
        }
        return false
    }

    def is_draw_ui_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_draw_ui_enabled_impl(_handle)
        }
        return false
    }

    def is_debug_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_debug_enabled_impl(_handle)
        }
        return false
    }

    def is_children_enabled() : bool {
        if (_handle != null) {
            return dn_gsm_is_children_enabled_impl(_handle)
        }
        return false
    }
}
