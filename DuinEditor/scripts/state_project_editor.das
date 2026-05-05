options gen2

require daslib/class_boost

require imgui_boost
require dn_log
require dn_gamestatemachine
require dn_filesystem

require ./UI/file_browser.das

class State_ProjectEditor : dn_GameState {
    file_browser : FileBrowser?

    def State_ProjectEditor() : void {
        _alias = "State_ProjectEditor"
        dn_info("Entered {_alias}")

    }

    def override _enter() : void {
        file_browser = add_child(new FileBrowser())
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
