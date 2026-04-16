options gen2
options remove_unused_symbols = false
options rtti

require daslib/class_boost public
require daslib/rtti public

module dn_gameworld shared public

require dn_gameworld_class public
require dn_log public
require dn_ecs public
require dn_gameworld_core public

class dn_GameWorld : dn_GameWorld_Adapter {
    initialized : bool = false
    def dn_GameWorld() : void {
        initialize()
    }

    def initialize() : void {
        self._handle = unsafe(dn_create_gameworld_impl(addr(self), class_info(self)))
        initialized = self._handle != null
    }

    def dn_world() : dn_World? {
        return new dn_World(ecs_world())
    }

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

    [class_method]
    def static world() : ecs_world_t? {
        var ecs_w : ecs_world_t?
        var h = _handle
        if (h != null) {
            unsafe {
                ecs_w = reinterpret<ecs_world_t?>(dn_gameworld_get_flecs_world_impl(h))
            }
        }
        return ecs_w
    }

    [class_method]
    def static init() : void {
        _handle = dn_create_gameworld_impl(unsafe(addr(self)), class_info(self))
        dn_info("init: _handle = {_handle}\n")
    }

    [class_method]
    def static fini() : void {
        if (_handle != null) {
            dn_destroy_gameworld_impl(_handle)
            _handle = null
        }
    }

    [class_method]
    def static create_entity(name : string) : dn_Entity? {
        if (_handle == null) {
            return null
        }
        var e = new dn_Entity()
        e.entity = dn_gameworld_create_entity_impl(_handle, name)
        unsafe {
            e.world = reinterpret<ecs_world_t?>(dn_gameworld_get_flecs_world_impl(_handle))
        }
        return e
    }

    [class_method]
    def static find_prefab(name : string) : uint64 {
        if (_handle == null) {
            return 0ul
        }
        return dn_gameworld_find_prefab_impl(_handle, name)
    }

    [class_method]
    def static set_global_position(var e : dn_Entity?; x, y, z : float) {
        if (_handle != null && e != null) {
            dn_gameworld_set_global_position_impl(_handle, e.entity, x, y, z)
        }
    }

    [class_method]
    def static get_global_position(var e : dn_Entity?) : float3 {
        var x = 0.0f
        var y = 0.0f
        var z = 0.0f
        if (_handle != null && e != null) {
            unsafe {
                dn_gameworld_get_global_position_impl(_handle, e.entity, addr(x), addr(y), addr(z))
            }
        }
        return float3(x, y, z)
    }

    [class_method]
    def static set_global_rotation(var e : dn_Entity?; x, y, z, w : float) {
        if (_handle != null && e != null) {
            dn_gameworld_set_global_rotation_impl(_handle, e.entity, x, y, z, w)
        }
    }

    [class_method]
    def static get_global_rotation(var e : dn_Entity?) : float4 {
        var x = 0.0f
        var y = 0.0f
        var z = 0.0f
        var w = 1.0f
        if (_handle != null && e != null) {
            unsafe {
                dn_gameworld_get_global_rotation_impl(_handle, e.entity, addr(x), addr(y), addr(z), addr(w))
            }
        }
        return float4(x, y, z, w)
    }

    [class_method]
    def static set_global_scale(var e : dn_Entity?; x, y, z : float) {
        if (_handle != null && e != null) {
            dn_gameworld_set_global_scale_impl(_handle, e.entity, x, y, z)
        }
    }

    [class_method]
    def static get_global_scale(var e : dn_Entity?) : float3 {
        var x = 1.0f
        var y = 1.0f
        var z = 1.0f
        if (_handle != null && e != null) {
            unsafe {
                dn_gameworld_get_global_scale_impl(_handle, e.entity, addr(x), addr(y), addr(z))
            }
        }
        return float3(x, y, z)
    }
}
