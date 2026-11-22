options gen2
options rtti

require daslib/rtti
require daslib/random
require daslib/class_boost
require math

require flecs_boost
require imgui
require imgui/imgui_boost_runtime
require imgui/imgui_boost_v2
require imgui/imgui_widgets_builtin
require imgui/imgui_containers_builtin

require dn_log
require dn_gameobject
require dn_ecs
require dn_renderer
require dn_camera
require dn_input
require dn_physics
require dn_application
require dn_gamestatemachine

require state_project_selection
require state_project_editor
require editor_context



class _Root : DnGameObject {
    game_states : DnGameStateMachine?  // Lifetime scope, OK to be a pointer

    def _Root() : void {
        super()
    }

    def override _ready() : void {
        game_states = self.add_child(new DnGameStateMachine())
        game_states.switch_state(new State_ProjectSelection())
    }


    def override _update(delta : double) : void {

    }

    def override _physics_update(delta : double) : void {

    }
 
    def override _draw() : void {

    }

    def override _draw_ui() : void {
        window(ROOT_DASH_WIN, (text = "Daslang Dashboard!", closable = false, flags = ImGuiWindowFlags.None)) {
            text("Render  FPS: {dn_get_render_frame_time() * 1000f}")
            text("Physics FPS: {dn_get_physics_frame_time() * 1000f}")
            text("Editor state: {game_states?.current_state?._alias}")
        }
    }
}
