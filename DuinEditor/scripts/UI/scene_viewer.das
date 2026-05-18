options gen2

module scene_viewer public

require daslib/delegate

require imgui_boost
require dn_log
require dn_gameobject

require ../scene.das


class SceneViewer : DnGameObject {
    active_scene : Scene

    def SceneViewer() : void {
        super()
    }

    def override _init() : void {
    }

    def override _ready() : void {
    }

    def override _draw_ui() : void {
        imgui_boost::Begin("Scene Viewer")
        imgui_boost::End()
    }

    def update_scene(var new_scn : Scene) : void {
        active_scene = new_scn
    }
}

