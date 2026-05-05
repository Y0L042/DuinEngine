options gen2
options always_call_super

require daslib/ansi_colors

require tomlc17
require imgui_boost
require dn_log
require dn_live_host
require dn_gamestatemachine
require dn_filesystem

require editor_context
require state_project_editor
require filemanager

class State_ProjectSelection : dn_GameState {
    // CFG Structure
    TAG_EDITOR_CONFIG : string const = "EditorConfig"
    TAG_CURRENT_PROJECT_DIR : string const = "CurrentProjectDir"
    TAG_RECENT_PROJECT_DIRS : string const = "RecentProjectDirs"

    EDITOR_CFG_PATH : string const = "data/editor_cfg.toml"
    PROJECT_FILE_NAME : string const = "duin.project"

    selected_project : string = "NONE"
    recent_project_dirs : array<string>

    def State_ProjectSelection() : void {
        super()

        _alias = "State_ProjectSelection"
        dn_info("Entered {_alias}")
    }

    def override _enter() : void {
        initialize_project_list(recent_project_dirs)
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
            load_selected_project(selected_project)
        }

        for (path in recent_project_dirs) {
            let project_selected : bool = imgui_boost::Selectable(path, selected_project == path)
            if (project_selected) {
                selected_project = path
            }
            if (imgui_boost::IsItemHovered() && imgui_boost::IsMouseDoubleClicked(0)) {
                dn_info("Load project [{path}]")
                load_selected_project(path)
            }
        }

        imgui_boost::End()
    }

    def override _exit() : void {}

    def initialize_project_list(var list : array<string>&) : void {
        // Read editor config
        let cfg_file : TomlResult = toml_parse_file(EDITOR_CFG_PATH)
        if (!cfg_file.ok) {
            dn_warn("Could not read editor config <{EDITOR_CFG_PATH}>!")
            return
        }
 
        // Ensure EditorConfig exists and is an object
        assume cfg_data  = cfg_file.toptab
        if (toml_seek_value(cfg_data, TAG_EDITOR_CONFIG) is _unknown) {
            dn_warn("Could not read {TAG_EDITOR_CONFIG} value <{EDITOR_CFG_PATH}>!")
            return
        }

        // Read RecentProjectDirs
        let dirs_toml : TomlValue = toml_seek_value(cfg_data, toml_append_paths(TAG_EDITOR_CONFIG, TAG_RECENT_PROJECT_DIRS))
        if (dirs_toml is _unknown) {
            dn_warn("{TAG_RECENT_PROJECT_DIRS} not found in {EDITOR_CFG_PATH}!")
            return
        }
        if (dirs_toml is _array) {
            let toml_arr : TomlDatum = dirs_toml as _array
            let arr_size : int = toml_datum_arr_size(toml_arr)
            for (i in range(0, arr_size)) {
                let elem : TomlDatum = toml_datum_arr_elem(toml_arr, i)
                push(list, string(toml_datum_str(elem)))
            }
        } else {
            dn_warn("RecentProjectDirs NOT an array!")
        }
    }

    def load_selected_project(project_dir : string) : void {
        // Check if selection is valid (not -1)
        if (project_dir.empty()) return
        
        var path_info : DnPathInfo
        var res : bool = dn_fs_get_path_info(project_dir, path_info)

        var project_info : DnPathInfo
        res = res && dn_fs_get_path_info(project_dir + "/" + PROJECT_FILE_NAME, project_info)

        if (!res) {
            dn_warn("Project file {project_dir} is invalid!")
            return
        }

        // If project is found:
        // 1. Set singleton active_project variable
        var new_ctx : EditorContext = get_editor_ctx()
        new_ctx.project_data = Project(path=project_dir)
        set_editor_ctx(new_ctx)
        set_working_dir(project_dir)

        // 2. Set filemanager root path so that the working project root points
        //      to the selected project

        // 3. Set imgui path, to have a per-project editor layout (optional, TODO)

        // 4. Switch state
        self.switch_state(new State_ProjectEditor())
    }
}
