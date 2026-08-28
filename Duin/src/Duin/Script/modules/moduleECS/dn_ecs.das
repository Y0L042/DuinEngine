options gen2
options rtti

module dn_ecs shared public
require strings
require daslib/class_boost
require daslib/rtti
require daslib/option

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
        if (world != null) {
            ecs_add_id(world, entity, ecs_make_pair(EcsIsA, prefab_id))
        }
        return self
    }

    [class_method]
    def static add_tag(tag_id : ecs_entity_t) : DnEntity {
        if (world != null) {
            ecs_add(world, entity, tag_id)
        }
        return self
    }

    // Add with a value: stores the value (flecs add-then-set semantics).
    // Zero-size tag components can't be set in flecs, so they get a plain add.
    [class_method]
    def static add(var tag : auto(T)) : DnEntity {
        if (world != null) {
            static_if (typeinfo sizeof(tag) == 0) {
                ecs_add(world, entity, ecs_component_register(world, type<T>))
            } else {
                ecs_set(world, entity, tag)
            }
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

    // Hook fires before removal so callbacks can still read the live component value.
    // Zero-size tags can't be fetched via get_mut, so they emit a default value.
    [class_method]
    def static remove(var cmp_type : type<auto(T)>) : DnEntity {
        if (world != null) {
            static_if (typeinfo sizeof(cmp_type) == 0) {
                let id = ecs_component_register(world, type<T>)
                if (ecs_has(world, entity, id)) {
                    var empty_val = default<T>
                    ecs_emit_hook_event(type<T>, EcsHookEvent.ONREMOVE, world, entity, empty_val)
                }
                ecs_remove(world, entity, id)
            } else {
                let live = ecs_get_mut(world, entity, type<T>)
                if (live != null) {
                    unsafe {
                        var live_val = *live
                        ecs_emit_hook_event(type<T>, EcsHookEvent.ONREMOVE, world, entity, live_val)
                    }
                }
                ecs_remove(world, entity, ecs_component_register(world, type<T>))
            }
        }
        return self
    }

    [class_method]
    def static get(var t : type<auto(T)>) : T const {
        unsafe {
            let cmp : T? = reinterpret<T?>(ecs_get(world, entity, type<T>))
            let return_cmp = cmp != null ? *cmp : T()
            return return_cmp
        }
    }

    // reinterpret via void? strips the const propagated from self
    [class_method]
    def static get_mut(var cmp_type : type<auto(T)>) : T ? {
        if (world != null) {
            unsafe { return reinterpret<T?>(reinterpret<void?>(ecs_get_mut(world, entity, type<T>))); }
        }
        return null
    }

    [explicit_const_class_method]
    def static get_mut(var cmp_type : type<auto(T)>) : T ? {
        if (world != null) {
            unsafe { return reinterpret<T?>(reinterpret<void?>(ecs_get_mut(world, entity, type<T>))); }
        }
        return null
    }


    [class_method]
    def static has(cmp_type : type<auto(T)>) : bool {
        if (world == null) {
            return false
        }
        return ecs_has(world, entity, ecs_component_register(world, type<T>))
    }

    [class_method]
    def static has_tag(tag_id : ecs_entity_t) : bool {
        if (world == null) {
            return false
        }
        return ecs_has(world, entity, tag_id)
    }

    [class_method]
    def static remove_tag(tag_id : ecs_entity_t) : DnEntity {
        if (world != null) {
            ecs_remove(world, entity, tag_id)
        }
        return self
    }

    // ---- Hierarchy -------------------------------------------------------
    // Flecs has two hierarchy storages, and an entity must use exactly one:
    //   child_of()    -> ChildOf pair storage (fragmenting). Best for large,
    //                    dynamic hierarchies such as scenes.
    //   set_parent()  -> Parent component storage (non-fragmenting, 4.1.6).
    //                    Best for small, deeply nested hierarchies such as
    //                    prefabs; does not put the parent in the entity's type.
    // parent(), children() and path() read from either storage.

    [class_method]
    def static child_of(var parent : DnEntity) : DnEntity {
        if (world != null) {
            ecs_add_id(world, entity, ecs_make_pair(EcsChildOf, parent.entity))
        }
        return self
    }

    // Non-fragmenting Parent storage. Do not combine with child_of() on the
    // same entity.
    [class_method]
    def static set_parent(var parent : DnEntity) : DnEntity {
        if (world != null) {
            ecs_set_parent(world, entity, parent.entity)
        }
        return self
    }

    // Parent of this entity, for either storage.
    // The result is invalid (entity == 0) when this entity has no parent.
    [class_method]
    def static parent() : Option<DnEntity> {
        if (world == null || !has_parent()) {
            return none(type<DnEntity>)
        }
        var p = DnEntity(entity=ecs_get_parent(world, entity), world=world)
        return move_some(p)
    }

    [class_method]
    def static has_parent() : bool {
        if (world == null) {
            return false
        }
        return ecs_get_parent(world, entity) != 0ul
    }

    // Detach from the parent, for either storage.
    [class_method]
    def static remove_parent() : DnEntity {
        if (world != null) {
            if (ecs_has_parent_component(world, entity)) {
                ecs_remove_parent(world, entity)
            } else {
                let p = ecs_get_parent(world, entity)
                if (p != 0ul) {
                    ecs_remove(world, entity, ecs_make_pair(EcsChildOf, p))
                }
            }
        }
        return self
    }

    // Invoke `blk` once per direct child, for either storage.
    [class_method]
    def static children(blk : block<(var child : DnEntity)>) : void {
        if (world == null) {
            return
        }
        var w = world
        ecs_children_each(world, entity) <| $(child : ecs_entity_t) {
            invoke(blk, dn_make_entity(w, child))
        }
    }

    // Invoke `blk` once per descendant, depth-first.
    [class_method]
    def static children_recursive(blk : block<(var child : DnEntity)>) : void {
        if (world == null) {
            return
        }
        var w = world
        ecs_children_each_recursive(world, entity) <| $(child : ecs_entity_t) {
            invoke(blk, dn_make_entity(w, child))
        }
    }

    // Direct children as an array, for either storage. Prefer children() when a
    // callback will do; this materializes the list, which is what you want when
    // the order or the count has to be inspected as a whole.
    [class_method]
    def static children_array() : array<DnEntity> {
        var result : array<DnEntity>
        if (world == null) {
            return <- result
        }
        for (child in ecs_children_array(world, entity)) {
            result |> push(dn_make_entity(world, child))
        }
        return <- result
    }

    [class_method]
    def static child_count() : int {
        if (world == null) {
            return 0
        }
        return ecs_children_count(world, entity)
    }

    // Depth in a Parent-storage hierarchy; 0 for roots and ChildOf children.
    [class_method]
    def static depth() : int {
        if (world == null) {
            return 0
        }
        return ecs_get_parent_depth(world, entity)
    }

    // ---- Paths -----------------------------------------------------------

    // Full "::"-separated path from the root, e.g. "Ship::Cockpit".
    [class_method]
    def static path() : string {
        if (world == null) {
            return ""
        }
        return string(ecs_get_path(world, entity))
    }

    // Look up a descendant by "::"-separated path relative to this entity.
    // The result is invalid (entity == 0) when the path does not resolve.
    [class_method]
    def static lookup(path : string) : DnEntity {
        if (world == null) {
            return DnEntity()
        }
        return dn_make_entity(world, ecs_lookup_from(world, entity, path))
    }

    [class_method]
    def static set_name(name : string) : DnEntity {
        if (world != null) {
            ecs_set_name(world, entity, name)
        }
        return self
    }

    [class_method]
    def static get_name() : string {
        if (world == null) {
            return ""
        }
        return string(ecs_get_name(world, entity))
    }

    [class_method]
    def static enable(v : bool) : DnEntity {
        if (world != null) {
            ecs_enable(world, entity, v)
        }
        return self
    }

    [class_method]
    def static is_alive() : bool {
        if (world == null) {
            return false
        }
        return ecs_is_alive(world, entity)
    }

    [class_method]
    def static destruct() {
        if (world != null) {
            ecs_delete(world, entity)
        }
    }

    [class_method]
    def static ecs_entity() : ecs_entity_t {
        return entity
    }

    [class_method]
    def static ecs_world() : ecs_world_t ? {
        return world
    }

    [class_method]
    def static dn_world() : DnWorld {
        return DnWorld(world)
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

// Explicitly-named factory used by the ecs_query/ecs_observer macros: calling
// `DnEntity(world, id)` from generated code can resolve to the default struct
// constructor instead of the overload below, so macros call this instead.
def dn_make_entity(var world : ecs_world_t?; entity : ecs_entity_t) : DnEntity {
    var e : DnEntity = DnEntity()
    if (world != null) {
        e.world = world
        e.entity = entity
    }
    return e
}

def DnEntity(var dn_world : DnWorld, var entity : ecs_entity_t) : DnEntity {
    var e : DnEntity = DnEntity()
    if (dn_world.world != null) {
        e.world = dn_world.world
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

    // Create a child of `parent` using the non-fragmenting Parent storage.
    // For ChildOf storage instead, use create_entity(...).child_of(parent).
    [class_method]
    def static create_child(var parent : DnEntity, name : string = "") : DnEntity {
        var e : DnEntity = DnEntity()
        if (world != null) {
            e.entity = ecs_new_child(world, parent.entity, name)
            e.world = world
        }
        return e
    }

    // Look up an entity by "::"-separated path from the root.
    // The result is invalid (entity == 0) when the path does not resolve.
    [class_method]
    def static lookup(path : string) : DnEntity {
        if (world == null) {
            return DnEntity()
        }
        return dn_make_entity(world, ecs_lookup_from(world, 0ul, path))
    }

    // Invoke `blk` once per root-level entity.
    [class_method]
    def static root_children(blk : block<(var child : DnEntity)>) : void {
        if (world == null) {
            return
        }
        var w = world
        ecs_children_each(world, 0ul) <| $(child : ecs_entity_t) {
            invoke(blk, dn_make_entity(w, child))
        }
    }

    [class_method]
    def static create_prefab(name : string) : DnEntity {
        var e : DnEntity = DnEntity()
        unsafe {
            var desc = ecs_entity_desc_t(name = name)
            e.entity = ecs_entity_init(world, addr(desc))
        }
        ecs_add(world, e.entity, EcsPrefab)
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

// Accepts const world pointers (e.g. function parameters); strips const on copy.
def DnWorld(world : ecs_world_t?) : DnWorld {
    var w : DnWorld
    unsafe {
        w.world = reinterpret<ecs_world_t?>(world)
    }
    return w
}

// Identity constructor so macros can uniformly wrap either a raw ecs_world_t?
// or a DnWorld via DnWorld(world) without a double-wrap failing to compile.
def DnWorld(var world : DnWorld) : DnWorld {
    return world
}

def dn_register_component(var dn_world : DnWorld, var cmp_type : type<auto(T)>) : ecs_entity_t {
    return ecs_component_register(dn_world.world, type<T>)
}

def set_global_position(w : DnWorld; e : DnEntity; x, y, z : float) {
    dn_world_set_global_position_impl(w.world, e.entity, x, y, z)
}

def get_global_position(w : DnWorld; e : DnEntity) : float3 {
    var x : float = 0.0f
    var y : float = 0.0f
    var z : float = 0.0f
    unsafe {
        dn_world_get_global_position_impl(w.world, e.entity, addr(x), addr(y), addr(z))
    }
    return float3(x, y, z)
}

def set_global_rotation(w : DnWorld; e : DnEntity; x, y, z, rw : float) {
    dn_world_set_global_rotation_impl(w.world, e.entity, x, y, z, rw)
}

def get_global_rotation(w : DnWorld; e : DnEntity) : float4 {
    var x = 0.0f
    var y = 0.0f
    var z = 0.0f
    var rw = 1.0f
    unsafe {
        dn_world_get_global_rotation_impl(w.world, e.entity, addr(x), addr(y), addr(z), addr(rw))
    }
    return float4(x, y, z, rw)
}

def set_global_scale(w : DnWorld; e : DnEntity; x, y, z : float) {
    dn_world_set_global_scale_impl(w.world, e.entity, x, y, z)
}

def get_global_scale(w : DnWorld; e : DnEntity) : float3 {
    var x = 1.0f
    var y = 1.0f
    var z = 1.0f
    unsafe {
        dn_world_get_global_scale_impl(w.world, e.entity, addr(x), addr(y), addr(z))
    }
    return float3(x, y, z)
}

struct DnQuery {
    query : ecs_query_t?
    world : ecs_world_t?

    [class_method]
    def static each(blk : block<(var entity : DnEntity)>) : void {
        if (world == null || query == null) {
            return
        }
        var it = ecs_query_iter(world, query)
        unsafe {
            while (ecs_query_next_iter(it)) {
                for (i in range(it.count)) {
                    invoke(blk, DnEntity(world, it.entities[i]))
                }
            }
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

// Lazily-built query cache used by the ecs_query macro (dn_ecs_boost).
// Keyed by world pointer + call-site key, so the same key is safe across worlds.
var private _dn_query_cache : table<string, DnQuery>

def _dn_ecs_query_get(world : ecs_world_t?, site_key : string, builder_blk : block<() : DnQuery>) : DnQuery {
    let full_key = "{unsafe(reinterpret<uint64>(world))}#{site_key}"
    if (!key_exists(_dn_query_cache, full_key)) {
        _dn_query_cache[full_key] = invoke(builder_blk)
    }
    return _dn_query_cache?[full_key] ?? DnQuery()
}

// Destroys all cached queries for a world.
// raw ecs_init() worlds must call it manually before ecs_fini.
def dn_ecs_queries_fini(world : ecs_world_t?) : void {
    var dead : array<string>
    for (k, v in keys(_dn_query_cache), values(_dn_query_cache)) {
        if (v.world == world) {
            ecs_query_fini(v.query)
            dead |> push(k)
        }
    }
    for (k in dead) {
        _dn_query_cache |> erase(k)
    }
    delete dead
}

// Duin-level systems registry used by the ecs_system macro (no flecs system
// binding exists). Systems run in phase order via dn_ecs_progress.
struct DnSystemEntry {
    name : string
    phase : int
    world : ecs_world_t?
    fn : lambda<(dt : double) : void>
}

var private _dn_systems : array<DnSystemEntry>

// Idempotent by (world, name): re-registering is a no-op, so calling the
// ecs_system macro every frame is safe (though once at startup is preferred).
def dn_ecs_register_system(world : ecs_world_t?, name : string, phase : int, var fn : lambda<(dt : double) : void>) {
    for (s in _dn_systems) {
        if (s.world == world && s.name == name) {
            return
        }
    }
    var entry : DnSystemEntry
    entry.name := name
    entry.phase = phase
    unsafe {
        entry.world = reinterpret<ecs_world_t?>(world)
    }
    entry.fn = fn
    var insert_at = length(_dn_systems)
    for (idx, s in iter_range(_dn_systems), _dn_systems) {
        if (s.phase > phase) {
            insert_at = idx
            break
        }
    }
    _dn_systems |> emplace(entry, insert_at)
}

def dn_ecs_progress(world : ecs_world_t?, dt : double) {
    for (s in _dn_systems) {
        if (s.world == world) {
            invoke(s.fn, dt)
        }
    }
}

// Unregisters all systems for a world.
def dn_ecs_systems_fini(world : ecs_world_t?) {
    var i = length(_dn_systems) - 1
    while (i >= 0) {
        if (_dn_systems[i].world == world) {
            _dn_systems |> erase(i)
        }
        i--
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

    // ---- Relationship traversal -------------------------------------------
    // These read the component from an ANCESTOR rather than from the matched
    // entity. `flags` is an OR of EcsUp / EcsCascade / EcsSelf / EcsDesc, and a
    // `trav` of 0 lets flecs default to EcsChildOf.
    //
    // NOTE: a non-optional traversal term does NOT match root entities, since a
    // root has no ancestor to read from. Pass optional = true (oper =
    // EcsOptional) to keep roots in the result; their field is then unset, and
    // ecs_field_get returns null for it.
    //
    // A traversed field is SHARED across the whole result: ecs_field_get
    // returns a pointer to one value, not a per-entity array. Read it at [0].
    [class_method]
    def static with_term_up(var cmp : type<auto(T)>, flags : uint64, inout : int = EcsInOutDefault, oper : int = EcsAnd, trav : ecs_entity_t = 0ul) : DnQueryBuilder {
        let id = ecs_component_register(world, type<T>)
        flecs_query_desc_set_term_src(q_desc, term_count, id, inout, oper, flags, trav)
        term_count++
        return self
    }

    // Breadth-first parent-ordered traversal: parents are visited before their
    // children. The workhorse for transform propagation down a hierarchy.
    [class_method]
    def static with_term_cascade(var cmp : type<auto(T)>, inout : int = EcsInOutDefault, optional : bool = false) : DnQueryBuilder {
        return with_term_up(cmp, EcsCascade, inout, optional ? EcsOptional : EcsAnd, 0ul)
    }

    // Bottom-up: children are visited before their parents.
    [class_method]
    def static with_term_cascade_desc(var cmp : type<auto(T)>, inout : int = EcsInOutDefault, optional : bool = false) : DnQueryBuilder {
        return with_term_up(cmp, EcsCascade | EcsDesc, inout, optional ? EcsOptional : EcsAnd, 0ul)
    }

    // Use the entity's own component when it has one, otherwise inherit the
    // nearest ancestor's.
    [class_method]
    def static with_term_self_up(var cmp : type<auto(T)>, inout : int = EcsInOutDefault, optional : bool = false) : DnQueryBuilder {
        return with_term_up(cmp, EcsSelf | EcsUp, inout, optional ? EcsOptional : EcsAnd, 0ul)
    }

    [class_method]
    def static term(var cmp : type<auto(T)>, inout : int = EcsInOutDefault) : ecs_term_t {
        return ecs_term(world, cmp, inout)
    }

    // Add multiple pre-built terms at once.
    [class_method]
    def static terms(var bulk_terms : array<ecs_term_t>) : DnQueryBuilder {
        for (t in bulk_terms) {
            // set_term_src so a caller-supplied src/trav (e.g. from ecs_term_up)
            // survives instead of being silently dropped
            flecs_query_desc_set_term_src(q_desc, term_count, t.id, int(t.inout), int(t.oper), t.src.id, t.trav)
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
