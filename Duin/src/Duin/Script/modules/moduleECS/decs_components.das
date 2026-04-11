options gen2

require decs_entity
require decs_components

// ---- Component structs ----

struct dn_Position3D {
    x : float
    y : float
    z : float
}

struct dn_Rotation3D {
    x : float
    y : float
    z : float
    w : float
}

struct dn_Scale3D {
    x : float
    y : float
    z : float
}

struct dn_Transform3D {
    px : float
    py : float
    pz : float
    sx : float
    sy : float
    sz : float
    rx : float
    ry : float
    rz : float
    rw : float
}

struct dn_Velocity3D {
    x : float
    y : float
    z : float
}

// ---- Component ID lookups (require a world handle from GameWorld) ----

def decs_component_id_position3d(world_handle : void?) : uint64 {
    return decs_component_id_position3d_impl(world_handle)
}

def decs_component_id_rotation3d(world_handle : void?) : uint64 {
    return decs_component_id_rotation3d_impl(world_handle)
}

def decs_component_id_scale3d(world_handle : void?) : uint64 {
    return decs_component_id_scale3d_impl(world_handle)
}

def decs_component_id_transform3d(world_handle : void?) : uint64 {
    return decs_component_id_transform3d_impl(world_handle)
}

def decs_component_id_velocity3d(world_handle : void?) : uint64 {
    return decs_component_id_velocity3d_impl(world_handle)
}

// ---- Tag ID lookups ----

def decs_tag_id_local(world_handle : void?) : uint64 {
    return decs_tag_id_local_impl(world_handle)
}

def decs_tag_id_global(world_handle : void?) : uint64 {
    return decs_tag_id_global_impl(world_handle)
}

def decs_tag_id_pxkinematic(world_handle : void?) : uint64 {
    return decs_tag_id_pxkinematic_impl(world_handle)
}

def decs_tag_id_pxdynamic(world_handle : void?) : uint64 {
    return decs_tag_id_pxdynamic_impl(world_handle)
}

def decs_tag_id_pxstatic(world_handle : void?) : uint64 {
    return decs_tag_id_pxstatic_impl(world_handle)
}

def decs_tag_id_nonpx(world_handle : void?) : uint64 {
    return decs_tag_id_nonpx_impl(world_handle)
}

def decs_tag_id_set_camera_as_active(world_handle : void?) : uint64 {
    return decs_tag_id_set_camera_as_active_impl(world_handle)
}

def decs_tag_id_camera_is_active(world_handle : void?) : uint64 {
    return decs_tag_id_camera_is_active_impl(world_handle)
}

def decs_tag_id_active_camera(world_handle : void?) : uint64 {
    return decs_tag_id_active_camera_impl(world_handle)
}

// ---- Entity component set/get ----

def decs_entity_set_position3d(var e : dn_Entity?; x, y, z : float) {
    if (e != null) {
        decs_entity_set_position3d_impl(e._world_handle, e._id, x, y, z)
    }
}

def decs_entity_get_position3d(var e : dn_Entity?) : dn_Position3D {
    var r : dn_Position3D
    if (e != null) {
        unsafe {
            decs_entity_get_position3d_impl(e._world_handle, e._id, addr(r.x), addr(r.y), addr(r.z))
        }
    }
    return r
}

def decs_entity_set_rotation3d(var e : dn_Entity?; x, y, z, w : float) {
    if (e != null) {
        decs_entity_set_rotation3d_impl(e._world_handle, e._id, x, y, z, w)
    }
}

def decs_entity_get_rotation3d(var e : dn_Entity?) : dn_Rotation3D {
    var r : dn_Rotation3D
    if (e != null) {
        unsafe {
            decs_entity_get_rotation3d_impl(e._world_handle, e._id, addr(r.x), addr(r.y), addr(r.z), addr(r.w))
        }
    }
    return r
}

def decs_entity_set_scale3d(var e : dn_Entity?; x, y, z : float) {
    if (e != null) {
        decs_entity_set_scale3d_impl(e._world_handle, e._id, x, y, z)
    }
}

def decs_entity_get_scale3d(var e : dn_Entity?) : dn_Scale3D {
    var r : dn_Scale3D
    if (e != null) {
        unsafe {
            decs_entity_get_scale3d_impl(e._world_handle, e._id, addr(r.x), addr(r.y), addr(r.z))
        }
    }
    return r
}

def decs_entity_set_transform3d(var e : dn_Entity?; px, py, pz, sx, sy, sz, rx, ry, rz, rw : float) {
    if (e != null) {
        decs_entity_set_transform3d_impl(e._world_handle, e._id, px, py, pz, sx, sy, sz, rx, ry, rz, rw)
    }
}

def decs_entity_get_transform3d(var e : dn_Entity?) : dn_Transform3D {
    var r : dn_Transform3D
    if (e != null) {
        unsafe {
            decs_entity_get_transform3d_impl(e._world_handle, e._id,
                addr(r.px), addr(r.py), addr(r.pz),
                addr(r.sx), addr(r.sy), addr(r.sz),
                addr(r.rx), addr(r.ry), addr(r.rz), addr(r.rw))
        }
    }
    return r
}

def decs_entity_set_velocity3d(var e : dn_Entity?; x, y, z : float) {
    if (e != null) {
        decs_entity_set_velocity3d_impl(e._world_handle, e._id, x, y, z)
    }
}

def decs_entity_get_velocity3d(var e : dn_Entity?) : dn_Velocity3D {
    var r : dn_Velocity3D
    if (e != null) {
        unsafe {
            decs_entity_get_velocity3d_impl(e._world_handle, e._id, addr(r.x), addr(r.y), addr(r.z))
        }
    }
    return r
}
