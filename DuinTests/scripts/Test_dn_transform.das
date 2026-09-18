options gen2
require dastest/testing_boost public

require math
require daslib/option

require dn_log
require dn_ecs
require dn_ecs_components

let EPS = 0.0001f

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

def make_tx_entity(var w : DnWorld; name : string) : DnEntity {
    var e = w.create_entity(name)
    e |> set(DnTransform3D())
    return e
}

def approx_f(a, b : float) : bool {
    return abs(a - b) < EPS
}

def approx3(a, b : float3) : bool {
    return approx_f(a.x, b.x) && approx_f(a.y, b.y) && approx_f(a.z, b.z)
}

def approx4(a, b : float4) : bool {
    return approx_f(a.x, b.x) && approx_f(a.y, b.y) && approx_f(a.z, b.z) && approx_f(a.w, b.w)
}

// ---------------------------------------------------------------------------
// Defaults
// ---------------------------------------------------------------------------

[test]
def test_default_transform_is_identity() {
    var t : DnTransform3D
    verify(approx3(t.pos, float3(0.0f, 0.0f, 0.0f)), "default pos is zero")
    verify(approx4(t.rot, float4(0.0f, 0.0f, 0.0f, 1.0f)), "default rot is identity quaternion")
    verify(approx3(t.scale, float3(1.0f, 1.0f, 1.0f)), "default scale is one")
}

[test]
def test_default_transform_caches_start_dirty() {
    var t : DnTransform3D
    verify(t.gPosCacheDirtyFlag, "position cache starts dirty")
    verify(t.gRotCacheDirtyFlag, "rotation cache starts dirty")
    verify(t.gScaleCacheDirtyFlag, "scale cache starts dirty")
}

// ---------------------------------------------------------------------------
// quat_inverse vs quat_conjugate
// ---------------------------------------------------------------------------

[test]
def test_quat_inverse_matches_conjugate_for_unit_quat() {
    let q = quat_from_euler(0.3f, 0.5f, 0.7f)
    verify(approx4(quat_inverse(q), quat_conjugate(q)), "inverse == conjugate for a unit quaternion")
}

[test]
def test_quat_inverse_differs_from_conjugate_for_non_unit_quat() {
    let q = float4(0.0f, 0.0f, 0.0f, 2.0f)
    let inv = quat_inverse(q)
    let con = quat_conjugate(q)
    verify(!approx4(inv, con), "inverse != conjugate for a non-unit quaternion")
    verify(approx_f(inv.w, 0.25f), "inverse divides by length squared")
}

[test]
def test_quat_inverse_of_zero_is_identity() {
    let inv = quat_inverse(float4(0.0f, 0.0f, 0.0f, 0.0f))
    verify(approx4(inv, float4(0.0f, 0.0f, 0.0f, 1.0f)), "inverse of a zero quaternion is identity")
}

// ---------------------------------------------------------------------------
// Root entities: global == local
// ---------------------------------------------------------------------------

[test]
def test_root_global_equals_local() {
    var w = make_world()
    var e = w.create_entity("root")
    e |> set(DnTransform3D(pos = float3(1.0f, 2.0f, 3.0f), scale = float3(2.0f, 2.0f, 2.0f)))

    let g = get_global_transform(e)
    verify(approx3(g.pos, float3(1.0f, 2.0f, 3.0f)), "root global pos == local pos")
    verify(approx3(g.scale, float3(2.0f, 2.0f, 2.0f)), "root global scale == local scale")
}

[test]
def test_entity_without_transform_returns_identity() {
    var w = make_world()
    var e = w.create_entity("bare")

    verify(approx3(get_global_position(e), float3(0.0f, 0.0f, 0.0f)), "missing transform yields zero position")
    verify(approx4(get_global_rotation(e), float4(0.0f, 0.0f, 0.0f, 1.0f)), "missing transform yields identity rotation")
    verify(approx3(get_global_scale(e), float3(1.0f, 1.0f, 1.0f)), "missing transform yields unit scale")
}

// ---------------------------------------------------------------------------
// One level of hierarchy
// ---------------------------------------------------------------------------

[test]
def test_child_position_offsets_by_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(pos = float3(10.0f, 0.0f, 0.0f)))

    var child = w.create_entity("child")
    child |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    child |> child_of(parent)

    verify(approx3(get_global_position(child), float3(11.0f, 0.0f, 0.0f)), "child global pos = parent + local")
}

[test]
def test_child_position_scales_by_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(scale = float3(2.0f, 2.0f, 2.0f)))

    var child = w.create_entity("child")
    child |> set(DnTransform3D(pos = float3(3.0f, 0.0f, 0.0f)))
    child |> child_of(parent)

    verify(approx3(get_global_position(child), float3(6.0f, 0.0f, 0.0f)), "parent scale multiplies child offset")
}

[test]
def test_child_position_rotates_by_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(rot = quat_from_euler(0.0f, PI * 0.5f, 0.0f)))

    var child = w.create_entity("child")
    child |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    child |> child_of(parent)

    let g = get_global_position(child)
    verify(approx_f(length(g), 1.0f), "rotation preserves distance from parent")
    verify(!approx3(g, float3(1.0f, 0.0f, 0.0f)), "parent rotation moves the child off its local axis")
}

[test]
def test_child_global_scale_multiplies() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(scale = float3(2.0f, 3.0f, 4.0f)))

    var child = w.create_entity("child")
    child |> set(DnTransform3D(scale = float3(5.0f, 5.0f, 5.0f)))
    child |> child_of(parent)

    verify(approx3(get_global_scale(child), float3(10.0f, 15.0f, 20.0f)), "global scale is the product")
}

// ---------------------------------------------------------------------------
// Two levels
// ---------------------------------------------------------------------------

[test]
def test_three_level_chain_accumulates_position() {
    var w = make_world()
    var root = w.create_entity("root")
    root |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))

    var mid = w.create_entity("mid")
    mid |> set(DnTransform3D(pos = float3(2.0f, 0.0f, 0.0f)))
    mid |> child_of(root)

    var leaf = w.create_entity("leaf")
    leaf |> set(DnTransform3D(pos = float3(4.0f, 0.0f, 0.0f)))
    leaf |> child_of(mid)

    verify(approx3(get_global_position(leaf), float3(7.0f, 0.0f, 0.0f)), "positions accumulate down the chain")
}

// ---------------------------------------------------------------------------
// set_global_* round trips
// ---------------------------------------------------------------------------

[test]
def test_set_global_position_round_trips_on_root() {
    var w = make_world()
    var e = make_tx_entity(w, "root")

    set_global_position(e, float3(5.0f, 6.0f, 7.0f))
    verify(approx3(get_global_position(e), float3(5.0f, 6.0f, 7.0f)), "set then get returns the same global position")
}

[test]
def test_set_global_position_round_trips_under_rotated_scaled_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(pos = float3(1.0f, 2.0f, 3.0f),
                                rot = quat_from_euler(0.3f, 0.5f, 0.0f),
                                scale = float3(2.0f, 2.0f, 2.0f)))

    var child = make_tx_entity(w, "child")
    child |> child_of(parent)

    let target = float3(9.0f, -4.0f, 2.5f)
    set_global_position(child, target)
    verify(approx3(get_global_position(child), target), "global position round trips through a rotated+scaled parent")
}

[test]
def test_set_global_scale_round_trips_under_scaled_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(scale = float3(2.0f, 4.0f, 5.0f)))

    var child = make_tx_entity(w, "child")
    child |> child_of(parent)

    let target = float3(6.0f, 8.0f, 10.0f)
    set_global_scale(child, target)
    verify(approx3(get_global_scale(child), target), "global scale round trips through a scaled parent")
}

[test]
def test_set_global_rotation_round_trips_under_rotated_parent() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(rot = quat_from_euler(0.2f, 0.4f, 0.6f)))

    var child = make_tx_entity(w, "child")
    child |> child_of(parent)

    let target = quat_from_euler(0.7f, 0.1f, 0.3f)
    set_global_rotation(child, target)

    let g = get_global_rotation(child)
    let flipped = float4(0.0f - target.x, 0.0f - target.y, 0.0f - target.z, 0.0f - target.w)
    verify(approx4(g, target) || approx4(g, flipped), "global rotation round trips through a rotated parent")
}

[test]
def test_set_global_transform_makes_child_local_relative() {
    var w = make_world()
    var parent = w.create_entity("parent")
    parent |> set(DnTransform3D(pos = float3(10.0f, 0.0f, 0.0f)))

    var child = make_tx_entity(w, "child")
    child |> child_of(parent)

    set_global_position(child, float3(12.0f, 0.0f, 0.0f))

    let local = local_transform(child)
    verify(approx3(local.pos, float3(2.0f, 0.0f, 0.0f)), "local position is stored relative to the parent")
}

// ---------------------------------------------------------------------------
// Cache invalidation
// ---------------------------------------------------------------------------

[test]
def test_child_global_updates_after_parent_moves() {
    var w = make_world()
    var parent = make_tx_entity(w, "parent")
    var child = w.create_entity("child")
    child |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    child |> child_of(parent)

    verify(approx3(get_global_position(child), float3(1.0f, 0.0f, 0.0f)), "child starts at parent origin + local")

    set_global_position(parent, float3(100.0f, 0.0f, 0.0f))

    verify(approx3(get_global_position(child), float3(101.0f, 0.0f, 0.0f)),
           "child global position follows the parent instead of returning a stale cache")
}

[test]
def test_grandchild_global_updates_after_root_moves() {
    var w = make_world()
    var root = make_tx_entity(w, "root")

    var mid = w.create_entity("mid")
    mid |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    mid |> child_of(root)

    var leaf = w.create_entity("leaf")
    leaf |> set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    leaf |> child_of(mid)

    verify(approx3(get_global_position(leaf), float3(2.0f, 0.0f, 0.0f)), "leaf starts at the accumulated offset")

    set_global_position(root, float3(10.0f, 0.0f, 0.0f))

    verify(approx3(get_global_position(leaf), float3(12.0f, 0.0f, 0.0f)),
           "invalidation reaches descendants, not just direct children")
}

[test]
def test_invalidate_global_cache_marks_entity_dirty() {
    var w = make_world()
    var e = make_tx_entity(w, "e")

    let ignored = get_global_transform(e)
    unsafe {
        let raw = e.get_mut(type<DnTransform3D>)
        verify(raw != null, "transform is present")
        verify(!raw.gPosCacheDirtyFlag, "reading the global transform clears the dirty flag")
    }

    invalidate_global_cache(e)
    unsafe {
        let raw = e.get_mut(type<DnTransform3D>)
        verify(raw.gPosCacheDirtyFlag, "invalidate_global_cache re-dirties the flag")
    }
}

// ---------------------------------------------------------------------------
// parent() correctness (regression: used to return the entity's own id)
// ---------------------------------------------------------------------------

[test]
def test_parent_returns_the_parent_not_self() {
    var w = make_world()
    var parent = w.create_entity("parent")
    var child = w.create_entity("child")
    child |> child_of(parent)

    verify(child.parent() |> is_some(), "child reports a parent")
    var p = child.parent().move_unwrap()
    verify(p.ecs_entity() == parent.ecs_entity(), "parent() returns the parent's id")
    verify(p.ecs_entity() != child.ecs_entity(), "parent() does not return the child's own id")
}

[test]
def test_root_has_no_parent() {
    var w = make_world()
    var e = w.create_entity("root")
    verify(!(e.parent() |> is_some()), "a root entity has no parent")
}

// ---------------------------------------------------------------------------
// DnEntity.has
// ---------------------------------------------------------------------------

[test]
def test_has_reports_component_presence() {
    var w = make_world()
    var e = w.create_entity("e")

    verify(!e.has(type<DnTransform3D>), "entity without the component reports false")
    e |> set(DnTransform3D())
    verify(e.has(type<DnTransform3D>), "entity with the component reports true")
    verify(!e.has(type<DnVelocity3D>), "unrelated component still reports false")
}

// ---------------------------------------------------------------------------
// Registration
// ---------------------------------------------------------------------------

[test]
def test_register_internal_components_is_idempotent() {
    var w = make_world()
    dn_register_internal_components(w)
    let first = w.dn_register_component(type<DnTransform3D>)
    dn_register_internal_components(w)
    let second = w.dn_register_component(type<DnTransform3D>)
    verify(first == second, "re-registering a component yields the same id")
}
