options gen2
require dastest/testing_boost public

require dn_log
require dn_ecs
require dn_ecs_boost
require dn_ecs_components

// ---------------------------------------------------------------------------
// Helpers
// ---------------------------------------------------------------------------

struct BTag {}

struct BPos {
    x : float
}

struct BDepth {
    d : int
}

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

// root -> mid -> leaf, each carrying its own depth and an x to accumulate into.
def make_chain(var w : DnWorld; var root, mid, leaf : DnEntity&) {
    root = w.create_entity("root")
    root.set(BPos(x = 0.0f))
    root.set(BDepth(d = 0))

    mid = w.create_entity("mid")
    mid.child_of(root)
    mid.set(BPos(x = 0.0f))
    mid.set(BDepth(d = 1))

    leaf = w.create_entity("leaf")
    leaf.child_of(mid)
    leaf.set(BPos(x = 0.0f))
    leaf.set(BDepth(d = 2))
}

// ---------------------------------------------------------------------------
// Baseline: plain queries must be unaffected by the traversal changes
// ---------------------------------------------------------------------------

[test]
def test_plain_query_still_works() {
    var w = make_world()
    var a = w.create_entity("a")
    a.set(BPos(x = 1.0f))
    var b = w.create_entity("b")
    b.set(BPos(x = 2.0f))

    var total = 0.0f
    ecs_query(w.ecs_world()) $ [CACHED] (p : BPos) {
        total += p.x
    }
    verify(total == 3.0f, "plain ecs_query visits every matching entity")
}

[test]
def test_plain_query_mutation_still_works() {
    var w = make_world()
    var a = w.create_entity("a")
    a.set(BPos(x = 1.0f))

    ecs_query(w.ecs_world()) $ (var p : BPos) {
        p.x += 10.0f
    }

    var seen = 0.0f
    ecs_query(w.ecs_world()) $ (p : BPos) {
        seen = p.x
    }
    verify(seen == 11.0f, "mutations through a plain query are written back")
}

// ---------------------------------------------------------------------------
// CASCADE
// ---------------------------------------------------------------------------

[test]
def test_cascade_reads_parent_component() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    // Accumulate down the hierarchy: each entity's x becomes parent.x + 1.
    // This is only correct if parents are processed BEFORE their children.
    ecs_query(w.ecs_world()) $ [CASCADE(parent)] (var p : BPos; parent : BPos?) {
        if (parent != null) {
            p.x = (*parent).x + 1.0f
        }
    }

    verify(root.get(type<BPos>).x == 0.0f, "root keeps x = 0 (no parent)")
    verify(mid.get(type<BPos>).x == 1.0f, "mid reads root and becomes 1")
    verify(leaf.get(type<BPos>).x == 2.0f, "leaf reads mid and becomes 2")
}

[test]
def test_cascade_optional_root_is_null() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    var null_count = 0
    var visited = 0
    ecs_query(w.ecs_world()) $ [CASCADE(parent)] (p : BPos; parent : BPos?) {
        visited++
        if (parent == null) {
            null_count++
        }
    }

    verify(visited == 3, "optional cascade matches all three levels, roots included")
    verify(null_count == 1, "exactly the root sees a null parent")
}

[test]
def test_cascade_required_skips_roots() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    // non-pointer traversal arg -> required term -> roots do not match
    var visited = 0
    ecs_query(w.ecs_world()) $ [CASCADE(parent)] (p : BPos; parent : BPos) {
        visited++
    }

    verify(visited == 2, "a required cascade term excludes root entities")
}

// The regression that a single-child test cannot catch. Several children share
// one parent and land in the same result; if the traversed field were indexed
// per-entity ([_dn_i]) instead of at [0], child 0 would look right and every
// later child would read past the end of a one-element array.
[test]
def test_cascade_multi_child_same_parent() {
    var w = make_world()
    var parent = w.create_entity("p")
    parent.set(BPos(x = 100.0f))

    var kids : array<DnEntity>
    for (i in range(4)) {
        var c = w.create_entity("c{i}")
        c.child_of(parent)
        c.set(BPos(x = 0.0f))
        kids |> push(c)
    }

    ecs_query(w.ecs_world()) $ [CASCADE(par)] (var p : BPos; par : BPos?) {
        if (par != null) {
            p.x = (*par).x
        }
    }

    var all_ok = true
    for (k in kids) {
        if (k.get(type<BPos>).x != 100.0f) {
            all_ok = false
        }
    }
    verify(all_ok, "every child of the same parent reads the parent's value")
}

[test]
def test_cascade_desc_visits_children_first() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    var order : array<int>
    ecs_query(w.ecs_world()) $ [CASCADE_DESC(parent)] (d : BDepth; parent : BDepth?) {
        order |> push(d.d)
    }

    verify(length(order) == 3, "cascade_desc matches all three levels")
    var non_increasing = true
    for (i in range(1, length(order))) {
        if (order[i] > order[i - 1]) {
            non_increasing = false
        }
    }
    verify(non_increasing, "cascade_desc visits children before parents")
}

// ---------------------------------------------------------------------------
// UP / SELF_UP
// ---------------------------------------------------------------------------

[test]
def test_up_reads_nearest_ancestor() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    // give each level a distinct value so "nearest" is observable
    root.set(BPos(x = 10.0f))
    mid.set(BPos(x = 20.0f))

    var leaf_saw = -1.0f
    ecs_query(w.ecs_world()) $ [UP(parent)] (d : BDepth; parent : BPos?) {
        if (d.d == 2 && parent != null) {
            leaf_saw = (*parent).x
        }
    }

    verify(leaf_saw == 20.0f, "UP reads the nearest ancestor (mid), not the root")
}

[test]
def test_self_up_prefers_own_component() {
    var w = make_world()
    var parent = w.create_entity("p")
    parent.set(BPos(x = 100.0f))
    parent.set(BDepth(d = 0))

    // has its own BPos
    var owner = w.create_entity("owner")
    owner.child_of(parent)
    owner.set(BDepth(d = 1))
    owner.set(BPos(x = 7.0f))

    // has no BPos of its own, so it must inherit the parent's
    var inheritor = w.create_entity("inheritor")
    inheritor.child_of(parent)
    inheritor.set(BDepth(d = 2))

    var owner_saw = -1.0f
    var inheritor_saw = -1.0f
    ecs_query(w.ecs_world()) $ [SELF_UP(val)] (d : BDepth; val : BPos?) {
        if (val != null) {
            if (d.d == 1) { owner_saw = (*val).x; }
            if (d.d == 2) { inheritor_saw = (*val).x; }
        }
    }

    verify(owner_saw == 7.0f, "SELF_UP uses the entity's own component when present")
    verify(inheritor_saw == 100.0f, "SELF_UP falls back to the ancestor's component")
}

// ---------------------------------------------------------------------------
// Shared implement() path: ecs_system must behave identically
// ---------------------------------------------------------------------------

[test]
def test_cascade_inside_ecs_system() {
    var w = make_world()
    var root = DnEntity()
    var mid = DnEntity()
    var leaf = DnEntity()
    make_chain(w, root, mid, leaf)

    ecs_system(w.ecs_world(), "cascade_prop", 0) $ [CASCADE(parent)] (var p : BPos; parent : BPos?) {
        if (parent != null) {
            p.x = (*parent).x + 1.0f
        }
    }
    dn_ecs_progress(w.ecs_world(), 0.016lf)

    verify(mid.get(type<BPos>).x == 1.0f, "cascade works through ecs_system too")
    verify(leaf.get(type<BPos>).x == 2.0f, "ecs_system cascade still orders parents first")
}

// ---------------------------------------------------------------------------
// ecs_find_query
// ---------------------------------------------------------------------------

[test]
def test_find_query_returns_true_on_first_match() {
    var w = make_world()
    var a = w.create_entity("find_a")
    a.set(BPos(x = 10.0f))
    var b = w.create_entity("find_b")
    b.set(BPos(x = 50.0f))

    let found = ecs_find_query(w.ecs_world()) $(p : BPos) {
        return p.x > 40.0f
    }
    verify(found, "ecs_find_query finds the entity above the threshold")
}

[test]
def test_find_query_returns_false_without_match() {
    var w = make_world()
    var a = w.create_entity("nomatch_a")
    a.set(BPos(x = 1.0f))

    let found = ecs_find_query(w.ecs_world()) $(p : BPos) {
        return p.x > 1000.0f
    }
    verify(!found, "ecs_find_query reports false when nothing matches")
}

// ---------------------------------------------------------------------------
// REQUIRE / REQUIRE_NOT
// ---------------------------------------------------------------------------

[test]
def test_require_filters_by_tag() {
    var w = make_world()
    var tagged = w.create_entity("tagged")
    tagged.set(BPos(x = 1.0f))
    tagged.add(BTag())
    var plain = w.create_entity("plain")
    plain.set(BPos(x = 2.0f))

    var total = 0.0f
    var count = 0
    ecs_query(w.ecs_world()) $ [REQUIRE(BTag)] (p : BPos) {
        total += p.x
        count++
    }
    verify(count == 1, "REQUIRE matches only the tagged entity")
    verify(total == 1.0f, "REQUIRE binds the tagged entity's component")
}

[test]
def test_require_not_excludes_tag() {
    var w = make_world()
    var tagged = w.create_entity("tagged")
    tagged.set(BPos(x = 1.0f))
    tagged.add(BTag())
    var plain = w.create_entity("plain")
    plain.set(BPos(x = 2.0f))

    var total = 0.0f
    var count = 0
    ecs_query(w.ecs_world()) $ [REQUIRE_NOT(BTag)] (p : BPos) {
        total += p.x
        count++
    }
    verify(count == 1, "REQUIRE_NOT skips the tagged entity")
    verify(total == 2.0f, "REQUIRE_NOT binds the untagged entity's component")
}

[test]
def test_query_binds_entity_argument() {
    var w = make_world()
    var e = w.create_entity("bound_entity")
    e.set(BPos(x = 3.0f))

    var seen = 0ul
    ecs_query(w.ecs_world()) $(ent : DnEntity; p : BPos) {
        seen = ent.entity
    }
    verify(seen == e.entity, "DnEntity argument binds the matched entity")
}

// ---------------------------------------------------------------------------
// ecs_observer
// ---------------------------------------------------------------------------

struct OPos {
    x : float
}

var g_observer_onset_count = 0
var g_observer_onset_last = 0.0f
var g_observer_onremove_count = 0

[test]
def test_observer_fires_on_set_and_remove() {
    var w = make_world()
    ecs_observer(EcsHookEvent.ONSET) $(ent : DnEntity; v : OPos) {
        g_observer_onset_count++
        g_observer_onset_last = v.x
    }
    ecs_observer(EcsHookEvent.ONREMOVE) $(v : OPos) {
        g_observer_onremove_count++
    }

    let setBefore = g_observer_onset_count
    let removeBefore = g_observer_onremove_count

    var e = w.create_entity("observed")
    e.set(OPos(x = 7.0f))
    verify(g_observer_onset_count == setBefore + 1, "ONSET observer fires on DnEntity.set")
    verify(g_observer_onset_last == 7.0f, "ONSET observer receives the new value")

    e.remove(type<OPos>)
    verify(g_observer_onremove_count == removeBefore + 1, "ONREMOVE observer fires on DnEntity.remove")
}

// ---------------------------------------------------------------------------
// Registry cleanup
// ---------------------------------------------------------------------------

var g_fini_system_runs = 0

[test]
def test_systems_fini_unregisters_and_is_idempotent() {
    var w = make_world()
    var e = w.create_entity("fini_target")
    e.set(BPos(x = 0.0f))

    ecs_system(w.ecs_world(), "fini_counter", 0) $(p : BPos) {
        g_fini_system_runs++
    }
    dn_ecs_progress(w.ecs_world(), 0.016lf)
    let afterFirst = g_fini_system_runs
    verify(afterFirst >= 1, "registered system runs on progress")

    dn_ecs_systems_fini(w.ecs_world())
    dn_ecs_systems_fini(w.ecs_world())
    dn_ecs_progress(w.ecs_world(), 0.016lf)
    verify(g_fini_system_runs == afterFirst, "system no longer runs after dn_ecs_systems_fini")
}

[test]
def test_queries_fini_is_idempotent_and_queries_rebuild() {
    var w = make_world()
    var e = w.create_entity("rebuild_target")
    e.set(BPos(x = 4.0f))

    var total = 0.0f
    ecs_query(w.ecs_world(), "rebuild_key") $(p : BPos) {
        total += p.x
    }
    dn_ecs_queries_fini(w.ecs_world())
    dn_ecs_queries_fini(w.ecs_world())

    ecs_query(w.ecs_world(), "rebuild_key") $(p : BPos) {
        total += p.x
    }
    verify(total == 8.0f, "query works before and after dn_ecs_queries_fini")
}

[test]
def test_system_registration_is_idempotent_by_name() {
    var w = make_world()
    var e = w.create_entity("idem_target")
    e.set(BPos(x = 0.0f))

    for (i in range(3)) {
        ecs_system(w.ecs_world(), "idem_system", 0) $(var p : BPos) {
            p.x += 1.0f
        }
    }
    dn_ecs_progress(w.ecs_world(), 0.016lf)
    verify(e.get(type<BPos>).x == 1.0f, "re-registering a system by name does not duplicate it")
    dn_ecs_systems_fini(w.ecs_world())
}
