options gen2
require dastest/testing_boost public

require daslib/option

require dn_log
require dn_ecs
require dn_ecs_components

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

// Creates an entity that already carries a DnTransform3D so the global-space
// helpers have something to operate on.
def make_transform_entity(var w : DnWorld; name : string) : DnEntity {
    var e = w.create_entity(name)
    e.set(DnTransform3D())
    return e
}

// ---------------------------------------------------------------------------
// DnWorld lifecycle
// ---------------------------------------------------------------------------

[test]
def test_world_valid_after_init() {
    var w : DnWorld
    verify(w.initialize(), "DnWorld.initialize() returns true")
    verify(w.is_valid(), "DnWorld is valid after init")
}

[test]
def test_world_create_entity_nonzero() {
    var w = make_world()
    let e = w.create_entity("test_entity")
    verify(e.entity != 0ul, "create_entity returns non-zero entity")
}

[test]
def test_world_create_child_parents_entity() {
    var w = make_world()
    var parent = w.create_entity("parent")
    let child = w.create_child(parent, "child")
    verify(child.has_parent(), "create_child sets a parent")
    let pr = child.parent()
    verify(is_some(pr) && pr._value.entity == parent.entity, "create_child parent is the given entity")
}

[test]
def test_world_lookup_and_delete() {
    var w = make_world()
    var e = w.create_entity("lookup_me")
    verify(w.lookup("lookup_me").entity == e.entity, "lookup by name finds the entity")
    verify(w.is_alive(e), "entity alive before delete")
    w.delete_entity(e)
    verify(!w.is_alive(e), "entity not alive after delete")
}

[test]
def test_world_root_children_lists_top_level_only() {
    var w = make_world()
    var root = w.create_entity("top_level_root")
    w.create_child(root, "nested_child")

    var sawRoot = false
    var sawChild = false
    w.root_children() $(var c : DnEntity) {
        if (c.entity == root.entity) {
            sawRoot = true
        }
        if (c.get_name() == "nested_child") {
            sawChild = true
        }
    }
    verify(sawRoot, "root_children includes the root entity")
    verify(!sawChild, "root_children excludes nested children")
}

// ---------------------------------------------------------------------------
// Entity-level global-space transform helpers (DnTransform3D)
// ---------------------------------------------------------------------------

[test]
def test_entity_set_get_global_position() {
    var w = make_world()
    let e = make_transform_entity(w, "pos_entity")
    set_global_position(e, float3(1.0f, 2.0f, 3.0f))
    let pos = unwrap(get_global_position(e))
    verify(pos.x == 1.0f, "global position x matches")
    verify(pos.y == 2.0f, "global position y matches")
    verify(pos.z == 3.0f, "global position z matches")
}

[test]
def test_entity_set_get_global_scale() {
    var w = make_world()
    let e = make_transform_entity(w, "scale_entity")
    set_global_scale(e, float3(2.0f, 3.0f, 4.0f))
    let sc = unwrap(get_global_scale(e))
    verify(sc.x == 2.0f, "global scale x matches")
    verify(sc.y == 3.0f, "global scale y matches")
    verify(sc.z == 4.0f, "global scale z matches")
}

[test]
def test_entity_set_get_global_rotation() {
    var w = make_world()
    let e = make_transform_entity(w, "rot_entity")
    set_global_rotation(e, float4(0.0f, 0.0f, 0.0f, 1.0f))
    let rot = unwrap(get_global_rotation(e))
    verify(rot.x == 0.0f, "global rotation x matches")
    verify(rot.y == 0.0f, "global rotation y matches")
    verify(rot.z == 0.0f, "global rotation z matches")
    verify(rot.w == 1.0f, "global rotation w matches")
}

// ---------------------------------------------------------------------------
// World-level helpers act on the C++ Transform3D, not DnTransform3D
// ---------------------------------------------------------------------------

[test]
def test_world_helpers_do_not_touch_dn_transform() {
    var w = make_world()
    let e = make_transform_entity(w, "world_helper_entity")
    w.set_global_position(e, 1.0f, 2.0f, 3.0f)
    w.set_global_scale(e, 2.0f, 2.0f, 2.0f)

    let tx = e.get(type<DnTransform3D>)
    verify(tx.pos.x == 0.0f && tx.pos.y == 0.0f && tx.pos.z == 0.0f, "DnTransform3D position unchanged")
    verify(tx.scale.x == 1.0f && tx.scale.y == 1.0f && tx.scale.z == 1.0f, "DnTransform3D scale unchanged")

    let pos = w.get_global_position(e)
    verify(pos.x == 0.0f && pos.y == 0.0f && pos.z == 0.0f, "world helper reads zero without a C++ Transform3D")
}

[test]
def test_known_bug_world_helpers_move_dn_transform() {
    // KNOWN BUG: dn_world_set/get_global_* bind to the C++ Transform3D, so scripts
    // using DnTransform3D cannot use the DnWorld helpers.
    var w = make_world()
    let e = make_transform_entity(w, "world_helper_bug_entity")
    w.set_global_position(e, 1.0f, 2.0f, 3.0f)
    let pos = w.get_global_position(e)
    verify(pos.x == 1.0f && pos.y == 2.0f && pos.z == 3.0f, "DnWorld helpers round-trip on a DnTransform3D entity")
}
