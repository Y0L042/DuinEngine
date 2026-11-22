options gen2

module scene_viewer public

require daslib/delegate

require imgui
require imgui/imgui_boost_runtime
require imgui/imgui_boost_v2
require imgui/imgui_widgets_builtin
require imgui/imgui_containers_builtin
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
        window(SCENE_VIEWER_WIN, (text = "Scene Viewer", closable = false, flags = ImGuiWindowFlags.None)) {
        }
    }

    def update_scene(var new_scn : Scene) : void {
        active_scene = new_scn
    }
}

