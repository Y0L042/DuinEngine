options gen2
options remove_unused_symbols = false
options rtti

// Minimal class-only definition of dn_GameWorld for adapter generation.
// Does not require dn_ecs (and by extension flecs), so it can be loaded
// by gen_bind.das in the standalone daslang runner.

class dn_GameWorld {
    _handle : void?

    def abstract _post_update(delta : double) : void
    def abstract _post_physics_update(delta : double) : void
    def abstract _post_draw() : void
    def abstract _post_draw_ui() : void
}
