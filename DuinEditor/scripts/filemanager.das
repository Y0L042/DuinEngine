options gen2

module filemanager shared public

require daslib/static_let
require daslib/linq
require strings
require math

require dn_log
require dn_filesystem

enum SortType {
    SORT_BY_NAME = 0
}

enum GroupType {
    GROUP_BY_TYPE = 0          // directories first, then files
}

class FSNode {
    path : string
    name : string
    typev : DnPathType
    parent : FSNode?
    children : array<FSNode?>
}

var WORKING_DIR : string = ""
var g_filemanager_instance : FileManager? = new FileManager()

class FileManager {  
    workspace_path : string
    allowed_extensions : array<string>
    root_node : FSNode?
    sort_type : SortType = SortType.SORT_BY_NAME
    group_type : GroupType = GroupType.GROUP_BY_TYPE

    def build_tree() : void {
        // Clear tree recursively
        _clear_tree(root_node)

        root_node = new FSNode()
        _build_tree(WORKING_DIR, root_node)
        _sort_tree(root_node)
    }

    def recurse_tree(fun : block<(node : FSNode?) : void>) : void {
        if (root_node == null) {
            build_tree()
        }

        _recurse_tree(root_node, fun)
    }

    def _recurse_tree(node : FSNode?, fun : block<(node : FSNode?) : void>) : void {
        if (node == null) {
            return
        }

        for (child in node.children) {
            fun(child)
            if (child.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
                _recurse_tree(child, fun)
            }
        }
    }

    def _build_tree(path : string, var parent : FSNode?) : void {
        if (parent == null) {
            return
        }

        dn_fs_enumerate_directory(path) $(entry_dir, fname : string) {
            var base_dir : string

            if (ends_with(entry_dir, "\\") || ends_with(entry_dir, "/")) {
                base_dir = chop(entry_dir, 0, length(entry_dir) - 1)
            } else {
                base_dir = entry_dir
            }

            var full_path : string = base_dir + "/" + fname
            var info : DnPathInfo

            if (!dn_fs_get_path_info(full_path, info)) {
                return DnEnumerationResult.DNFS_ENUM_CONTINUE
            }

            if (info.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
                var dir_node : FSNode? = new FSNode()
                dir_node.path := clone_string(full_path) 
                dir_node.name := clone_string(fname)
                dir_node.typev = DnPathType.DNFS_PATHTYPE_DIRECTORY
                dir_node.parent = parent
                _build_tree(full_path, dir_node)          
                push_clone(parent.children, dir_node)

            } elif (info.typev == DnPathType.DNFS_PATHTYPE_FILE) {
                let ext = dn_fs_get_file_extension(fname)
                if (length(allowed_extensions) == 0 || find_index(allowed_extensions, ext) >= 0) {
                    // Construct file node
                    var leaf : FSNode? = new FSNode()
                    leaf.path := clone_string(full_path) 
                    leaf.name := clone_string(fname)
                    leaf.typev = DnPathType.DNFS_PATHTYPE_FILE
                    leaf.parent = parent
                    push(parent.children, leaf)
                }
            }

            return DnEnumerationResult.DNFS_ENUM_CONTINUE
        }
    }

    def _sort_tree(var node : FSNode?) : void {
        if (node == null) {
            return
        }

        // Sort children of this node
        _sort_children(node.children)

        // Recursively sort children of directories
        for (child in node.children) {
            if (child.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
                _sort_tree(child)
            }
        }
    }

    def _sort_children(var children : array<FSNode?>) : void {
        sort(children) <| $(a, b) {
            if (group_type == GroupType.GROUP_BY_TYPE) {
                // Group by type: directories first
                let a_is_dir = a.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY
                let b_is_dir = b.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY
                
                if (a_is_dir != b_is_dir) {
                    return a_is_dir  // directories come first
                }
            }
            
            // Within same group (or if not grouping): sort by name ascending
            return a.name < b.name
        }
    }

    def _clear_tree(var node : FSNode?) : void {
        if (node == null) {
            return
        }

        for (child in node.children) {
            if (child.typev == DnPathType.DNFS_PATHTYPE_DIRECTORY) {
                _clear_tree(child)
            }
            delete child
        }

        delete node
    }

}

def filemanager_get() : FileManager? {
    return g_filemanager_instance
}

def set_working_dir(new_dir : string) : void {
    WORKING_DIR = new_dir
}
