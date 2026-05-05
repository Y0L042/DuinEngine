options gen2

module editor_context shared public

require daslib/static_let public
require project public

// Singleton
struct EditorContext {
    project_data : Project
}

var g_editor_ctx : EditorContext

def get_editor_ctx() : EditorContext {
    return g_editor_ctx
}

def set_editor_ctx(new_ctx : EditorContext) : void {
    g_editor_ctx = new_ctx
}


