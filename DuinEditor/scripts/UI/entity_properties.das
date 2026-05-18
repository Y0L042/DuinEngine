options gen2

module entity_properties public

require daslib/delegate

require imgui_boost
require dn_log
require dn_gameobject
require dn_ecs

require ../scene.das



class EntityProperties : DnGameObject {
    active_scene : Scene
    selected_entity : DnEntity?

    def EntityProperties() : void {
        super()
    }

    def override _init() : void {
    }

    def override _ready() : void {
    }

    def override _draw_ui() : void {
        imgui_boost::Begin("Entity Properties")
        imgui_boost::End()
    }

    def update_scene(var new_scn : Scene) : void {
        active_scene = new_scn
    }
}

