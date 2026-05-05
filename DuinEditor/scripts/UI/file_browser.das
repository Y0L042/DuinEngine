options gen2

require imgui_boost
require dn_log
require dn_gameobject
require dn_filesystem

require ../editor_context.das
require ../filemanager.das

var uid : int = 0

class FileBrowser : dn_GameObject {
    selected_path : string
    expanded_dirs : array<string>

    def override _draw_ui() : void {
        imgui_boost::Begin("File Browser")

        uid = 0
        var fm : FileManager? = filemanager_get()
        var root = fm?.root_node
        if (root != null) {
            for (child in root.children) {
                draw_node(child)
            }
        } else {
            fm.build_tree()
        }

        imgui_boost::End()
    }

    def draw_node(var node : FSNode?) : void {
        if (node == null) return

        if (node.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
            let opened : bool = imgui_boost::TreeNode(node.path, node.name)
            
            if (imgui_boost::IsItemClicked(0)) {
                selected_path = node.path
            }

            if (opened) {
                for (child in node.children) {
                    draw_node(child)
                }
                imgui_boost::TreePop()
            }
        } else {
            // file leaf
            let sel = imgui_boost::Selectable(node.name, selected_path == node.path)
            if (sel) { 
                selected_path = node.path 
            }
            if (imgui_boost::IsItemHovered() && imgui_boost::IsMouseDoubleClicked(0)) {
                // open file action (optional)
            }
        }
    }
}

