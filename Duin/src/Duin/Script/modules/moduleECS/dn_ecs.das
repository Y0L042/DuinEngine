options gen2
options rtti

module dn_ecs shared public
require strings
require daslib/class_boost
require daslib/rtti

require dn_log public
require flecs_boost public
require dn_ecs_core public

struct DnEntity {
    entity : ecs_entity_t = 0ul
    world : ecs_world_t? = null

    [class_method]
    def static is_valid() : bool {
        return (world != null) && (entity != 0ul)
    }

    [class_method]
    def static is_a(prefab_id : ecs_entity_t) : DnEntity {
        let IsA = ecs_lookup(world, "IsA")
        ecs_add_id(world, entity, ecs_make_pair(IsA, prefab_id))
        return self
    }

    [class_method]
    def static add_tag(tag_id : ecs_entity_t) : DnEntity {
        if (world != null) {
            ecs_add(world, entity, tag_id)
        }
        return self
    }

    [class_method]
    def static add(var tag : auto(T)) : DnEntity {
        if (world != null) {
            ecs_add(world, entity, tag)
            ecs_emit_hook_event(type<T>, EcsHookEvent.ONADD, world, entity, tag)
        }
        return self
    }

    [class_method]
    def static set(var cmp : auto(T)) : DnEntity {
        if (world != null) {
            ecs_set(world, entity, cmp)
            ecs_emit_hook_event(type<T>, EcsHookEvent.ONSET, world, entity, cmp)
        }
        return self
    }

    [class_method]
    def static remove(var cmp : auto(T)) : DnEntity {
        if (world != null) {
            ecs_emit_hook_event(type<T>, EcsHookEvent.ONREMOVE, world, entity, cmp)
            ecs_remove(world, entity, ecs_component_register(world, type<T>))
        }
        return self
    }

    [class_method]
    def static get_mut(var cmp_type : type<auto(T)>) : T? {
        if (world != null) {
            unsafe { return reinterpret<T?>(reinterpret<void?>(ecs_get_mut(world, entity, type<T>))); }
        }
        return null
    }

    [class_method]
    def static has_tag(tag_id : ecs_entity_t) : bool {
        return ecs_has(world, entity, tag_id)
    }

    [class_method]
    def static remove_tag(tag_id : ecs_entity_t) : DnEntity {
        ecs_remove(world, entity, tag_id)
        return self
    }

    [class_method]
    def static child_of(var parent : DnEntity?) : DnEntity {
        if (parent != null) {
            let ChildOf = ecs_lookup(world, "ChildOf")
            ecs_add_id(world, entity, ecs_make_pair(ChildOf, parent.entity))
        }
        return self
    }

    [class_method]
    def static set_name(name : string) : DnEntity {
        ecs_set_name(world, entity, name)
        return self
    }

    [class_method]
    def static get_name() : string {
        return string(ecs_get_name(world, entity))
    }

    [class_method]
    def static enable(v : bool) : DnEntity {
        ecs_enable(world, entity, v)
        return self
    }

    [class_method]
    def static is_alive() : bool {
        return ecs_is_alive(world, entity)
    }

    [class_method]
    def static destruct() {
        ecs_delete(world, entity)
    }

    [class_method]
    def static ecs_entity() : ecs_entity_t {
        return entity
    }

    [class_method]
    def static ecs_world() : ecs_world_t ? {
        return world
    }
}

def DnEntity(var world : ecs_world_t?, var name : string = "") : DnEntity {
    var e : DnEntity = DnEntity()
    if (world != null) {
        e.world = world
        e.entity = ecs_entity_new(world, name)
    }
    return e
}

def DnEntity(var world : ecs_world_t?, var entity : ecs_entity_t) : DnEntity {
    var e : DnEntity = DnEntity()
    if (world != null) {
        e.world = world
        e.entity = entity
    }
    return e
}


struct DnWorld {
    world : ecs_world_t?

    [class_method]
    def static initialize() : bool {
        self.world = ecs_init()
        return (self.world != null)
    }

    [class_method]
    def static is_valid() : bool {
        return (world != null)
    }

    [class_method]
    def static create_entity(name : string) : DnEntity {
        var e : DnEntity = DnEntity()
        e.entity = ecs_entity_new(world, name)
        e.world = world
        return e
    }

    [class_method]
    def static create_prefab(name : string) : DnEntity {
        var e : DnEntity = DnEntity()
        unsafe {
            var desc = ecs_entity_desc_t(name = name)
            e.entity = ecs_entity_init(world, addr(desc))
        }
        let Prefab = ecs_lookup(world, "Prefab")
        ecs_add(world, e.entity, Prefab)
        e.world = world
        return e
    }

    [class_method]
    def static delete_entity(var e : DnEntity) {
        ecs_delete(world, e.entity)
    }

    [class_method]
    def static is_alive(var e : DnEntity) : bool {
        return ecs_is_alive(world, e.entity)
    }

    [class_method]
    def static query_builder() : DnQueryBuilder {
        return DnQueryBuilder(world)
    }

    [class_method]
    def static ecs_world() : ecs_world_t ? {
        return world
    }
}

def DnWorld(var world : ecs_world_t?) : DnWorld {
    var w : DnWorld
    w.world = world
    return w
}

def dn_register_component(var dn_world : DnWorld, var cmp_type : type<auto(T)>) : void {
    ecs_component_register(dn_world.world, type<T>)
}

struct DnQuery {
    query : ecs_query_t?
    world : ecs_world_t?

    [class_method]
    def static each(var q : DnQuery ?, blk : block<(entity : DnEntity?)>) : void {
        if (q != null) {
            q.each(blk)
        }
    }

    [class_method]
    def static fini() : void {
        ecs_query_fini(query)
    }

    [class_method]
    def static ecs_query() : ecs_query_t ? {
        return query
    }
}

struct DnQueryBuilder {
    world : ecs_world_t?
    q_desc : ecs_query_desc_t
    term_count : int

    // Add a single typed term.
    // Pass EcsIn as inout to match flecs' with<const T>() (read-only).
    [class_method]
    def static with_term(var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : DnQueryBuilder {
        let id = ecs_component_register(world, type<T>)
        flecs_query_desc_set_term_full(q_desc, term_count, id, inout, EcsAnd)
        term_count++
        return self
    }

    // Exclude entities that have this component.
    [class_method]
    def static without_term(var cmp : type<auto(T)>) : DnQueryBuilder {
        let id = ecs_component_register(world, type<T>)
        flecs_query_desc_set_term_full(q_desc, term_count, id, EcsInOutNone, EcsNot)
        term_count++
        return self
    }

    [class_method]
    def static term(var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : ecs_term_t {
        return ecs_term(world, cmp, inout)
    }

    // Add multiple pre-built terms at once.
    [class_method]
    def static terms(var bulk_terms : array<ecs_term_t>) : DnQueryBuilder {
        for (t in bulk_terms) {
            flecs_query_desc_set_term_full(q_desc, term_count, t.id, int(t.inout), int(t.oper))
            term_count++
        }
        return self
    }

    [class_method]
    def static build() : DnQuery {
        var result : DnQuery
        result.world = world
        unsafe {
            result.query = ecs_query_init(world, addr(q_desc))
        }
        return result
    }

    [class_method]
    def static cached() : DnQueryBuilder {
        flecs_query_desc_set_cache_kind(q_desc, EcsQueryCacheAuto)
        return self
    }

    [class_method]
    def static cache_kind(cache_kind : int = 1) : DnQueryBuilder {
        flecs_query_desc_set_cache_kind(q_desc, cache_kind)
        return self
    }

    [class_method]
    def static query_flags(flags : int) : DnQueryBuilder {
        return self
    }
}

def DnQueryBuilder(var world : ecs_world_t?) : DnQueryBuilder {
    var qb : DnQueryBuilder
    qb.world = world
    qb.term_count = 0
    return qb
}

def ecs_term(var world : DnWorld?, var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : ecs_term_t {
    let id : ecs_entity_t = ecs_component_register(world.ecs_world(), type<T>)
    return ecs_term_t(id = id, inout = int16(inout))
}
