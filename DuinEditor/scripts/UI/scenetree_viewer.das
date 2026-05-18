options gen2

module scenetree_viewer public

require imgui_boost
require dn_gameobject
require dn_ecs

require ../scene.das

typedef OnEntitySelected = delegate(type<function<(var selected : DnEntity) : void>>)

class STNode {
    entity_id   : ecs_entity_t
    entity_name : string
    children    : array<STNode?>
}

class ScenetreeViewer : DnGameObject {
    active_scene : Scene

    entity_tree : array<STNode?>

    def ScenetreeViewer() : void {
        super()
    }

    def override _init() : void {
    }

    def override _ready() : void {
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
        for (node in entity_tree) {
            unsafe {
                delete node
            }
        }
        clear(entity_tree)

        var w = active_scene.world
        if (w == null) { return ; }
        let ecs_w = w.ecs_world()
        if (ecs_w == null) { return ; }

        var root = build_node(active_scene.root_entity, ecs_w)
        push(entity_tree, root)

        // ecs_children_each(ecs_w, $(e : ecs_entity_t) : void {
        //     if (!ecs_has(ecs_w, e, EcsPrefab) && ecs_get_parent(ecs_w, e) == 0ul) {
        //         build_node(e, ecs_w)
        //     }
        // })

    }

    def build_node(entity_id : ecs_entity_t, ecs_w : ecs_world_t?) : STNode? {
        var node : STNode? = new STNode()
        node.entity_id   = entity_id
        node.entity_name = string(ecs_get_name(ecs_w, entity_id))
        // dn_info("Entity {node.entity_name} | {node.entity_id}")
        ecs_children_each(ecs_w, entity_id, $(child : ecs_entity_t) : void {
            if (!ecs_has(ecs_w, child, EcsPrefab)) {
                var tmpnode = build_node(child, ecs_w)
                push(node.children, tmpnode)
            }
        })

        return node
    }

    def draw_entity_node(var node : STNode?) : void {
        var flags = int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_OpenOnArrow) | int(ImGuiTreeNodeFlags.ImGuiTreeNodeFlags_DefaultOpen)

        let display_name = node.entity_name == "" ? "Unnamed Entity" : node.entity_name
        let node_id = "{display_name}##{node.entity_id}"

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
