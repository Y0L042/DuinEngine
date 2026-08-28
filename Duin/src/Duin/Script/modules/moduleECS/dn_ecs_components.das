options gen2

module dn_ecs_components shared
require math
require daslib/option

require dn_ecs public
require dn_log public

// 2D

struct DnTransform2D {
    pos : float2 = float2(0.0f, 0.0f)
    rot : float2 = float2(0.0f, 0.0f)
    scale : float2 = float2(1.0f, 1.0f)
}

struct DnVelocity2D {
    lin : float2 = float2(0.0f, 0.0f)
    rot : float2 = float2(0.0f, 0.0f)
}




// 3D

struct DnTransform3D {
    pos : float3 = float3(0.0f, 0.0f, 0.0f)
    rot : float4 = float4(0.0f, 0.0f, 0.0f, 1.0f)
    scale : float3 = float3(1.0f, 1.0f, 1.0f)

    globalPositionCache : float3 = float3(0.0f, 0.0f, 0.0f)
    globalRotationCache : float4 = float4(0.0f, 0.0f, 0.0f, 1.0f)
    globalScaleCache : float3 = float3(1.0f, 1.0f, 1.0f)

    gPosCacheDirtyFlag : bool = true
    gRotCacheDirtyFlag : bool = true
    gScaleCacheDirtyFlag : bool = true
}

struct DnVelocity3D {
    lin : float3 = float3(0.0f, 0.0f, 0.0f)
    rot : float4 = float4(0.0f, 0.0f, 0.0f, 1.0f)
}

def quat_identity() : float4 {
    return float4(0.0f, 0.0f, 0.0f, 1.0f)
}

def quat_inverse(q : float4) : float4 {
    let len_sq = q.x * q.x + q.y * q.y + q.z * q.z + q.w * q.w
    if (len_sq == 0.0f) {
        return quat_identity()
    }
    let inv = 1.0f / len_sq
    return float4(-q.x * inv, -q.y * inv, -q.z * inv, q.w * inv)
}

def compose(parent_tx : DnTransform3D; local_tx : DnTransform3D) : DnTransform3D {
    var tx : DnTransform3D
    tx.pos = parent_tx.pos + quat_mul_vec(parent_tx.rot, local_tx.pos * parent_tx.scale)
    tx.rot = quat_mul(parent_tx.rot, local_tx.rot)
    tx.scale = parent_tx.scale * local_tx.scale
    return tx
}

def decompose(parent_tx : DnTransform3D; global_tx : DnTransform3D) : DnTransform3D {
    var tx : DnTransform3D
    let inv_parent_rot = quat_inverse(parent_tx.rot)
    tx.pos = quat_mul_vec(inv_parent_rot, global_tx.pos - parent_tx.pos) / parent_tx.scale
    tx.rot = quat_mul(inv_parent_rot, global_tx.rot)
    tx.scale = global_tx.scale / parent_tx.scale
    return tx
}

def local_transform(e : DnEntity) : Option<DnTransform3D> {
    let raw = e.get_mut(type<DnTransform3D>)
    if (raw == null) {
        return none(type<DnTransform3D>)
    }
    var tx : DnTransform3D
    unsafe {
        tx = *raw
    }
    return move_some(tx)
}

def transform_parent(e : DnEntity) : Option<DnTransform3D> {
    var maybe_parent = e.parent()
    if (!(maybe_parent |> is_some())) {
        return none(type<DnTransform3D>)
    }
    var p = maybe_parent.move_unwrap()
    if (!p.is_valid()) {
        return none(type<DnTransform3D>)
    }
    return get_global_transform(p)
}

def mark_cache_dirty(e : DnEntity) : void {
    let raw = e.get_mut(type<DnTransform3D>)
    if (raw != null) {
        raw.gPosCacheDirtyFlag = true
        raw.gRotCacheDirtyFlag = true
        raw.gScaleCacheDirtyFlag = true
    }
}

def invalidate_children_cache(e : DnEntity) : void {
    e.children_recursive() <| $(var child : DnEntity) {
        mark_cache_dirty(child)
    }
}

def invalidate_global_cache(e : DnEntity) : void {
    mark_cache_dirty(e)
    invalidate_children_cache(e)
}

def get_global_transform(e : DnEntity) : Option<DnTransform3D> {
    if (!e.is_valid()) {
        return none(type<DnTransform3D>)
    }

    var maybe_local = local_transform(e)
    if (!(maybe_local |> is_some())) {
        return none(type<DnTransform3D>)
    }
    let tx = maybe_local.move_unwrap()

    var parent_g_tx = transform_parent(e)
    var global_tx : DnTransform3D
    if (parent_g_tx |> is_some()) {
        global_tx = compose(parent_g_tx.move_unwrap(), tx)
    } else {
        global_tx = tx
    }

    let raw = e.get_mut(type<DnTransform3D>)
    if (raw != null) {
        raw.globalPositionCache = global_tx.pos
        raw.globalRotationCache = global_tx.rot
        raw.globalScaleCache = global_tx.scale
        raw.gPosCacheDirtyFlag = false
        raw.gRotCacheDirtyFlag = false
        raw.gScaleCacheDirtyFlag = false
    }

    return move_some(global_tx)
}

def set_global_transform(e : DnEntity; global_tx : DnTransform3D) : void {
    if (!e.is_valid()) {
        dn_warn("Entity not valid, or does not have Transform3D!")
        return
    }
    if (!e.has(type<DnTransform3D>)) {
        dn_warn("Entity not valid, or does not have Transform3D!")
        return
    }

    var parent_g_tx = transform_parent(e)
    var local_tx : DnTransform3D
    if (parent_g_tx |> is_some()) {
        local_tx = decompose(parent_g_tx.move_unwrap(), global_tx)
    } else {
        local_tx = global_tx
    }

    let raw = e.get_mut(type<DnTransform3D>)
    if (raw == null) {
        return
    }

    raw.pos = local_tx.pos
    raw.rot = local_tx.rot
    raw.scale = local_tx.scale
    raw.globalPositionCache = global_tx.pos
    raw.globalRotationCache = global_tx.rot
    raw.globalScaleCache = global_tx.scale
    raw.gPosCacheDirtyFlag = false
    raw.gRotCacheDirtyFlag = false
    raw.gScaleCacheDirtyFlag = false

    invalidate_children_cache(e)
}

def get_global_position(e : DnEntity) : Option<float3> {
    let raw = e.get_mut(type<DnTransform3D>)
    if (raw == null) {
        return none(type<float3>)
    }
    if (!raw.gPosCacheDirtyFlag) {
        var cached = raw.globalPositionCache
        return move_some(cached)
    }
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        return none(type<float3>)
    }
    var result = g.move_unwrap().pos
    return move_some(result)
}

def set_global_position(e : DnEntity; pos : float3) : void {
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        dn_warn("Entity not valid, or does not have Transform3D!")
        return
    }
    var tx = g.move_unwrap()
    tx.pos = pos
    set_global_transform(e, tx)
}

def get_global_rotation(e : DnEntity) : Option<float4> {
    let raw = e.get_mut(type<DnTransform3D>)
    if (raw == null) {
        return none(type<float4>)
    }
    if (!raw.gRotCacheDirtyFlag) {
        var cached = raw.globalRotationCache
        return move_some(cached)
    }
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        return none(type<float4>)
    }
    var result = g.move_unwrap().rot
    return move_some(result)
}

def set_global_rotation(e : DnEntity; rot : float4) : void {
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        dn_warn("Entity not valid, or does not have Transform3D!")
        return
    }
    var tx = g.move_unwrap()
    tx.rot = rot
    set_global_transform(e, tx)
}

def get_global_scale(e : DnEntity) : Option<float3> {
    let raw = e.get_mut(type<DnTransform3D>)
    if (raw == null) {
        return none(type<float3>)
    }
    if (!raw.gScaleCacheDirtyFlag) {
        var cached = raw.globalScaleCache
        return move_some(cached)
    }
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        return none(type<float3>)
    }
    var result = g.move_unwrap().scale
    return move_some(result)
}

def set_global_scale(e : DnEntity; scale : float3) : void {
    var g = get_global_transform(e)
    if (!(g |> is_some())) {
        dn_warn("Entity not valid, or does not have Transform3D!")
        return
    }
    var tx = g.move_unwrap()
    tx.scale = scale
    set_global_transform(e, tx)
}

// Marker components
struct DnCubeComponent {}
struct DnStaticBodyComponent {}
struct DnKinematicBodyComponent {}
struct DnDynamicBodyComponent {}
struct DnCharacterBodyComponent {}
struct DnPhysicsStaticCubeComponent {}
struct DnDebugCapsuleComponent {}
struct DnDebugCubeComponent {}

// struct DnCamera {}

def dn_register_internal_components(var world : DnWorld&) : void {
    world.dn_register_component(type<DnTransform2D>)
    world.dn_register_component(type<DnVelocity2D>)

    world.dn_register_component(type<DnTransform3D>)
    world.dn_register_component(type<DnVelocity3D>)

    world.dn_register_component(type<DnCubeComponent>)
    world.dn_register_component(type<DnStaticBodyComponent>)
    world.dn_register_component(type<DnKinematicBodyComponent>)
    world.dn_register_component(type<DnDynamicBodyComponent>)
    world.dn_register_component(type<DnCharacterBodyComponent>)
    world.dn_register_component(type<DnPhysicsStaticCubeComponent>)
    world.dn_register_component(type<DnDebugCapsuleComponent>)
    world.dn_register_component(type<DnDebugCubeComponent>)
}
