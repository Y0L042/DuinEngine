options gen2

require math
require daslib/rtti
require daslib/random
require daslib/class_boost
require daslib/ansi_colors

require flecs_boost
require imgui_boost

require dn_log
require dn_gameobject
require dn_ecs
require dn_gameworld
require dn_renderer
require dn_camera
require dn_input
require dn_physics
require dn_application
require dn_gamestatemachine
require dn_live_host


require main

var _root : _Root?  // Permanent Lifetime scope, OK to be a pointer

[export]
def game_ready() {
    use_tty_colors = true
    _root = dn_application_add_child(new _Root())
}


[export]
def game_update(delta : double) {

}

[export]
def game_physics_update(delta : double) {

}

[export]
def game_draw() {

}

[export]
def game_draw_ui() {
}
