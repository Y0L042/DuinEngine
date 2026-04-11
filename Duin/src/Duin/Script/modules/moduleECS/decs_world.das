options gen2

require decs_entity
require decs_world

class dn_World {
    _handle : void?

    def dn_World() {
        init()
    }

    def init() : void {
        // Initialize C++ world
        self._handle = decs_world_init_impl()
    }

    def is_valid() : bool {
        return (_handle != null)
    }

    def create_entity(name : string) : dn_Entity? {
        var e : dn_Entity? = new dn_Entity()
        e._id = decs_world_create_entity_impl(_handle, name)
        e._world_handle = _handle
        return e
    }

    def create_prefab(name : string) : dn_Entity? {
        var e : dn_Entity? = new dn_Entity()
        e._id = decs_world_create_prefab_impl(_handle, name)
        e._world_handle = _handle
        return e
    }

    def delete_entity(var e : dn_Entity?) {
        if (e != null) {
            decs_world_delete_entity_impl(_handle, e._id)
        }
    }

    def is_alive(var e : dn_Entity?) : bool {
        if (e == null) {
            return false
        }
        return decs_world_is_alive_impl(_handle, e._id)
    }
}
