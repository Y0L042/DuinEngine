options gen2

module scene shared public

require dn_uuid public
require dn_ecs public
require dn_ecs
require dn_gameworld
require dn_scenebuilder
require dn_log

require filemanager

struct Scene {
    uuid : DnUUID
    name : string
    world : DnGameWorld?
    pscn : DnPackedScene?
    root_entity : ecs_entity_t
}

def instantiate_scene_from_file(var sb : DnSceneBuilder?, path : string) : Scene {
    var scn : Scene

    scn.world = new DnGameWorld()
    scn.pscn = sb.deserialize_from_file(path)

    if (scn.pscn != null) {
        scn.root_entity = sb.instantiate(scn.pscn, scn.world)
        if (scn.root_entity == 0ul) {
            dn_warn("Instantiation of {path} failed!")
        }
    }

    return scn
}

def remove_scene(var scene : Scene&) : void {

}

def is_scene_empty(scene : Scene&) : bool {
    return scene.world == null
}
