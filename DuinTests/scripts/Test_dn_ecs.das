options gen2
require dastest/testing_boost public

require strings
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

// ---------------------------------------------------------------------------
// DnWorld
// ---------------------------------------------------------------------------

[test]
def test_world_initialize() {
    var w : DnWorld
    let ok = w.initialize()
    verify(ok, "DnWorld.initialize() returns true")
}

[test]
def test_world_is_valid_after_init() {
    var w : DnWorld
    w.initialize()
    verify(w.is_valid(), "DnWorld is valid after initialize")
}

[test]
def test_world_not_valid_before_init() {
    var w : DnWorld
    verify(!w.is_valid(), "DnWorld is not valid before initialize")
}

[test]
def test_world_ecs_world_not_null() {
    var w : DnWorld
    w.initialize()
    verify(w.ecs_world() != null, "DnWorld.ecs_world() is non-null after initialize")
}

[test]
def test_world_create_entity_nonzero() {
    var w = make_world()
    var e = w.create_entity("foo")
    verify(e.entity != 0ul, "create_entity returns non-zero entity id")
}

[test]
def test_world_create_entity_world_pointer() {
    var w = make_world()
    var e = w.create_entity("bar")
    verify(e.world != null, "created entity has non-null world pointer")
}

[test]
def test_world_is_alive_after_create() {
    var w = make_world()
    var e = w.create_entity("alive_test")
    verify(w.is_alive(e), "newly created entity is alive")
}

[test]
def test_world_delete_entity() {
    var w = make_world()
    var e = w.create_entity("to_delete")
    w.delete_entity(e)
    verify(!w.is_alive(e), "deleted entity is not alive")
}

[test]
def test_world_create_prefab_nonzero() {
    var w = make_world()
    var p = w.create_prefab("MyPrefab")
    verify(p.entity != 0ul, "create_prefab returns non-zero entity id")
}

[test]
def test_world_create_prefab_is_alive() {
    var w = make_world()
    var p = w.create_prefab("MyPrefab2")
    verify(w.is_alive(p), "prefab entity is alive after creation")
}

// ---------------------------------------------------------------------------
// DnEntity — identity and validity
// ---------------------------------------------------------------------------

[test]
def test_entity_is_valid_after_create() {
    var w = make_world()
    var e = w.create_entity("valid_e")
    verify(e.is_valid(), "entity is_valid() true after creation")
}

[test]
def test_entity_is_alive_method() {
    var w = make_world()
    var e = w.create_entity("alive_e")
    verify(e.is_alive(), "entity is_alive() true after creation")
}

[test]
def test_entity_ecs_entity_accessor() {
    var w = make_world()
    var e = w.create_entity("accessor_e")
    verify(e.ecs_entity() == e.entity, "ecs_entity() matches .entity field")
}

[test]
def test_entity_ecs_world_accessor() {
    var w = make_world()
    var e = w.create_entity("world_acc_e")
    verify(e.ecs_world() == e.world, "ecs_world() matches .world field")
}

[test]
def test_entity_ecs_world_not_null() {
    var w = make_world()
    var e = w.create_entity("world_notnull_e")
    verify(e.ecs_world() != null, "ecs_world() is non-null")
}

// ---------------------------------------------------------------------------
// DnEntity — name
// ---------------------------------------------------------------------------

[test]
def test_entity_get_name_matches_create() {
    var w = make_world()
    var e = w.create_entity("named_one")
    let n = e.get_name()
    verify(n == "named_one", "get_name() matches name given to create_entity")
}

[test]
def test_entity_set_name() {
    var w = make_world()
    var e = w.create_entity("original")
    e.set_name("renamed")
    let n = e.get_name()
    verify(n == "renamed", "get_name() reflects name set via set_name()")
}

// ---------------------------------------------------------------------------
// DnEntity — enable/disable
// ---------------------------------------------------------------------------

[test]
def test_entity_enable_disable() {
    var w = make_world()
    var e = w.create_entity("enable_e")
    e.enable(false)
    // Entity should still be alive after disable
    verify(w.is_alive(e), "entity is still alive after enable(false)")
    e.enable(true)
    verify(w.is_alive(e), "entity is still alive after re-enable")
}

// ---------------------------------------------------------------------------
// DnEntity — tags
// ---------------------------------------------------------------------------

[test]
def test_entity_add_and_has_tag() {
    var w = make_world()
    var tag = w.create_entity("Tag_A")
    var e = w.create_entity("tagged_e")
    e.add_tag(tag.entity)
    verify(e.has_tag(tag.entity), "entity has tag after add_tag")
}

[test]
def test_entity_remove_tag() {
    var w = make_world()
    var tag = w.create_entity("Tag_B")
    var e = w.create_entity("tagged_e2")
    e.add_tag(tag.entity)
    e.remove_tag(tag.entity)
    verify(!e.has_tag(tag.entity), "entity no longer has tag after remove_tag")
}

[test]
def test_entity_has_tag_false_by_default() {
    var w = make_world()
    var tag = w.create_entity("Tag_C")
    var e = w.create_entity("untagged_e")
    verify(!e.has_tag(tag.entity), "entity does not have tag before add_tag")
}

[test]
def test_entity_add_multiple_tags() {
    var w = make_world()
    var tag1 = w.create_entity("Tag_M1")
    var tag2 = w.create_entity("Tag_M2")
    var e = w.create_entity("multi_tagged")
    e.add_tag(tag1.entity)
    e.add_tag(tag2.entity)
    verify(e.has_tag(tag1.entity), "entity has tag1")
    verify(e.has_tag(tag2.entity), "entity has tag2")
}

[test]
def test_entity_remove_one_of_multiple_tags() {
    var w = make_world()
    var tag1 = w.create_entity("Tag_R1")
    var tag2 = w.create_entity("Tag_R2")
    var e = w.create_entity("partial_remove")
    e.add_tag(tag1.entity)
    e.add_tag(tag2.entity)
    e.remove_tag(tag1.entity)
    verify(!e.has_tag(tag1.entity), "tag1 removed")
    verify(e.has_tag(tag2.entity), "tag2 still present")
}

// ---------------------------------------------------------------------------
// DnEntity — destruct
// ---------------------------------------------------------------------------

[test]
def test_entity_destruct() {
    var w = make_world()
    var e = w.create_entity("destruct_e")
    e.destruct()
    verify(!w.is_alive(e), "entity not alive after destruct()")
}

// ---------------------------------------------------------------------------
// DnEntity — child_of
// ---------------------------------------------------------------------------

[test]
def test_entity_child_of() {
    var w = make_world()
    var parent = w.create_entity("parent_e")
    var child = w.create_entity("child_e")
    child.child_of(parent)
    // Both entities should remain alive
    verify(w.is_alive(parent), "parent alive after child_of")
    verify(w.is_alive(child), "child alive after child_of")
}

// ---------------------------------------------------------------------------
// DnEntity — is_a (prefab inheritance)
// ---------------------------------------------------------------------------

[test]
def test_entity_is_a_prefab() {
    var w = make_world()
    var prefab = w.create_prefab("BasePrefab")
    var e = w.create_entity("instance_e")
    e.is_a(prefab.entity)
    verify(w.is_alive(e), "entity alive after is_a()")
}

// ---------------------------------------------------------------------------
// DnEntity — component set/get/has/remove
// ---------------------------------------------------------------------------

struct EcsTestTag {}

struct EcsTestValue {
    v : int
}

[test]
def test_entity_set_get_transform3d() {
    var w = make_world()
    var e = w.create_entity("tx_e")
    e.set(DnTransform3D(pos = float3(1.0f, 2.0f, 3.0f), scale = float3(4.0f, 5.0f, 6.0f)))
    let t = e.get(type<DnTransform3D>)
    verify(t.pos.x == 1.0f && t.pos.y == 2.0f && t.pos.z == 3.0f, "DnTransform3D pos round-trips")
    verify(t.scale.x == 4.0f && t.scale.y == 5.0f && t.scale.z == 6.0f, "DnTransform3D scale round-trips")
    verify(t.rot.w == 1.0f, "DnTransform3D rot keeps identity default")
}

[test]
def test_entity_set_overwrites_component() {
    var w = make_world()
    var e = w.create_entity("overwrite_e")
    e.set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))
    e.set(DnTransform3D(pos = float3(9.0f, 0.0f, 0.0f)))
    verify(e.get(type<DnTransform3D>).pos.x == 9.0f, "second set overwrites the first")
}

[test]
def test_entity_set_get_velocity3d() {
    var w = make_world()
    var e = w.create_entity("vel_e")
    e.set(DnVelocity3D(lin = float3(1.0f, -2.0f, 3.0f), rot = float4(0.0f, 0.0f, 0.0f, 1.0f)))
    let v = e.get(type<DnVelocity3D>)
    verify(v.lin.x == 1.0f && v.lin.y == -2.0f && v.lin.z == 3.0f, "DnVelocity3D lin round-trips")
    verify(v.rot.w == 1.0f, "DnVelocity3D rot round-trips")
}

[test]
def test_entity_get_missing_component_returns_default() {
    var w = make_world()
    var e = w.create_entity("missing_e")
    let t = e.get(type<DnTransform3D>)
    verify(t.scale.x == 1.0f && t.pos.x == 0.0f, "get on a missing component returns the default value")
}

[test]
def test_entity_get_mut_modifies_in_place() {
    var w = make_world()
    var e = w.create_entity("mut_e")
    e.set(DnTransform3D())
    unsafe {
        var t = e.get_mut(type<DnTransform3D>)
        verify(t != null, "get_mut returns a pointer for a present component")
        t.pos.y = 5.0f
    }
    verify(e.get(type<DnTransform3D>).pos.y == 5.0f, "get_mut writes are visible to get")
}

[test]
def test_entity_get_mut_missing_is_null() {
    var w = make_world()
    var e = w.create_entity("mut_missing_e")
    verify(e.get_mut(type<DnVelocity3D>) == null, "get_mut on a missing component is null")
}

[test]
def test_entity_has_and_remove() {
    var w = make_world()
    var e = w.create_entity("has_e")
    verify(!e.has(type<EcsTestValue>), "has is false before set")
    e.set(EcsTestValue(v = 3))
    verify(e.has(type<EcsTestValue>), "has is true after set")
    e.remove(type<EcsTestValue>)
    verify(!e.has(type<EcsTestValue>), "has is false after remove")
}

[test]
def test_entity_add_zero_size_tag() {
    var w = make_world()
    var e = w.create_entity("tag_struct_e")
    e.add(EcsTestTag())
    verify(e.has(type<EcsTestTag>), "zero-size struct can be added as a tag")
    e.remove(type<EcsTestTag>)
    verify(!e.has(type<EcsTestTag>), "zero-size tag can be removed")
}

[test]
def test_entity_add_with_value_stores_value() {
    var w = make_world()
    var e = w.create_entity("add_value_e")
    e.add(EcsTestValue(v = 11))
    verify(e.get(type<EcsTestValue>).v == 11, "add with a value stores the value")
}

[test]
def test_invalid_entity_component_calls_are_safe() {
    var e = DnEntity()
    verify(!e.is_valid(), "default DnEntity is invalid")
    e.set(EcsTestValue(v = 1))
    e.remove(type<EcsTestValue>)
    verify(!e.has(type<EcsTestValue>), "has on an invalid entity is false")
    verify(e.get_mut(type<EcsTestValue>) == null, "get_mut on an invalid entity is null")
}

// ---------------------------------------------------------------------------
// Component struct defaults
// ---------------------------------------------------------------------------

[test]
def test_dntransform3d_defaults() {
    let t = DnTransform3D()
    verify(t.pos.x == 0.0f && t.pos.y == 0.0f && t.pos.z == 0.0f, "default pos is zero")
    verify(t.scale.x == 1.0f && t.scale.y == 1.0f && t.scale.z == 1.0f, "default scale is one")
    verify(t.rot.x == 0.0f && t.rot.w == 1.0f, "default rot is identity")
    verify(t.gPosCacheDirtyFlag && t.gRotCacheDirtyFlag && t.gScaleCacheDirtyFlag, "caches start dirty")
}

[test]
def test_dnvelocity3d_defaults() {
    let v = DnVelocity3D()
    verify(v.lin.x == 0.0f && v.lin.y == 0.0f && v.lin.z == 0.0f, "default linear velocity is zero")
    verify(v.rot.w == 1.0f, "default angular velocity is the identity quaternion")
}

// ---------------------------------------------------------------------------
// DnEntity — hierarchy
// ---------------------------------------------------------------------------

[test]
def test_entity_parent_and_has_parent() {
    var w = make_world()
    var parent = w.create_entity("h_parent")
    var child = w.create_entity("h_child")
    verify(!child.has_parent(), "fresh entity has no parent")
    verify(is_none(child.parent()), "parent() of a root is none")

    child.child_of(parent)
    verify(child.has_parent(), "child_of sets a parent")
    let p = child.parent()
    verify(is_some(p) && p._value.entity == parent.entity, "parent() returns the parent")
}

[test]
def test_entity_set_parent_and_remove_parent() {
    var w = make_world()
    var a = w.create_entity("sp_a")
    var b = w.create_entity("sp_b")
    var child = w.create_entity("sp_child")

    child.set_parent(a)
    verify(child.parent()._value.entity == a.entity, "set_parent attaches to the first parent")
    child.set_parent(b)
    verify(child.parent()._value.entity == b.entity, "set_parent re-parents")
    verify(a.child_count() == 0, "old parent loses the child")

    child.remove_parent()
    verify(!child.has_parent(), "remove_parent detaches the entity")
}

[test]
def test_entity_depth_and_child_count() {
    var w = make_world()
    var root = w.create_entity("d_root")
    var mid = w.create_child(root, "d_mid")
    var leaf = w.create_child(mid, "d_leaf")
    w.create_child(root, "d_sibling")

    verify(root.depth() == 0, "root depth is 0")
    verify(mid.depth() == 1, "mid depth is 1")
    verify(leaf.depth() == 2, "leaf depth is 2")
    verify(root.child_count() == 2, "root has two direct children")
    verify(leaf.child_count() == 0, "leaf has no children")
}

[test]
def test_entity_children_and_children_recursive() {
    var w = make_world()
    var root = w.create_entity("c_root")
    var mid = w.create_child(root, "c_mid")
    w.create_child(mid, "c_leaf")

    var direct = 0
    root.children() $(var c : DnEntity) {
        direct++
    }
    var all = 0
    root.children_recursive() $(var c : DnEntity) {
        all++
    }
    verify(direct == 1, "children visits direct children only")
    verify(all == 2, "children_recursive visits every descendant")
}

[test]
def test_entity_path_and_lookup() {
    var w = make_world()
    var root = w.create_entity("p_root")
    var child = w.create_child(root, "p_child")

    let path = child.path()
    verify(find(path, "p_root") >= 0 && find(path, "p_child") >= 0, "path contains parent and child names")
    verify(root.lookup("p_child").entity == child.entity, "lookup finds a child by name")
    verify(w.lookup(path).entity == child.entity, "world lookup by full path finds the child")
}

[test]
def test_entity_is_valid_after_destruct_pins_current_behaviour() {
    var w = make_world()
    var e = w.create_entity("valid_after_destruct")
    e.destruct()
    verify(!e.is_alive(), "destructed entity is not alive")
    verify(e.is_valid(), "is_valid only checks world and id, so it stays true after destruct")
}

[test]
def test_known_bug_entity_is_valid_false_after_destruct() {
    // KNOWN BUG: DnEntity.is_valid() does not check liveness.
    var w = make_world()
    var e = w.create_entity("known_bug_valid_after_destruct")
    e.destruct()
    verify(!e.is_valid(), "a destructed entity should not be valid")
}

// ---------------------------------------------------------------------------
// Bound struct fields — DnTransform3D
// ---------------------------------------------------------------------------

[test]
def test_dntransform3d_pos_field() {
    var t = DnTransform3D()
    t.pos.x = 1.0f
    verify(t.pos.x == 1.0f, "DnTransform3D.pos.x readable")
}

[test]
def test_dntransform3d_scale_field() {
    var t = DnTransform3D()
    t.scale.y = 2.0f
    verify(t.scale.y == 2.0f, "DnTransform3D.scale.y readable")
}

[test]
def test_dntransform3d_rot_field() {
    var t = DnTransform3D()
    t.rot.w = 1.0f
    verify(t.rot.w == 1.0f, "DnTransform3D.rot.w readable")
}

// ---------------------------------------------------------------------------
// dn_register_component
// ---------------------------------------------------------------------------

[test]
def test_dn_register_component_is_stable() {
    var w = make_world()
    let first = dn_register_component(w, type<DnTransform3D>)
    let second = dn_register_component(w, type<DnTransform3D>)
    verify(first != 0ul, "registered component id is non-zero")
    verify(first == second, "registering twice returns the same id")
}

// ---------------------------------------------------------------------------
// DnQueryBuilder / DnQuery
// ---------------------------------------------------------------------------

[test]
def test_query_builder_build_not_null() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnTransform3D>)
    var q = qb.build()
    verify(q.query != null, "built query has non-null ecs_query_t pointer")
    q.fini()
}

[test]
def test_query_builder_cached_build_not_null() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnTransform3D>)
    qb.cached()
    var q = qb.build()
    verify(q.query != null, "cached query has non-null ecs_query_t pointer")
    q.fini()
}

[test]
def test_query_builder_with_multiple_terms() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnTransform3D>)
    qb.with_term(type<DnVelocity3D>)
    var q = qb.build()
    verify(q.query != null, "query with two terms builds successfully")
    q.fini()
}

[test]
def test_query_builder_without_term() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnTransform3D>)
    qb.without_term(type<DnVelocity3D>)
    var q = qb.build()
    verify(q.query != null, "query with without_term builds successfully")
    q.fini()
}

[test]
def test_query_ecs_query_accessor() {
    var w = make_world()
    var q = w.query_builder().with_term(type<DnTransform3D>).build()
    verify(q.ecs_query() == q.query, "DnQuery.ecs_query() matches .query field")
    q.fini()
}

[test]
def test_query_each_runs_on_matching_entity() {
    var w = make_world()
    var e = w.create_entity("query_target")
    e.set(DnTransform3D(pos = float3(42.0f, 0.0f, 0.0f)))

    var q = w.query_builder().with_term(type<DnTransform3D>).build()
    var found = 0
    var seen = 0ul
    q.each() $(var ent : DnEntity) {
        found++
        seen = ent.entity
    }
    verify(found == 1, "query each visits the matching entity")
    verify(seen == e.entity, "query each passes the matching entity")
    q.fini()
}

[test]
def test_query_each_skips_non_matching_entity() {
    var w = make_world()
    var e = w.create_entity("no_tx_e")
    e.set(DnVelocity3D())

    var q = w.query_builder().with_term(type<DnTransform3D>).build()
    var found = 0
    q.each() $(var ent : DnEntity) {
        found++
    }
    verify(found == 0, "query each visits zero entities when none match")
    q.fini()
}

[test]
def test_query_without_term_excludes_entities() {
    var w = make_world()
    var moving = w.create_entity("moving")
    moving.set(DnTransform3D())
    moving.set(DnVelocity3D())
    var still = w.create_entity("still")
    still.set(DnTransform3D())

    var q = w.query_builder().with_term(type<DnTransform3D>).without_term(type<DnVelocity3D>).build()
    var seen = 0ul
    var found = 0
    q.each() $(var ent : DnEntity) {
        found++
        seen = ent.entity
    }
    verify(found == 1, "without_term excludes the moving entity")
    verify(seen == still.entity, "without_term keeps the still entity")
    q.fini()
}

[test]
def test_query_fini_no_crash() {
    var w = make_world()
    var q = w.query_builder().with_term(type<DnTransform3D>).build()
    q.fini()
}

[test]
def test_world_query_builder_returns_builder() {
    var w = make_world()
    var qb = w.query_builder()
    verify(qb.world == w.world, "query_builder().world matches DnWorld.world")
}

// ---------------------------------------------------------------------------
// Hierarchy traversal (up / cascade)
// ---------------------------------------------------------------------------

// root -> mid -> leaf, each with a DnTransform3D whose pos.x encodes its depth.
def make_hierarchy(var w : DnWorld; var root, mid, leaf : DnEntity&) {
    root = w.create_entity("root")
    root.set(DnTransform3D(pos = float3(0.0f, 0.0f, 0.0f)))

    mid = w.create_entity("mid")
    mid.child_of(root)
    mid.set(DnTransform3D(pos = float3(1.0f, 0.0f, 0.0f)))

    leaf = w.create_entity("leaf")
    leaf.child_of(mid)
    leaf.set(DnTransform3D(pos = float3(2.0f, 0.0f, 0.0f)))
}

[test]
def test_children_array_returns_direct_children() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_hierarchy(w, root, mid, leaf)

    let kids = root.children_array()
    verify(length(kids) == 1, "root has exactly one direct child")
    if (length(kids) == 1) {
        verify(kids[0].entity == mid.entity, "root's child is mid")
    }
}

[test]
def test_with_term_up_reads_parent_component() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_hierarchy(w, root, mid, leaf)

    var q = (w.query_builder()
        .with_term(type<DnTransform3D>, EcsIn)
        .with_term_up(type<DnTransform3D>, EcsUp, EcsIn, EcsAnd, 0ul)
        .build())

    var count = 0
    var it = ecs_query_iter(w.world, q.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            verify(!ecs_field_is_self(addr(it), int8(1)), "the up-traversed field is not self-owned")
            count += it.count
        }
    }
    // mid and leaf have parents; root does not, so it is excluded
    verify(count == 2, "non-optional up term matches only entities that have a parent")
    q.fini()
}

[test]
def test_with_term_cascade_orders_parents_first() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_hierarchy(w, root, mid, leaf)

    var q = (w.query_builder()
        .with_term(type<DnTransform3D>, EcsIn)
        .with_term_up(type<DnTransform3D>, EcsCascade, EcsIn, EcsOptional, 0ul)
        .build())

    var order : array<float>
    var it = ecs_query_iter(w.world, q.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            var t = ecs_field_get(it, type<DnTransform3D>, 0)
            for (i in range(it.count)) {
                order |> push(t[i].pos.x)
            }
        }
    }

    verify(length(order) == 3, "optional cascade matches all three levels incl. the root")
    var monotonic = true
    for (i in range(1, length(order))) {
        if (order[i] < order[i - 1]) { monotonic = false; }
    }
    verify(monotonic, "cascade visits parents before children")
    q.fini()
}

[test]
def test_with_term_cascade_optional_includes_root() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_hierarchy(w, root, mid, leaf)

    var q = (w.query_builder()
        .with_term(type<DnTransform3D>, EcsIn)
        .with_term_up(type<DnTransform3D>, EcsCascade, EcsIn, EcsOptional, 0ul)
        .build())

    var unset_seen = 0
    var it = ecs_query_iter(w.world, q.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            if (!ecs_field_is_set(addr(it), int8(1))) {
                unset_seen += it.count
            }
        }
    }
    verify(unset_seen == 1, "exactly the root is matched with an unset parent field")
    q.fini()
}

[test]
def test_terms_preserves_src_and_trav() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_hierarchy(w, root, mid, leaf)

    // a traversal term routed through terms() must keep its src/trav
    var bulk : array<ecs_term_t>
    bulk |> push(ecs_term(w.world, type<DnTransform3D>, EcsAnd, EcsIn))
    bulk |> push(ecs_term_up(w.world, type<DnTransform3D>, EcsUp, EcsChildOf, EcsAnd, EcsIn))

    var q = w.query_builder().terms(bulk).build()

    var count = 0
    var it = ecs_query_iter(w.world, q.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            verify(!ecs_field_is_self(addr(it), int8(1)), "term src survived terms(): field is traversed")
            count += it.count
        }
    }
    verify(count == 2, "terms() built an up query that excludes the root")
    q.fini()
}
