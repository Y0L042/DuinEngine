options gen2

require flecs
require dn_log

class dn_Entity {
    entity : ecs_entity_t
    world : ecs_world_t?

    def is_valid() : bool {
        return (world != null) && (entity != 0ul)
    }

    def is_a(prefab_id : ecs_entity_t) : dn_Entity? {
        let IsA = ecs_lookup(world, "IsA")
        ecs_add_id(world, entity, ecs_make_pair(IsA, prefab_id))
        return unsafe(addr(self))
    }

    def add_tag(tag_id : ecs_entity_t) : dn_Entity? {
        ecs_add(world, entity, tag_id)
        return unsafe(addr(self))
    }

    def has_tag(tag_id : ecs_entity_t) : bool {
        return ecs_has(world, entity, tag_id)
    }

    def remove_tag(tag_id : ecs_entity_t) : dn_Entity? {
        ecs_remove(world, entity, tag_id)
        return unsafe(addr(self))
    }

    def child_of(var parent : dn_Entity?) : dn_Entity? {
        if (parent != null) {
            let ChildOf = ecs_lookup(world, "ChildOf")
            ecs_add_id(world, entity, ecs_make_pair(ChildOf, parent.entity))
        }
        return unsafe(addr(self))
    }

    def set_name(name : string) : dn_Entity? {
        ecs_set_name(world, entity, name)
        return unsafe(addr(self))
    }

    def get_name() : string {
        return string(ecs_get_name(world, entity))
    }

    def enable(v : bool) : dn_Entity? {
        ecs_enable(world, entity, v)
        return unsafe(addr(self))
    }

    def is_alive() : bool {
        return ecs_is_alive(world, entity)
    }

    def destruct() {
        ecs_delete(world, entity)
    }
}



class dn_World {
    world : ecs_world_t?

    def init() : bool {
        self.world = ecs_init();
        return (self.world != null);
    }

    def is_valid() : bool {
        return (world != null)
    }

    def create_entity(name : string) : dn_Entity? {
        var e = new dn_Entity()
        e.entity = ecs_entity_new(world, name)
        e.world = world
        return e
    }

    def create_prefab(name : string) : dn_Entity? {
        var e = new dn_Entity()
        unsafe {
            var desc = ecs_entity_desc_t(name = name)
            e.entity = ecs_entity_init(world, addr(desc))
        }
        let Prefab = ecs_lookup(world, "Prefab")
        ecs_add(world, e.entity, Prefab)
        e.world = world
        return e
    }

    def delete_entity(var e : dn_Entity?) {
        if (e != null) {
            ecs_delete(world, e.entity)
        }
    }

    def is_alive(var e : dn_Entity?) : bool {
        if (e == null) {
            return false
        }
        return ecs_is_alive(world, e.entity)
    }
}



typedef dn_Term = ecs_entity_t

struct dn_QueryDesc {
    terms : array<dn_Term>
}

class dn_Query {
    query : ecs_query_t?
    world : ecs_world_t?
/*
    def each(blk : block<(entity : dn_Entity?)>) {
        if (query == null || world == null) {
            return
        }
        var it = ecs_query_iter(world, query)
        while (ecs_query_next_iter(it)) {
            for (i in range(it.count)) {
                var e = new dn_Entity()
                e.entity = it.entities[i]
                e.world = world
                invoke(blk, e)
            }
        }
    }
*/
}

def dn_query_each(var q : dn_Query?; blk : block<(entity : dn_Entity?)>) {
    if (q != null) {
        q.each(blk)
    }
}

def dn_query_builder(var world_obj : dn_World?; var query_desc : dn_QueryDesc) : dn_Query? {
    if (world_obj == null) {
        return null
    }
    var terms : array<ecs_term_t>
    for (t in query_desc.terms) {
        push(terms, ecs_term(t))
    }
    var q = new dn_Query()
    q.world = world_obj.world
    q.query = ecs_query_builder(world_obj.world, terms, EcsQueryCacheAuto)
    return q
}