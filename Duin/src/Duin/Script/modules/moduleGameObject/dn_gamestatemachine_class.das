options gen2
options remove_unused_symbols = false
options rtti

module dn_gamestatemachine_class shared public

// Minimal class-only definition for adapter generation.
class dn_GameStateMachine_Adapter {
    _handle : void?

    def abstract _init() : void
    def abstract _ready() : void
    def abstract _update(delta : double) : void
    def abstract _physics_update(delta : double) : void
    def abstract _draw() : void
    def abstract _draw_ui() : void
    def abstract _debug() : void
}