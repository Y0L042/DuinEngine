options gen2

require imgui_boost
require dn_log

require dn_gamestatemachine

class State_ProjectSelection : dn_GameState {
    EDITOR_CFG_PATH : string const = "data/editor_cfg.toml"
    PROJECT_FILE_NAME : string const = "duin.project"

    selected_project : string = "NONE"
    recent_project_dirs : array<string>

    def State_ProjectSelection() : void {
        _alias = "State_ProjectSelection"

        recent_project_dirs.push("Proj1")
        recent_project_dirs.push("Proj2")
        recent_project_dirs.push("Proj3")
    }

    def override _enter() : void {
    }

    def override _update(delta : double) : void {
    }

    def override _physics_update(delta : double) : void {}

    def override _draw() : void {}

    def override _draw_ui() : void {
        imgui_boost::Begin("Project Selection Menu")
        imgui_boost::Text("Select project   ")

        if (imgui_boost::Button("Import Project")) {
            dn_info("Import project pressed!")
        }
        imgui_boost::SameLine()
        if (imgui_boost::Button("Load Project")) {
            dn_info("Load project pressed!")
        }

        for (path in recent_project_dirs) {
            let project_selected : bool = imgui_boost::Selectable(path, selected_project == path)
            if (project_selected) {
                selected_project = path
            }
            if (imgui_boost::IsItemHovered() && imgui_boost::IsMouseDoubleClicked(0)) {
                dn_info("Load project [{path}]")
            }
        }

        imgui_boost::End()
    }

    def override _exit() : void {}
}