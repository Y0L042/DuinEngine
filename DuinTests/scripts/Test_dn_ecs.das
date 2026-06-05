options gen2
require dastest/testing_boost public

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
    assert(!w.is_valid(), "DnWorld is not valid before initialize")
}

[test]
def test_world_ecs_world_not_null() {
    var w : DnWorld
    w.initialize()
    assert(w.ecs_world() != null, "DnWorld.ecs_world() is non-null after initialize")
}

[test]
def test_world_create_entity_nonzero() {
    var w = make_world()
    var e = w.create_entity("foo")
    assert(e.entity != 0ul, "create_entity returns non-zero entity id")
}

[test]
def test_world_create_entity_world_pointer() {
    var w = make_world()
    var e = w.create_entity("bar")
    assert(e.world != null, "created entity has non-null world pointer")
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
    assert(p.entity != 0ul, "create_prefab returns non-zero entity id")
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
    assert(e.ecs_entity() == e.entity, "ecs_entity() matches .entity field")
}

[test]
def test_entity_ecs_world_accessor() {
    var w = make_world()
    var e = w.create_entity("world_acc_e")
    assert(e.ecs_world() == e.world, "ecs_world() matches .world field")
}

[test]
def test_entity_ecs_world_not_null() {
    var w = make_world()
    var e = w.create_entity("world_notnull_e")
    assert(e.ecs_world() != null, "ecs_world() is non-null")
}

// ---------------------------------------------------------------------------
// DnEntity — name
// ---------------------------------------------------------------------------

[test]
def test_entity_get_name_matches_create() {
    var w = make_world()
    var e = w.create_entity("named_one")
    let n = e.get_name()
    assert(n == "named_one", "get_name() matches name given to create_entity")
}

[test]
def test_entity_set_name() {
    var w = make_world()
    var e = w.create_entity("original")
    e.set_name("renamed")
    let n = e.get_name()
    assert(n == "renamed", "get_name() reflects name set via set_name()")
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
    child.child_of(unsafe(addr(parent)))
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
// C++ externs — component ID lookups
// ---------------------------------------------------------------------------

[test]
def test_component_id_position3d_nonzero() {
    var w = make_world()
    let id = dn_component_id_position3d(w.world)
    assert(id != 0ul, "position3d component id is non-zero")
}

[test]
def test_component_id_rotation3d_nonzero() {
    var w = make_world()
    let id = dn_component_id_rotation3d(w.world)
    assert(id != 0ul, "rotation3d component id is non-zero")
}

[test]
def test_component_id_scale3d_nonzero() {
    var w = make_world()
    let id = dn_component_id_scale3d(w.world)
    assert(id != 0ul, "scale3d component id is non-zero")
}

[test]
def test_component_id_transform3d_nonzero() {
    var w = make_world()
    let id = dn_component_id_transform3d(w.world)
    assert(id != 0ul, "transform3d component id is non-zero")
}

[test]
def test_component_id_velocity3d_nonzero() {
    var w = make_world()
    let id = dn_component_id_velocity3d(w.world)
    assert(id != 0ul, "velocity3d component id is non-zero")
}

[test]
def test_component_ids_all_distinct() {
    var w = make_world()
    let pos_id   = dn_component_id_position3d(w.world)
    let rot_id   = dn_component_id_rotation3d(w.world)
    let scale_id = dn_component_id_scale3d(w.world)
    let xfm_id   = dn_component_id_transform3d(w.world)
    let vel_id   = dn_component_id_velocity3d(w.world)
    assert(pos_id   != rot_id,   "position id != rotation id")
    assert(pos_id   != scale_id, "position id != scale id")
    assert(pos_id   != xfm_id,   "position id != transform id")
    assert(pos_id   != vel_id,   "position id != velocity id")
    assert(rot_id   != scale_id, "rotation id != scale id")
    assert(scale_id != xfm_id,   "scale id != transform id")
    assert(vel_id   != xfm_id,   "velocity id != transform id")
}

[test]
def test_component_id_null_world_returns_zero() {
    let id = dn_component_id_position3d(null)
    assert(id == 0ul, "null world returns 0 for position3d id")
}

// ---------------------------------------------------------------------------
// C++ externs — tag ID lookups
// ---------------------------------------------------------------------------

[test]
def test_tag_id_local_nonzero() {
    var w = make_world()
    let id = dn_tag_id_local(w.world)
    assert(id != 0ul, "Local tag id is non-zero")
}

[test]
def test_tag_id_global_nonzero() {
    var w = make_world()
    let id = dn_tag_id_global(w.world)
    assert(id != 0ul, "Global tag id is non-zero")
}

[test]
def test_tag_id_pxkinematic_nonzero() {
    var w = make_world()
    let id = dn_tag_id_pxkinematic(w.world)
    assert(id != 0ul, "PxKinematic tag id is non-zero")
}

[test]
def test_tag_id_pxdynamic_nonzero() {
    var w = make_world()
    let id = dn_tag_id_pxdynamic(w.world)
    assert(id != 0ul, "PxDynamic tag id is non-zero")
}

[test]
def test_tag_id_pxstatic_nonzero() {
    var w = make_world()
    let id = dn_tag_id_pxstatic(w.world)
    assert(id != 0ul, "PxStatic tag id is non-zero")
}

[test]
def test_tag_id_nonpx_nonzero() {
    var w = make_world()
    let id = dn_tag_id_nonpx(w.world)
    assert(id != 0ul, "NonPx tag id is non-zero")
}

[test]
def test_tag_id_set_camera_as_active_nonzero() {
    var w = make_world()
    let id = dn_tag_id_set_camera_as_active(w.world)
    assert(id != 0ul, "SetCameraAsActive tag id is non-zero")
}

[test]
def test_tag_id_camera_is_active_nonzero() {
    var w = make_world()
    let id = dn_tag_id_camera_is_active(w.world)
    assert(id != 0ul, "CameraIsActive tag id is non-zero")
}

[test]
def test_tag_id_active_camera_nonzero() {
    var w = make_world()
    let id = dn_tag_id_active_camera(w.world)
    assert(id != 0ul, "ActiveCamera tag id is non-zero")
}

[test]
def test_tag_ids_all_distinct() {
    var w = make_world()
    let local_id  = dn_tag_id_local(w.world)
    let global_id = dn_tag_id_global(w.world)
    let kinem_id  = dn_tag_id_pxkinematic(w.world)
    let dyn_id    = dn_tag_id_pxdynamic(w.world)
    let stat_id   = dn_tag_id_pxstatic(w.world)
    let nonpx_id  = dn_tag_id_nonpx(w.world)
    assert(local_id  != global_id, "Local id != Global id")
    assert(local_id  != kinem_id,  "Local id != PxKinematic id")
    assert(kinem_id  != dyn_id,    "PxKinematic id != PxDynamic id")
    assert(dyn_id    != stat_id,   "PxDynamic id != PxStatic id")
    assert(stat_id   != nonpx_id,  "PxStatic id != NonPx id")
}

[test]
def test_tag_id_null_world_returns_zero() {
    let id = dn_tag_id_local(null)
    assert(id == 0ul, "null world returns 0 for Local tag id")
}

// ---------------------------------------------------------------------------
// C++ externs — Position3D set/get
// ---------------------------------------------------------------------------

[test]
def test_set_get_position3d_roundtrip() {
    var w = make_world()
    var e = w.create_entity("pos_e")
    var pos : DnPosition3D
    pos.value.x = 1.0f
    pos.value.y = 2.0f
    pos.value.z = 3.0f
    dn_entity_set_position3d(w.world, e.entity, pos)
    let got = dn_entity_get_position3d(w.world, e.entity)
    verify(got.value.x == 1.0f, "position x roundtrips")
    verify(got.value.y == 2.0f, "position y roundtrips")
    verify(got.value.z == 3.0f, "position z roundtrips")
}

[test]
def test_get_position3d_default_zero() {
    var w = make_world()
    var e = w.create_entity("pos_default_e")
    let got = dn_entity_get_position3d(w.world, e.entity)
    verify(got.value.x == 0.0f, "default position x is zero")
    verify(got.value.y == 0.0f, "default position y is zero")
    verify(got.value.z == 0.0f, "default position z is zero")
}

[test]
def test_set_position3d_null_world_no_crash() {
    var pos : DnPosition3D
    dn_entity_set_position3d(null, 1ul, pos)
}

[test]
def test_set_position3d_zero_eid_no_crash() {
    var w = make_world()
    var pos : DnPosition3D
    dn_entity_set_position3d(w.world, 0ul, pos)
}

// ---------------------------------------------------------------------------
// C++ externs — Rotation3D set/get
// ---------------------------------------------------------------------------

[test]
def test_set_get_rotation3d_roundtrip() {
    var w = make_world()
    var e = w.create_entity("rot_e")
    var rot : DnRotation3D
    rot.value.x = 0.1f
    rot.value.y = 0.2f
    rot.value.z = 0.3f
    dn_entity_set_rotation3d(w.world, e.entity, rot)
    let got = dn_entity_get_rotation3d(w.world, e.entity)
    verify(got.value.x == 0.1f, "rotation x roundtrips")
    verify(got.value.y == 0.2f, "rotation y roundtrips")
    verify(got.value.z == 0.3f, "rotation z roundtrips")
}

[test]
def test_get_rotation3d_default_zero() {
    var w = make_world()
    var e = w.create_entity("rot_default_e")
    let got = dn_entity_get_rotation3d(w.world, e.entity)
    verify(got.value.x == 0.0f, "default rotation x is zero")
    verify(got.value.y == 0.0f, "default rotation y is zero")
    verify(got.value.z == 0.0f, "default rotation z is zero")
}

[test]
def test_set_rotation3d_null_world_no_crash() {
    var rot : DnRotation3D
    dn_entity_set_rotation3d(null, 1ul, rot)
}

// ---------------------------------------------------------------------------
// C++ externs — Scale3D set/get
// ---------------------------------------------------------------------------

[test]
def test_set_get_scale3d_roundtrip() {
    var w = make_world()
    var e = w.create_entity("scale_e")
    var sc : DnScale3D
    sc.value.x = 2.0f
    sc.value.y = 3.0f
    sc.value.z = 4.0f
    dn_entity_set_scale3d(w.world, e.entity, sc)
    let got = dn_entity_get_scale3d(w.world, e.entity)
    verify(got.value.x == 2.0f, "scale x roundtrips")
    verify(got.value.y == 3.0f, "scale y roundtrips")
    verify(got.value.z == 4.0f, "scale z roundtrips")
}

[test]
def test_get_scale3d_default_zero() {
    var w = make_world()
    var e = w.create_entity("scale_default_e")
    let got = dn_entity_get_scale3d(w.world, e.entity)
    verify(got.value.x == 0.0f, "default scale x is zero")
    verify(got.value.y == 0.0f, "default scale y is zero")
    verify(got.value.z == 0.0f, "default scale z is zero")
}

[test]
def test_set_scale3d_null_world_no_crash() {
    var sc : DnScale3D
    dn_entity_set_scale3d(null, 1ul, sc)
}

// ---------------------------------------------------------------------------
// C++ externs — Transform3D set/get
// ---------------------------------------------------------------------------

[test]
def test_set_get_transform3d_roundtrip() {
    var w = make_world()
    var e = w.create_entity("xfm_e")
    var xfm : DnTransform3D
    xfm.pos.x = 5.0f
    xfm.pos.y = 6.0f
    xfm.pos.z = 7.0f
    xfm.scale.x = 1.0f
    xfm.scale.y = 2.0f
    xfm.scale.z = 3.0f
    xfm.rot.x = 0.0f
    xfm.rot.y = 0.0f
    xfm.rot.z = 0.0f
    xfm.rot.w = 1.0f
    dn_entity_set_transform3d(w.world, e.entity, xfm)
    let got = dn_entity_get_transform3d(w.world, e.entity)
    verify(got.pos.x == 5.0f, "transform pos.x roundtrips")
    verify(got.pos.y == 6.0f, "transform pos.y roundtrips")
    verify(got.pos.z == 7.0f, "transform pos.z roundtrips")
    verify(got.scale.x == 1.0f, "transform scale.x roundtrips")
    verify(got.scale.y == 2.0f, "transform scale.y roundtrips")
    verify(got.scale.z == 3.0f, "transform scale.z roundtrips")
}

[test]
def test_set_transform3d_null_world_no_crash() {
    var xfm : DnTransform3D
    dn_entity_set_transform3d(null, 1ul, xfm)
}

// ---------------------------------------------------------------------------
// C++ externs — Velocity3D set/get
// ---------------------------------------------------------------------------

[test]
def test_set_get_velocity3d_roundtrip() {
    var w = make_world()
    var e = w.create_entity("vel_e")
    var vel : DnVelocity3D
    vel.value.x = 10.0f
    vel.value.y = 20.0f
    vel.value.z = 30.0f
    dn_entity_set_velocity3d(w.world, e.entity, vel)
    let got = dn_entity_get_velocity3d(w.world, e.entity)
    verify(got.value.x == 10.0f, "velocity x roundtrips")
    verify(got.value.y == 20.0f, "velocity y roundtrips")
    verify(got.value.z == 30.0f, "velocity z roundtrips")
}

[test]
def test_get_velocity3d_default_zero() {
    var w = make_world()
    var e = w.create_entity("vel_default_e")
    let got = dn_entity_get_velocity3d(w.world, e.entity)
    verify(got.value.x == 0.0f, "default velocity x is zero")
    verify(got.value.y == 0.0f, "default velocity y is zero")
    verify(got.value.z == 0.0f, "default velocity z is zero")
}

[test]
def test_set_velocity3d_null_world_no_crash() {
    var vel : DnVelocity3D
    dn_entity_set_velocity3d(null, 1ul, vel)
}

// ---------------------------------------------------------------------------
// Bound struct fields — DnVector3
// ---------------------------------------------------------------------------

[test]
def test_dnvector3_fields_readable() {
    var v : DnVector3
    v.x = 1.0f
    v.y = 2.0f
    v.z = 3.0f
    assert(v.x == 1.0f, "DnVector3 x field")
    assert(v.y == 2.0f, "DnVector3 y field")
    assert(v.z == 3.0f, "DnVector3 z field")
}

// ---------------------------------------------------------------------------
// Bound struct fields — DnQuaternion
// ---------------------------------------------------------------------------

[test]
def test_dnquaternion_fields_readable() {
    var q : DnQuaternion
    q.x = 0.0f
    q.y = 0.0f
    q.z = 0.0f
    q.w = 1.0f
    assert(q.x == 0.0f, "DnQuaternion x field")
    assert(q.y == 0.0f, "DnQuaternion y field")
    assert(q.z == 0.0f, "DnQuaternion z field")
    assert(q.w == 1.0f, "DnQuaternion w field")
}

// ---------------------------------------------------------------------------
// Bound struct fields — DnPosition3D / DnRotation3D / DnScale3D / DnVelocity3D
// ---------------------------------------------------------------------------

[test]
def test_dnposition3d_value_field() {
    var p : DnPosition3D
    p.value.x = 7.0f
    assert(p.value.x == 7.0f, "DnPosition3D.value.x readable")
}

[test]
def test_dnrotation3d_value_field() {
    var r : DnRotation3D
    r.value.y = 5.0f
    assert(r.value.y == 5.0f, "DnRotation3D.value.y readable")
}

[test]
def test_dnscale3d_value_field() {
    var s : DnScale3D
    s.value.z = 3.0f
    assert(s.value.z == 3.0f, "DnScale3D.value.z readable")
}

[test]
def test_dnvelocity3d_value_field() {
    var v : DnVelocity3D
    v.value.x = 9.0f
    assert(v.value.x == 9.0f, "DnVelocity3D.value.x readable")
}

// ---------------------------------------------------------------------------
// Bound struct fields — DnTransform3D
// ---------------------------------------------------------------------------

[test]
def test_dntransform3d_pos_field() {
    var t : DnTransform3D
    t.pos.x = 1.0f
    assert(t.pos.x == 1.0f, "DnTransform3D.pos.x readable")
}

[test]
def test_dntransform3d_scale_field() {
    var t : DnTransform3D
    t.scale.y = 2.0f
    assert(t.scale.y == 2.0f, "DnTransform3D.scale.y readable")
}

[test]
def test_dntransform3d_rot_field() {
    var t : DnTransform3D
    t.rot.w = 1.0f
    assert(t.rot.w == 1.0f, "DnTransform3D.rot.w readable")
}

// ---------------------------------------------------------------------------
// dn_register_component
// ---------------------------------------------------------------------------

[test]
def test_dn_register_component_no_crash() {
    var w = make_world()
    dn_register_component(w, type<DnPosition3D>)
}

// ---------------------------------------------------------------------------
// DnQueryBuilder / DnQuery
// ---------------------------------------------------------------------------

[test]
def test_query_builder_build_not_null() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnPosition3D>)
    var q = qb.build()
    assert(q.query != null, "built query has non-null ecs_query_t pointer")
}

[test]
def test_query_builder_cached_build_not_null() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnPosition3D>)
    qb.cached()
    var q = qb.build()
    assert(q.query != null, "cached query has non-null ecs_query_t pointer")
}

[test]
def test_query_builder_with_multiple_terms() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnPosition3D>)
    qb.with_term(type<DnVelocity3D>)
    var q = qb.build()
    assert(q.query != null, "query with two terms builds successfully")
}

[test]
def test_query_builder_without_term() {
    var w = make_world()
    var qb = w.query_builder()
    qb.with_term(type<DnPosition3D>)
    qb.without_term(type<DnVelocity3D>)
    var q = qb.build()
    assert(q.query != null, "query with without_term builds successfully")
}

[test]
def test_query_ecs_query_accessor() {
    var w = make_world()
    var q = w.query_builder().with_term(type<DnPosition3D>).build()
    assert(q.ecs_query() == q.query, "DnQuery.ecs_query() matches .query field")
}

[test]
def test_query_each_runs_on_matching_entity() {
    var w = make_world()
    var e = w.create_entity("query_target")
    var pos : DnPosition3D
    pos.value.x = 42.0f
    dn_entity_set_position3d(w.world, e.entity, pos)

    var q = w.query_builder().with_term(type<DnPosition3D>).build()
    var found = 0
    q.each(q) <| $(ent : DnEntity?) {
        found++
    }
    assert(found >= 1, "query each visits at least one matching entity")
}

[test]
def test_query_each_skips_non_matching_entity() {
    var w = make_world()
    // Create entity without Position3D
    var e = w.create_entity("no_pos_e")
    var vel : DnVelocity3D
    dn_entity_set_velocity3d(w.world, e.entity, vel)

    // Build query requiring Position3D
    var q = w.query_builder().with_term(type<DnPosition3D>).build()
    var found = 0
    q.each(q) <| $(ent : DnEntity?) {
        found++
    }
    // The entity without Position3D should not be counted
    assert(found == 0, "query each visits zero entities when none match")
}

[test]
def test_query_fini_no_crash() {
    var w = make_world()
    var q = w.query_builder().with_term(type<DnPosition3D>).build()
    q.fini()
}

[test]
def test_world_query_builder_returns_builder() {
    var w = make_world()
    var qb = w.query_builder()
    assert(qb.world == w.world, "query_builder().world matches DnWorld.world")
}
