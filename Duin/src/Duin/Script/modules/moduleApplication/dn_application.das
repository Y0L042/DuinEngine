options gen2
options remove_unused_symbols = false

module dn_application shared public

require dn_application_core public
require dn_gameobject public

def dn_application_add_child(var child : auto?) : auto? {
    if (child != null) {
        if (child._handle == null) {
            dn_gameobject_init(child)
        }
        dn_app_add_child_object_impl(child._handle)
        return child
    }
    return null
}

// Wrapper so the quit flag can be set with no argument (defaults to true),
// mirroring the C++ SetGameShouldQuit(bool = true). Forwards to the raw extern
// dn_set_game_should_quit_impl, which takes the bool explicitly.
def dn_set_game_should_quit(shouldQuit : bool = true) : void {
    dn_set_game_should_quit_impl(shouldQuit)
}
