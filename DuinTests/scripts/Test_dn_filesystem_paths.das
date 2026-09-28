options gen2
require dastest/testing_boost public
require strings

require dn_log
require dn_filesystem

// Script-side coverage of the eng:// virtual prefix (engine root = repo's Duin/ folder),
// used by scripts and data files instead of machine-specific absolute paths.

[test]
def test_eng_prefix_is_virtual() {
    verify(dn_fs_is_virtual_path("eng://src/Duin"), "eng:// is a virtual path")
    verify(!dn_fs_is_virtual_path("eng:/src/Duin"), "eng:/ (single slash) is not virtual")
}

[test]
def test_eng_root_resolves() {
    let root = dn_fs_map_virtual_to_system("eng://")
    verify(!dn_fs_is_path_invalid(root), "eng:// resolves to the engine root")
    verify(ends_with(root, "/"), "engine root ends with a slash")
    verify(find(root, "\\") == -1, "engine root uses forward slashes")
}

[test]
def test_eng_daslang_marker_exists() {
    let marker = dn_fs_map_virtual_to_system("eng://vendor/daslang/daslib/builtin.das")
    var info : DnPathInfo
    verify(dn_fs_get_path_info(marker, info), "vendored daslang marker exists under eng://")
}

[test]
def test_eng_default_shaders_exist() {
    var info : DnPathInfo
    let vs = dn_fs_map_virtual_to_system("eng://src/Duin/Resources/shaders/dx11/vs_cubes.bin")
    let fs = dn_fs_map_virtual_to_system("eng://src/Duin/Resources/shaders/dx11/fs_cubes.bin")
    verify(dn_fs_get_path_info(vs, info), "default vertex shader exists under eng://")
    verify(dn_fs_get_path_info(fs, info), "default fragment shader exists under eng://")
}

[test]
def test_eng_round_trip() {
    let virt = "eng://src/Duin/Resources/shaders/dx11/vs_cubes.bin"
    let sys = dn_fs_map_virtual_to_system(virt)
    let back = dn_fs_map_system_to_virtual(sys)
    // wrk:// wins if a workspace was set inside the engine root; otherwise it maps back to eng://
    verify(back == virt || starts_with(back, "wrk://"), "eng:// round-trips through system paths")
}

[test]
def test_eng_repo_siblings_reachable() {
    // DuinEditor/data/editor_cfg.toml lists example projects as eng://../ExampleProjects/X
    var info : DnPathInfo
    let dir = dn_fs_map_virtual_to_system("eng://../ExampleProjects/DuinFPS")
    verify(dn_fs_get_path_info(dir, info), "eng://../ExampleProjects/DuinFPS exists")
}
