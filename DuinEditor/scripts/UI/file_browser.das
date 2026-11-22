options gen2

module file_browser public

require daslib/delegate
require daslib/ansi_colors

require imgui
require imgui/imgui_boost_runtime
require imgui/imgui_boost_v2
require imgui/imgui_widgets_builtin
require imgui/imgui_containers_builtin
require imgui/imgui_layout_builtin
require dn_log
require dn_gameobject
require dn_filesystem

require ../editor_context.das
require ../filemanager.das

typedef OnFileSelected = delegate(type<function<(var selected : FSNode?) : void>>)

var private FB_DIR : table<string; TreeNodeState>
var private FB_FILE : table<string; ClickState>

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
        window(FILE_BROWSER_WIN, (text = "File Browser", closable = false, flags = ImGuiWindowFlags.None)) {
            var fm : FileManager? = filemanager_get()
            var root = fm?.root_node
            if (root != null) {
                for (child in root.children) {
                    draw_node(child)
                }
            } else {
                fm.build_tree()
            }
        }
    }

    def draw_node(var node : FSNode?) : void {
        if (node == null) return

        if (node.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
            let opened : bool = tree_node_ex(FB_DIR[node.path], (text = "{node.name}##{node.path}", flags = ImGuiTreeNodeFlags.None))

            if (IsItemClicked(ImGuiMouseButton.Left)) {
                selected_path = node.path
            }

            if (opened) {
                for (child in node.children) {
                    draw_node(child)
                }
                tree_pop()
            }
        } else {
            // file leaf
            let sel = selectable_label(FB_FILE[node.path], (text = "{node.name}##{node.path}", selected = selected_path == node.path))
            if (sel) {
                selected_path = node.path
            }
            if (IsItemHovered() && IsMouseDoubleClicked(ImGuiMouseButton.Left)) {
                on_file_selected.invoke(node)
            }
        }
    }
}

