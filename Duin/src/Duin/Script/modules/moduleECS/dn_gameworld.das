options gen2
options remove_unused_symbols = false
options rtti

require flecs
require dn_log
require dn_ecs
require dn_gameworld

class dn_GameWorld {
    _handle : void?

    def abstract _post_update(delta : double) : void
    def abstract _post_physics_update(delta : double) : void
    def abstract _post_draw() : void
    def abstract _post_draw_ui() : void

    def ecs_world() : ecs_world_t? {
        var ecs_w : ecs_world_t?
        var h = _handle
        if (h != null) {
            unsafe {
                ecs_w = reinterpret<ecs_world_t?>(dn_gameworld_get_flecs_world_impl(h))
            }
        }

        return ecs_w
    }
}

def dn_gameworld_world(var self : dn_GameWorld?) : ecs_world_t? {
    var ecs_w : ecs_world_t?
    var h = self?._handle
    if (h != null) {
        unsafe {
            ecs_w = reinterpret<ecs_world_t?>(dn_gameworld_get_flecs_world_impl(h))
        }
    }

    return ecs_w
}

def dn_gameworld_init(var self : auto?) : auto? {
    self._handle = dn_create_gameworld_impl(self, class_info(*self))
    dn_info("dn_gameworld_init: _handle = {self._handle}\n")
    return self
}

def dn_gameworld_finalize(var self : auto?) : void {
    if (self._handle != null) {
        dn_destroy_gameworld_impl(self._handle)
        self._handle = null
    }
}

def dn_gameworld_create_entity(var self : auto?; name : string) : dn_Entity? {
    if (self._handle == null) {
        return null
    }
    var e = new dn_Entity()
    e.entity = dn_gameworld_create_entity_impl(self._handle, name)
    unsafe {
        e.world = reinterpret<ecs_world_t?>(dn_gameworld_get_flecs_world_impl(self._handle))
    }
    return e
}

def dn_gameworld_find_prefab(var self : auto?; name : string) : uint64 {
    if (self._handle == null) {
        return 0ul
    }
    return dn_gameworld_find_prefab_impl(self._handle, name)
}

def dn_gameworld_set_global_position(var self : auto?; var e : dn_Entity?; x, y, z : float) {
    if (self._handle != null && e != null) {
        dn_gameworld_set_global_position_impl(self._handle, e.entity, x, y, z)
    }
}

def dn_gameworld_get_global_position(var self : auto?; var e : dn_Entity?) : float3 {
    var x = 0.0f
    var y = 0.0f
    var z = 0.0f
    if (self._handle != null && e != null) {
        unsafe {
            dn_gameworld_get_global_position_impl(self._handle, e.entity, addr(x), addr(y), addr(z))
        }
    }
    return float3(x, y, z)
}

def dn_gameworld_set_global_rotation(var self : auto?; var e : dn_Entity?; x, y, z, w : float) {
    if (self._handle != null && e != null) {
        dn_gameworld_set_global_rotation_impl(self._handle, e.entity, x, y, z, w)
    }
}

def dn_gameworld_get_global_rotation(var self : auto?; var e : dn_Entity?) : float4 {
    var x = 0.0f
    var y = 0.0f
    var z = 0.0f
    var w = 1.0f
    if (self._handle != null && e != null) {
        unsafe {
            dn_gameworld_get_global_rotation_impl(self._handle, e.entity, addr(x), addr(y), addr(z), addr(w))
        }
    }
    return float4(x, y, z, w)
}

def dn_gameworld_set_global_scale(var self : auto?; var e : dn_Entity?; x, y, z : float) {
    if (self._handle != null && e != null) {
        dn_gameworld_set_global_scale_impl(self._handle, e.entity, x, y, z)
    }
}

def dn_gameworld_get_global_scale(var self : auto?; var e : dn_Entity?) : float3 {
    var x = 1.0f
    var y = 1.0f
    var z = 1.0f
    if (self._handle != null && e != null) {
        unsafe {
            dn_gameworld_get_global_scale_impl(self._handle, e.entity, addr(x), addr(y), addr(z))
        }
    }
    return float3(x, y, z)
}
