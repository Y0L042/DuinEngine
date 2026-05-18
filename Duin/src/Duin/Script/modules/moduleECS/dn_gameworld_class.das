options gen2
options remove_unused_symbols = false

module dn_gameworld_class shared public

// Adapter contract for codegen — no C++ builtin requires.
// DnGameWorld inherits from this in dn_gameworld.das.
class DnGameWorld_Adapter {
    _handle : void?

    def abstract _post_update(delta : double) : void
    def abstract _post_physics_update(delta : double) : void
    def abstract _post_draw() : void
    def abstract _post_draw_ui() : void
}
