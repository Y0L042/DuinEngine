options gen2

module file_browser public

require daslib/delegate
require daslib/ansi_colors

require imgui_boost
require dn_log
require dn_gameobject
require dn_filesystem

require ../editor_context.das
require ../filemanager.das

typedef OnFileSelected = delegate(type<function<(var selected : FSNode?) : void>>)

class FileBrowser : DnGameObject {
    selected_path : string
    expanded_dirs : array<string>
    on_file_selected <- OnFileSelected()

    def FileBrowser() : void {
        super()
    }

    def override _init() : void {
        on_file_selected += @(var node : FSNode?) : void {
            dn_info("Selected file: {green_str(node.path)}")
        }
    }

    def override _ready() : void {
    }

    def subscribe_file_selected(lmb : lambda<(var node : FSNode?) : void>) : void {
        on_file_selected += lmb
    }

    def override _draw_ui() : void {
        imgui_boost::Begin("File Browser")

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
                on_file_selected.invoke(node)
            }
        }
    }
}

