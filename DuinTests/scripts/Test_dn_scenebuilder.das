options gen2
require dastest/testing_boost public

require dn_log
require dn_ecs
require dn_scenebuilder

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

def count_named(var w : DnWorld; name : string) : int {
    var count = 0
    w.root_children() $(var c : DnEntity) {
        if (c.get_name() == name) {
            count++
        }
        c.children_recursive() $(var d : DnEntity) {
            if (d.get_name() == name) {
                count++
            }
        }
    }
    return count
}

[test]
def test_pack_world_returns_packed_scene() {
    var w = make_world()
    w.create_entity("sb_root")

    var builder = new DnSceneBuilder()
    var scene = builder.pack_world(w)
    verify(scene != null, "pack_world returns a packed scene")
    verify(scene._handle != null, "packed scene has a native handle")

    unsafe {
        delete scene
        delete builder
    }
}

[test]
def test_instantiate_into_second_world_copies_entities() {
    var src = make_world()
    var root = src.create_entity("sb_copy_root")
    src.create_child(root, "sb_copy_child")

    var builder = new DnSceneBuilder()
    var scene = builder.pack_world(src)

    var dst = make_world()
    let instantiated = builder.instantiate(scene, dst)
    verify(instantiated != 0ul, "instantiate returns a root entity id")
    verify(count_named(dst, "sb_copy_root") == 1, "root entity copied into the destination world")
    verify(count_named(dst, "sb_copy_child") == 1, "child entity copied into the destination world")

    unsafe {
        delete scene
        delete builder
    }
}

[test]
def test_instantiate_as_children_parents_under_entity() {
    var src = make_world()
    src.create_entity("sb_child_scene_root")

    var builder = new DnSceneBuilder()
    var scene = builder.pack_world(src)

    var dst = make_world()
    var holder = dst.create_entity("sb_holder")
    let before = holder.child_count()
    let result = builder.instantiate_as_children(scene, holder.entity, dst)
    verify(result != 0ul, "instantiate_as_children returns a non-zero id")
    verify(holder.child_count() > before, "instantiated scene is parented under the holder")

    unsafe {
        delete scene
        delete builder
    }
}

[test]
def test_instantiate_with_null_scene_returns_zero() {
    var w = make_world()
    var builder = new DnSceneBuilder()
    var none : DnPackedScene? = null
    verify(builder.instantiate(none, w) == 0ul, "instantiate with a null scene returns 0")
    verify(builder.instantiate_as_children(none, 0ul, w) == 0ul, "instantiate_as_children with a null scene returns 0")
    unsafe {
        delete builder
    }
}
