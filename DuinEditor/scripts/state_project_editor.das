options gen2

require daslib/delegate
require daslib/class_boost
require daslib/json
require daslib/json_boost

require imgui_boost
require dn_log
require dn_uuid
require dn_gamestatemachine
require dn_filesystem
require dn_ecs
require dn_scenebuilder

require scene
require filemanager
require ./UI/file_browser.das
require ./UI/scenetree_viewer.das
require ./UI/scene_viewer.das
require ./UI/entity_properties.das

typedef OnActiveSceneChange = delegate(type<function<(var scene : Scene) : void>>)

class State_ProjectEditor : DnGameState {
    on_active_scene_change <- OnActiveSceneChange()

    file_browser : FileBrowser?
    scenetree_viewer : ScenetreeViewer?
    scene_viewer : SceneViewer?
    entity_properties : EntityProperties?

    scene_builder : DnSceneBuilder?
    open_scenes : table<DnUUID, Scene>
    active_scene : Scene

    def State_ProjectEditor() : void {
        _alias = "State_ProjectEditor"
        dn_info("Entered {_alias}")
    }

    def override _enter() : void {
        scene_builder = new DnSceneBuilder()
        file_browser = add_child(new FileBrowser())
        scenetree_viewer = add_child(new ScenetreeViewer())
        scene_viewer = add_child(new SceneViewer())
        entity_properties = add_child(new EntityProperties())

        file_browser.on_file_selected += @(var node : FSNode?) : void {
            on_file_selected(node)
        }
        scenetree_viewer.on_entity_selected += @(var e : DnEntity) : void {
            entity_properties.update_selected_entity(e)
        }
        on_active_scene_change += @(var scene : Scene) : void {
            scenetree_viewer.update_scene(scene)
        }

    }

    def override _update(delta : double) : void {
    }

    def override _physics_update(delta : double) : void { }

    def override _draw() : void {}

    def override _draw_ui() : void {
        imgui_boost::Begin("Project Editor")
        imgui_boost::Text("Edit Project.")
        imgui_boost::End()
    }

    def override _exit() : void {}

    def set_active_scene(var scene : Scene) : void {
        active_scene = scene
        on_active_scene_change.invoke(active_scene)
    }

    def on_file_selected(var node : FSNode?) : void {
        if (node.ext == "pscn") {
            handle_packedscene(node)
        }
    }

    def handle_packedscene(var node : FSNode?) : void {
        // load_packedscene()
        dn_info("Loading PackedScene {node.name}")
        ensure_instantiated_scene(node)
        dn_info("Finished loading PackedScene {node.name}")
    }

    def ensure_instantiated_scene(var node : FSNode?) : void {
        // Read scene UUID and Name from file
        let scn_file : string = dn_io_load_file_string(node.path)
        var error : string = ""
        var js : JsonValue? <- read_json(scn_file, error)

        var uuid : DnUUID = 0ul
        var scene_name : string = ""
        if (js != null) {
            var uuid_str : string = js?.sceneUUID ?? "" 
            if (uuid_str != "") {
                uuid = dn_uuid_from_string_hex(uuid_str)
            }

            scene_name = js?.sceneName ?? ""
        } else {
            dn_fatal("Failed to read Scene: {error}")
            return
        } 

        // Load scene from file
        var scn : Scene = instantiate_scene_from_file(scene_builder, node.path)

        scn.uuid = uuid
        scn.name = scene_name

        // Check if Scene is already loaded by comparing uuid (from file)
        // with map<uuid, scene>
        let is_open : bool = key_exists(open_scenes, scn.uuid)
        
        // Load scene if not already open
        if (!is_open) {
            open_scenes[scn.uuid] = scn
        }

        // Set as active scene
        set_active_scene(open_scenes[scn.uuid])

        dn_info("Set \"{scn.name}\" {scn.uuid} as Active Scene")
    }

    def remove_scene(uuid : DnUUID) : void {
        // Properly delete GameWorld, then remove scene
    }
}

