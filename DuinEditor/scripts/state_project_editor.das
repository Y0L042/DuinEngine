options gen2

require imgui_boost
require dn_log

require dn_gamestatemachine

class State_ProjectEditor : dn_GameState {


    def State_ProjectEditor() : void {
        _alias = "State_ProjectEdito"
    }

    def override _enter() : void {
    }

    def override _update(delta : double) : void {
    }

    def override _physics_update(delta : double) : void {}

    def override _draw() : void {}

    def override _draw_ui() : void {
        imgui_boost::Begin("Project Editor")
        imgui_boost::Text("Edit Project.")
        imgui_boost::End()
    }

    def override _exit() : void {}
}