options gen2
options remove_unused_symbols = false
options rtti

module dn_gamestate_class shared public

// Minimal class-only definition for adapter generation.
class dn_GameState_Adapter {
    _handle : void?

    def abstract _enter() : void
    def abstract _update(delta : double) : void
    def abstract _physics_update(delta : double) : void
    def abstract _draw() : void
    def abstract _draw_ui() : void
    def abstract _exit() : void
    def abstract _set_pause() : void
    def abstract _set_unpause() : void
}