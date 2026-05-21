options gen2

module scenetree_viewer public

require daslib/delegate

require imgui_boost
require dn_gameobject
require dn_ecs

require ../scene.das

typedef OnEntitySelected = delegate(type<function<(var selected : DnEntity?) : void>>)

struct STNode {
    entity_id   : ecs_entity_t
    entity_name : string
    children    : array<STNode>
}
  
class ScenetreeViewer : DnGameObject {
    active_scene : Scene
    entity_tree : array<STNode>
    selected_entity : ecs_entity_t
    on_entity_selected <- OnEntitySelected()
   
    def ScenetreeViewer() : void {
        super()
    }

    def override _init() : void {
    }

    def override _ready() : void {
        on_entity_selected += @(var e : DnEntity) : void {
            dn_info("Selected Entity: {e}")
        }
    }

    def override _physics_update(delta : double) : void {
        update_tree()
    }

    def override _draw_ui() : void {
        imgui_boost::Begin("Scenetree Viewer", null, int(ImGuiWindowFlags.ImGuiWindowFlags_NoCollapse))

        for (node in entity_tree) {
            draw_entity_node(node)
        }

        imgui_boost::End()
    }

    def update_scene(var new_scn : Scene) : void {
        active_scene = new_scn
    }

    def update_tree() : void {
        clear(entity_tree)

        var w = active_scene.world
        if (w == null) { return ; }
        let ecs_w = w.ecs_world()
        if (ecs_w == null) { return ; }

        emplace(entity_tree, build_node(active_scene.root_entity, ecs_w))
    }

    def build_node(entity_id : ecs_entity_t, ecs_w : ecs_world_t?) : STNode {
        var node : STNode = STNode()
        node.entity_id   = entity_id
        node.entity_name = string(ecs_get_name(ecs_w, entity_id))
        ecs_children_each(ecs_w, entity_id, $(child : ecs_entity_t) : void {
            if (!ecs_has(ecs_w, child, EcsPrefab)) {
                emplace(node.children, build_node(child, ecs_w))
            }
        })

        return <- node
    }

    def draw_entity_node(var node : STNode&) : void {
        var flags = int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_OpenOnArrow) | int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_DefaultOpen)

        let display_name = node.entity_name == "" ? "Unnamed Entity" : node.entity_name
        let node_id = "{display_name}##{node.entity_id}"

        if (imgui_boost::IsItemClicked(0)) {
            selected_entity = node.entity_id
        }

        if (length(node.children) == 0) {
            flags |= int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_Leaf) | int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_NoTreePushOnOpen)
            imgui_boost::TreeNodeEx(node_id, flags, display_name)
        } else {
            if (imgui_boost::TreeNodeEx(node_id, flags, display_name)) {
                for (child in node.children) {
                    draw_entity_node(child)
                }
                imgui_boost::TreePop()
            }
        }
    }
}
