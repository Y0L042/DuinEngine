options gen2
options rtti

module entity_properties public

require daslib/delegate
require daslib/rtti

require imgui
require imgui/imgui_boost_runtime
require imgui/imgui_boost_v2
require imgui/imgui_widgets_builtin
require imgui/imgui_containers_builtin
require imgui/imgui_scope_builtin
require dn_log
require dn_gameobject
require dn_ecs

require ../scene.das

var private COMP_HDR : table<string; CollapsingHeaderState>
var private COMP_SIZE : table<string; NarrativeState>


class EntityProperties : DnGameObject {
    selected_entity : DnEntity = DnEntity()
    active_scene : Scene

    def EntityProperties() : void {
        super()
    }

    def override _init() : void {
    }

    def override _ready() : void {
    }

    def override _draw_ui() : void {
        window(ENTITY_PROPS_WIN, (text = "Entity Properties", closable = false, flags = ImGuiWindowFlags.None)) {
            if (!selected_entity.is_valid()) {
                text_disabled("No entity selected")
            } else {
                draw_selected_entity()
            }
        }
    }

    def draw_selected_entity() : void {
        assume world = selected_entity.world
        assume entity = selected_entity.entity

        // --- Entity header ---
        let entity_name = string(ecs_get_name(world, entity))
        let display_name = entity_name == "" ? "Unnamed Entity" : entity_name
        separator_text(ENTITY_NAME_SEP, (text = display_name))
        text_disabled("ID: {entity}")
        spacing()

        // --- Collect ids into buckets ---
        var inscope tags  : array<ecs_entity_t>
        var inscope comps : array<ecs_entity_t>
        var inscope pairs : array<ecs_entity_t>

        ecs_entity_each(world, entity, $(id : ecs_entity_t) : void {
            if (ecs_id_is_pair(id)) {
                push(pairs, id)
            } elif (ecs_id_is_tag(world, id)) {
                push(tags, id)
            } else {
                push(comps, id)
            }
        })
        
        // --- Components ---
        for (comp_id in comps) {
            // imgui_boost::Text(string("{comp_id}"))
            let key = "{comp_id}"
            let ti = ecs_get_type_info(world, comp_id)
            let ti_name = ti != null ? string(ti.name) : ""
            let comp_name = ti_name != "" ? ti_name : ecs_get_path(world, comp_id)
            SetNextItemOpen(true, ImGuiCond.Once)
            collapsing_header(COMP_HDR[key], (text = "{comp_name}##{key}", closable = false, flags = ImGuiTreeNodeFlags.None)) {
                with_indent(0.0f) {
                    if (ti != null) {
                        text_disabled(COMP_SIZE[key], (text = "size: {ti.size} bytes"))
                    }
                }
            }
        }
        //
        // // --- Tags ---
        // if (length(tags) > 0) {
        //     imgui_boost::Spacing()
        //     imgui_boost::SetNextItemOpen(true, int(ImGuiCond.ImGuiCond_Once))
        //     if (imgui_boost::CollapsingHeader("Tags", int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_None))) {
        //         imgui_boost::Indent()
        //         for (tag_id in tags) {
        //             let lbl = ecs_get_path(world, tag_id)
        //             imgui_boost::Bullet()
        //             imgui_boost::SameLine()
        //             imgui_boost::Text(lbl)
        //         }
        //         imgui_boost::Unindent()
        //     }
        // }
        //
        // // --- Pairs (relationships) ---
        // if (length(pairs) > 0) {
        //     imgui_boost::Spacing()
        //     imgui_boost::SetNextItemOpen(true, int(ImGuiCond.ImGuiCond_Once))
        //     if (imgui_boost::CollapsingHeader("Relationships", int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_None))) {
        //         imgui_boost::Indent()
        //         for (pair_id in pairs) {
        //             let first_id    = flecs_pair_first(world, pair_id)
        //             let second_id   = flecs_pair_second(world, pair_id)
        //             let first_name  = ecs_get_path(world, first_id)
        //             let second_name = ecs_get_path(world, second_id)
        //             imgui_boost::Bullet()
        //             imgui_boost::SameLine()
        //             imgui_boost::Text("({first_name}, {second_name})")
        //         }
        //         imgui_boost::Unindent()
        //     }
        // }

        // delete tags
        // delete comps
        // delete pairs
    }

    def update_scene(var new_scn : Scene) : void {
        active_scene = new_scn
    }

    def update_selected_entity(var e : DnEntity) : void {
        selected_entity = e

        assume world = selected_entity.world
        if (selected_entity.is_valid()) {
            ecs_entity_each(selected_entity.world, selected_entity.entity, $(id : ecs_entity_t) : void {
                if (ecs_id_is_pair(id)) {
                    dn_info("Entity {e.entity} Pair: {id}")
                } elif (ecs_id_is_tag(world, id)) {
                    dn_info("Entity {e.entity} Tag: {id}")
                } else {
                    dn_info("Entity {e.entity} Comp: {id}")
                }
            })
        }
    }
}
