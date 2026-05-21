options gen2
options rtti = true

require daslib/rst
require daslib/fio
require daslib/rtti
require daslib/ast
require daslib/regex_boost
require daslib/random
require daslib/class_boost
require math
require strings

require imgui_boost public
require flecs_boost public
require tomlc17 public

require dn_log public
require dn_application public
require dn_gameobject public
require dn_gamestatemachine public
require dn_gameworld public
require dn_scenebuilder public
require dn_input public
require dn_camera public
require dn_filesystem public
require dn_uuid public
require dn_physics public
require dn_renderer public
require dn_ecs public


def push_if_exists(var arr : array<Module?>; name : string) {
    var m : Module? = find_module(name)
    if (m != null) {
        arr |> push(m)
    }
}

def document_log() {
    var mod : Module? = find_module("dn_log")
    if (mod == null) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnLog", mod, %regex~.*%%))
    document("Log", mod, "log.rst", groups)
}

def document_application() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_application_core")
    mods |> push_if_exists("dn_application")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnApplication_Core", find_module("dn_application_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnApplication",      find_module("dn_application"),      %regex~.*%%))
    documents("Application", mods, "application.rst", groups)
}

def document_gameobject() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_gameobject_core")
    mods |> push_if_exists("dn_gameobject_class")
    mods |> push_if_exists("dn_gameobject")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnGameObject_Core",  find_module("dn_gameobject_core"),  %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameObject_Class", find_module("dn_gameobject_class"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameObject",       find_module("dn_gameobject"),       %regex~.*%%))
    documents("GameObject", mods, "gameobject.rst", groups)
}

def document_gamestatemachine() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_gamestatemachine_core")
    mods |> push_if_exists("dn_gamestatemachine_class")
    mods |> push_if_exists("dn_gamestate_class")
    mods |> push_if_exists("dn_gamestatemachine")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnGameStateMachine_Core",  find_module("dn_gamestatemachine_core"),  %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameStateMachine_Class", find_module("dn_gamestatemachine_class"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameState_Class",        find_module("dn_gamestate_class"),        %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameStateMachine",       find_module("dn_gamestatemachine"),       %regex~.*%%))
    documents("GameStateMachine", mods, "gamestatemachine.rst", groups)
}

def document_gameworld() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_gameworld_core")
    mods |> push_if_exists("dn_gameworld_class")
    mods |> push_if_exists("dn_gameworld")
    mods |> push_if_exists("dn_scenebuilder_core")
    mods |> push_if_exists("dn_scenebuilder")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnGameWorld_Core",    find_module("dn_gameworld_core"),    %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameWorld_Class",   find_module("dn_gameworld_class"),   %regex~.*%%))
    groups |> emplace(group_by_regex("DnGameWorld",         find_module("dn_gameworld"),         %regex~.*%%))
    groups |> emplace(group_by_regex("DnSceneBuilder_Core", find_module("dn_scenebuilder_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnSceneBuilder",      find_module("dn_scenebuilder"),      %regex~.*%%))
    documents("GameWorld", mods, "gameworld.rst", groups)
}

def document_input() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_input_core")
    mods |> push_if_exists("dn_input")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnInput_Core", find_module("dn_input_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnInput",      find_module("dn_input"),      %regex~.*%%))
    documents("Input", mods, "input.rst", groups)
}

def document_renderer() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_camera_core")
    mods |> push_if_exists("dn_camera_impl")
    mods |> push_if_exists("dn_camera")
    mods |> push_if_exists("dn_renderer")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnCamera_Core", find_module("dn_camera_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnCamera_Impl", find_module("dn_camera_impl"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnCamera",      find_module("dn_camera"),      %regex~.*%%))
    groups |> emplace(group_by_regex("DnRenderer",    find_module("dn_renderer"),    %regex~.*%%))
    documents("Renderer", mods, "renderer.rst", groups)
}

def document_physics() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_physics_server_core")
    mods |> push_if_exists("dn_physics_server")
    mods |> push_if_exists("dn_characterbody_core")
    mods |> push_if_exists("dn_characterbody")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnPhysicsServer_Core", find_module("dn_physics_server_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnPhysicsServer",      find_module("dn_physics_server"),      %regex~.*%%))
    groups |> emplace(group_by_regex("DnCharacterBody_Core", find_module("dn_characterbody_core"),  %regex~.*%%))
    groups |> emplace(group_by_regex("DnCharacterBody",      find_module("dn_characterbody"),       %regex~.*%%))
    documents("Physics", mods, "physics.rst", groups)
}

def document_filesystem() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_filesystem_core")
    mods |> push_if_exists("dn_filesystem")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnFilesystem_Core", find_module("dn_filesystem_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnFilesystem",      find_module("dn_filesystem"),      %regex~.*%%))
    documents("Filesystem", mods, "filesystem.rst", groups)
}

def document_uuid() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_uuid_core")
    mods |> push_if_exists("dn_uuid")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnUUID_Core", find_module("dn_uuid_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnUUID",      find_module("dn_uuid"),      %regex~.*%%))
    documents("UUID", mods, "uuid.rst", groups)
}

def document_toml() {
    var mods : array<Module?>
    mods |> push_if_exists("tomlc17_core")
    mods |> push_if_exists("tomlc17")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("Tomlc17_Core", find_module("tomlc17_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("Tomlc17",      find_module("tomlc17"),      %regex~.*%%))
    documents("TOML", mods, "toml.rst", groups)
}

def document_imgui() {
    var mods : array<Module?>
    mods |> push_if_exists("imgui_core")
    mods |> push_if_exists("imgui_base")
    mods |> push_if_exists("imgui_boost")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("ImGui_Core",  find_module("imgui_core"),  %regex~.*%%))
    groups |> emplace(group_by_regex("ImGui_Base",  find_module("imgui_base"),  %regex~.*%%))
    groups |> emplace(group_by_regex("ImGui_Boost", find_module("imgui_boost"), %regex~.*%%))
    documents("ImGui", mods, "imgui.rst", groups)
}

def document_flecs() {
    var mods : array<Module?>
    mods |> push_if_exists("flecs_core")
    mods |> push_if_exists("flecs_boost")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("Flecs_Core",  find_module("flecs_core"),  %regex~.*%%))
    groups |> emplace(group_by_regex("Flecs_Boost", find_module("flecs_boost"), %regex~.*%%))
    documents("Flecs", mods, "flecs.rst", groups)
}

def document_ecs() {
    var mods : array<Module?>
    mods |> push_if_exists("dn_ecs_core")
    mods |> push_if_exists("dn_ecs")
    if (length(mods) == 0) { return; }
    var groups : array<DocGroup>
    groups |> emplace(group_by_regex("DnECS_Core", find_module("dn_ecs_core"), %regex~.*%%))
    groups |> emplace(group_by_regex("DnECS",      find_module("dn_ecs"),      %regex~.*%%))
    documents("ECS", mods, "ecs.rst", groups)
}

[export]
def game_update(delta : double) {}

[export]
def game_physics_update(delta : double) {}

[export]
def game_draw() {}

[export]
def game_draw_ui() {}

def touch_module_stub(mod_name : string) {
    let path = "{handmade_root}/handmade/module-{mod_name}.rst"
    mkdir_rec(dir_name(path))
    var st : FStat
    if (!stat(path, st)) {
        fopen(path, "wb") $(f) {
            if (f != null) {
                fwrite(f, "")
            }
        }
    }
}

[export]
def game_ready() {
    topic_root = "C:/Projects/CPP_Projects/Duin/DaslangDocGenerator/docs/generated"
    handmade_root = "C:/Projects/CPP_Projects/Duin/DaslangDocGenerator/docs/generated"
    rmdir_rec_result("{topic_root}/handmade")
    mkdir_rec(topic_root)
    to_log(LOG_INFO, "DaslangDocGenerator: writing RST to {topic_root}\n")

    touch_module_stub("dn_log")
    touch_module_stub("dn_application_core")
    touch_module_stub("dn_gameobject_core")
    touch_module_stub("dn_gamestatemachine_core")
    touch_module_stub("dn_gameworld_core")
    touch_module_stub("dn_input")
    touch_module_stub("dn_camera_core")
    touch_module_stub("dn_renderer")
    touch_module_stub("dn_physics_server_core")
    touch_module_stub("dn_characterbody_core")
    touch_module_stub("dn_filesystem_core")
    touch_module_stub("dn_uuid_core")
    touch_module_stub("tomlc17_core")
    touch_module_stub("imgui_core")
    touch_module_stub("flecs_core")
    touch_module_stub("decs")
    touch_module_stub("dn_ecs_core")
    touch_module_stub("dn_scenebuilder_core")

    document_log()
    document_application()
    document_gameobject()
    document_gamestatemachine()
    document_gameworld()
    document_input()
    document_renderer()
    document_physics()
    document_filesystem()
    document_uuid()
    document_toml()
    document_imgui()
    document_flecs()
    document_ecs()

    to_log(LOG_INFO, "DaslangDocGenerator: done.\n")
}
