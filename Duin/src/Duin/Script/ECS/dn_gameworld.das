options gen2

require dn_log
require decs_world
require decs_entity
require dn_gameworld

// dn_GameWorld._handle is duin::GameWorld*, which IS-A duin::World*.
// Inherited create_entity / create_prefab pass this handle as duin::World* (safe via upcast in C++).
class dn_GameWorld : dn_World {

    def find_prefab(name : string) : uint64 {
        return decs_gameworld_find_prefab_impl(_handle, name)
    }

    def set_global_position(var e : dn_Entity?; x, y, z : float) {
        if (e != null) {
            decs_gameworld_set_global_position_impl(_handle, e._id, x, y, z)
        }
    }

    def get_global_position(var e : dn_Entity?) : float3 {
        var x = 0.0f
        var y = 0.0f
        var z = 0.0f
        if (e != null) {
            decs_gameworld_get_global_position_impl(_handle, e._id, addr(x), addr(y), addr(z))
        }
        return float3(x, y, z)
    }

    def set_global_rotation(var e : dn_Entity?; x, y, z, w : float) {
        if (e != null) {
            decs_gameworld_set_global_rotation_impl(_handle, e._id, x, y, z, w)
        }
    }

    def get_global_rotation(var e : dn_Entity?) : float4 {
        var x = 0.0f
        var y = 0.0f
        var z = 0.0f
        var w = 1.0f
        if (e != null) {
            decs_gameworld_get_global_rotation_impl(_handle, e._id, addr(x), addr(y), addr(z), addr(w))
        }
        return float4(x, y, z, w)
    }

    def set_global_scale(var e : dn_Entity?; x, y, z : float) {
        if (e != null) {
            decs_gameworld_set_global_scale_impl(_handle, e._id, x, y, z)
        }
    }

    def get_global_scale(var e : dn_Entity?) : float3 {
        var x = 1.0f
        var y = 1.0f
        var z = 1.0f
        if (e != null) {
            decs_gameworld_get_global_scale_impl(_handle, e._id, addr(x), addr(y), addr(z))
        }
        return float3(x, y, z)
    }
}

def dn_get_gameworld() : dn_GameWorld? {
    var w : dn_GameWorld? = new dn_GameWorld()
    w._handle = decs_get_gameworld_impl()
    if (w._handle == null) {
        dn_warn("dn_get_gameworld: no GameWorld in context")
    }
    return w
}
