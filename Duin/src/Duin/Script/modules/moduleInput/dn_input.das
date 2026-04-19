options gen2

module dn_input shared public

require dn_input_core public
require dn_log

// Keyboard

def dn_is_key_pressed(code : DN_Scancode) : bool {
    return dn_is_key_pressed_impl(code) != 0
}

def dn_is_key_pressed_again(code : DN_Scancode) : bool {
    return dn_is_key_pressed_again_impl(code) != 0
}

def dn_is_key_down(code : DN_Scancode) : bool {
    return dn_is_key_down_impl(code) != 0
}

def dn_is_key_up(code : DN_Scancode) : bool {
    return dn_is_key_up_impl(code) != 0
}

def dn_is_key_released(code : DN_Scancode) : bool {
    return dn_is_key_released_impl(code) != 0
}

def dn_get_key_pressed() : DN_Scancode {
    return dn_get_key_pressed_impl()
}

def dn_is_modifier_down(mod : uint) : bool {
    return dn_is_modifier_down_impl(mod) != 0
}

def dn_is_modifier_exact(mod : uint) : bool {
    return dn_is_modifier_exact_impl(mod) != 0
}

def dn_is_input_vector_pressed(up, dn, left, right : DN_Scancode) : bool {
    return dn_is_input_vector_pressed_impl(up, dn, left, right) != 0
}

def dn_get_input_vector(up, dn, left, right : DN_Scancode) : float2 {
    return dn_get_input_vector_impl(up, dn, left, right)
}

// Mouse

def dn_capture_mouse(enable : bool) {
    dn_capture_mouse_impl(enable ? 1 : 0)
}

def dn_is_mouse_button_pressed(btn : uint) : bool {
    return dn_is_mouse_btn_pressed_impl(btn) != 0
}

def dn_is_mouse_button_down(btn : uint) : bool {
    return dn_is_mouse_btn_down_impl(btn) != 0
}

def dn_is_mouse_button_released(btn : uint) : bool {
    return dn_is_mouse_btn_released_impl(btn) != 0
}

def dn_is_mouse_button_up(btn : uint) : bool {
    return dn_is_mouse_btn_up_impl(btn) != 0
}

def dn_get_mouse_position() : float2 {
    return dn_get_mouse_position_impl()
}

def dn_get_mouse_global_position() : float2 {
    return dn_get_mouse_global_position_impl()
}

def dn_get_mouse_delta() : float2 {
    return dn_get_mouse_delta_impl()
}

def dn_get_mouse_wheel_move() : float {
    return dn_get_mouse_wheel_move_impl()
}

def dn_get_mouse_wheel_move_v() : float2 {
    return dn_get_mouse_wheel_move_v_impl()
}

def dn_set_mouse_position(x, y : int) {
    dn_set_mouse_position_impl(x, y)
}

def dn_set_mouse_offset(ox, oy : int) {
    dn_set_mouse_offset_impl(ox, oy)
}

def dn_set_mouse_scale(sx, sy : float) {
    dn_set_mouse_scale_impl(sx, sy)
}

def dn_set_mouse_cursor(cursor : int) {
    dn_set_mouse_cursor_impl(cursor)
}

// InputActions

def dn_create_input_action(name : string) {
    dn_create_input_action_impl(name)
}

def dn_is_input_action_triggered(name : string) : bool {
    return dn_is_input_action_triggered_impl(name)
}

def dn_remove_input_action(name : string) {
    dn_remove_input_action_impl(name)
}

def dn_add_keyboard_binding(action : string; key : DN_Scancode; event : KeyEvent; modifier : uint = 0u) {
    dn_add_keyboard_binding_impl(action, key, event, modifier)
}

def dn_add_mouse_binding(action : string; button : uint; event : KeyEvent) {
    dn_add_mouse_binding_impl(action, button, event)
}

def dn_remove_keyboard_binding(action : string; key : DN_Scancode; event : KeyEvent; modifier : uint = 0u) {
    dn_remove_keyboard_binding_impl(action, key, event, modifier)
}

def dn_remove_mouse_binding(action : string; button : uint; event : KeyEvent) {
    dn_remove_mouse_binding_impl(action, button, event)
}
