options gen2

require decs_entity

class dn_Entity {
    _id : uint64
    _world_handle : void?

    def is_valid() : bool {
        return (_world_handle != null) && (_id != 0ul)
    }

    def is_a(prefab_id : uint64) : dn_Entity? {
        decs_entity_is_a_impl(_world_handle, _id, prefab_id)
        return unsafe(addr(self))
    }

    def add_tag(tag_id : uint64) : dn_Entity? {
        decs_entity_add_tag_impl(_world_handle, _id, tag_id)
        return unsafe(addr(self))
    }

    def has_tag(tag_id : uint64) : bool {
        return decs_entity_has_tag_impl(_world_handle, _id, tag_id)
    }

    def remove_tag(tag_id : uint64) : dn_Entity? {
        decs_entity_remove_tag_impl(_world_handle, _id, tag_id)
        return unsafe(addr(self))
    }

    def child_of(var parent : dn_Entity?) : dn_Entity? {
        if (parent != null) {
            decs_entity_child_of_impl(_world_handle, _id, parent._id)
        }
        return unsafe(addr(self))
    }

    def set_name(name : string) : dn_Entity? {
        decs_entity_set_name_impl(_world_handle, _id, name)
        return unsafe(addr(self))
    }

    def get_name() : string {
        return decs_entity_get_name_impl(_world_handle, _id)
    }

    def enable(v : bool) : dn_Entity? {
        decs_entity_enable_impl(_world_handle, _id, v)
        return unsafe(addr(self))
    }

    def is_alive() : bool {
        return decs_entity_is_alive_impl(_world_handle, _id)
    }

    def destruct() {
        decs_entity_destruct_impl(_world_handle, _id)
    }
}
