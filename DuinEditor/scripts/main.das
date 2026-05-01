options gen2
options rtti

require daslib/rtti
require daslib/random
require daslib/class_boost
require math

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

require state_project_selection



class _Root : dn_GameObject {
    game_states : dn_GameStateMachine?  // Lifetime scope, OK to be a pointer
    test_obj : dn_GameObject?

    def override _ready() : void {
        game_states = self.add_child(new dn_GameStateMachine())

        game_states.switch_state(new State_ProjectSelection())
    }


    def override _update(delta : double) : void {

    }

    def override _physics_update(delta : double) : void {

    }

    def override _draw() : void {

    }

    def override _draw_ui() : void {
        imgui_boost::Begin("Daslang Dashboard!")
        imgui_boost::Text("Render  FPS: {dn_get_render_frame_time() * 1000f}")
        imgui_boost::Text("Physics FPS: {dn_get_physics_frame_time() * 1000f}")
        imgui_boost::Text("Editor state: {game_states?.current_state?._alias}")
        imgui_boost::End()
    }
}