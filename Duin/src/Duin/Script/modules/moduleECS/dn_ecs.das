options gen2
options rtti

require strings
require daslib/class_boost
require daslib/rtti

module dn_ecs shared public

require dn_log public
require flecs public
require dn_ecs_core public

require live/live_vars
require dn_live_host public


class dn_Entity {
    entity : ecs_entity_t
    world : ecs_world_t?

    def dn_Entity(var world : ecs_world_t?, var name : string = "") : void {
        if (world != null) {
            self.world = world
            entity = ecs_entity_new(world, name)
        }
    }

    def dn_Entity(var world : ecs_world_t?, var entity : ecs_entity_t) : void {
        if (world != null) {
            self.world = world
            self.entity = entity
        }
    }

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

    [class_method]
    def static set(var cmp : auto(T)) : void {
        if (world != null) {
            ecs_set(world, self, cmp)
        }
    }

    [class_method]
    def static get_mut(var cmp_type : type<auto(T)>) : T? {
        if (world != null) {
            return ecs_get_mut(world, entity, type<T>)
        }

        return null
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

    def ecs_entity() : ecs_entity_t {
        return entity
    }

    def ecs_world() : ecs_world_t? {
        return world
    }
}



class dn_World {
    world : ecs_world_t?

    def dn_World(var world : ecs_world_t?) : void {
        self.world = world
    }

    def initialize() : bool {
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

    def query_builder() : dn_QueryBuilder? {
        if (world == null) {
            return null
        }
        return new dn_QueryBuilder(world)
    }


    def ecs_world() : ecs_world_t? {
        return world
    }
}

def dn_register_component(var dn_world : dn_World?, var cmp_type : type<auto(T)>) : void {
    ecs_component_register(dn_world.world, type<T>)
}

class dn_Query {
    query : ecs_query_t?
    world : ecs_world_t?

    [class_method]
    def static each(var q : dn_Query?; blk : block<(entity : dn_Entity?)>) : void {
        if (q != null) {
            q.each(blk)
        }
    }

    def fini() : void {
        ecs_query_fini(query)
    }

    def ecs_query() : ecs_query_t? {
        return query
    }

}

class dn_QueryBuilder {
    world : ecs_world_t?
    q_desc : ecs_query_desc_t
    term_count : int

    def dn_QueryBuilder(var world : ecs_world_t?) : void {
        self.world = world
        self.term_count = 0
    }

    // Add a single typed term.
    // Pass EcsIn as inout to match flecs' with<const T>() (read-only).
    [class_method]
    def static with_term(var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : dn_QueryBuilder? {
        let id = ecs_component_register(world, type<T>)
        flecs_query_desc_set_term_full(q_desc, term_count, id, inout, EcsAnd)
        term_count++
        return unsafe(addr(self))
    }

    // Exclude entities that have this component.
    [class_method]
    def static without_term(var cmp : type<auto(T)>) : dn_QueryBuilder? {
        let id = ecs_component_register(world, type<T>)
        flecs_query_desc_set_term_full(q_desc, term_count, id, EcsInOutNone, EcsNot)
        term_count++
        return unsafe(addr(self))
    }

    [class_method]
    def static term(var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : ecs_term_t {
        return ecs_term(world, cmp, inout)
    }

    // Add multiple pre-built terms at once.
    def terms(var bulk_terms : array<ecs_term_t>) : dn_QueryBuilder? {
        for (t in bulk_terms) {
            flecs_query_desc_set_term_full(q_desc, term_count, t.id, int(t.inout), int(t.oper))
            term_count++
        }
        return unsafe(addr(self))
    }

    def build() : dn_Query? {
        var result = new dn_Query()
        result.world = world
        unsafe {
            result.query = ecs_query_init(world, addr(q_desc))
        }
        return result
    }

    def cached() : dn_QueryBuilder? {
        flecs_query_desc_set_cache_kind(q_desc, EcsQueryCacheAuto)
        return unsafe(addr(self))
    }

    def cache_kind(cache_kind : int = 1) : dn_QueryBuilder? {
        flecs_query_desc_set_cache_kind(q_desc, cache_kind)
        return unsafe(addr(self))
    }

    def query_flags(flags : int) : dn_QueryBuilder? {
        return unsafe(addr(self))
    }
}

def ecs_term(var world : dn_World?, var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : ecs_term_t {
    let id : ecs_entity_t = ecs_component_register(world.ecs_world(), type<T>)
    return ecs_term_t(id=id, inout=int16(inout))
}
