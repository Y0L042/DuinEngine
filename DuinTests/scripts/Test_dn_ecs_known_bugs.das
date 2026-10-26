options gen2
require dastest/testing_boost public

require dn_log
require dn_ecs
require dn_ecs_components

// KNOWN BUG: DnQueryBuilder.with_term_cascade / with_term_cascade_desc / with_term_self_up
// forward their type<T> argument as a value, which the compiler rejects
// ("type expression result is used"). Until dn_ecs.das is fixed this whole file
// fails to compile and the runner reports it as one failure.

def make_world() : DnWorld {
    var w : DnWorld
    w.initialize()
    return w
}

[test]
def test_known_bug_with_term_cascade_builds() {
    var w = make_world()
    var root = w.create_entity("kb_root")
    root.set(DnTransform3D())
    var child = w.create_entity("kb_child")
    child.child_of(root)
    child.set(DnTransform3D())

    var q = (w.query_builder()
        .with_term(type<DnTransform3D>, EcsIn)
        .with_term_cascade(type<DnTransform3D>, EcsIn, true)
        .build())
    verify(q.query != null, "with_term_cascade builds a query")
    q.fini()
}

[test]
def test_known_bug_with_term_cascade_desc_builds() {
    var w = make_world()
    var q = (w.query_builder()
        .with_term(type<DnTransform3D>, EcsIn)
        .with_term_cascade_desc(type<DnTransform3D>, EcsIn, true)
        .build())
    verify(q.query != null, "with_term_cascade_desc builds a query")
    q.fini()
}

[test]
def test_known_bug_with_term_self_up_builds() {
    var w = make_world()
    var q = (w.query_builder()
        .with_term_self_up(type<DnTransform3D>, EcsIn)
        .build())
    verify(q.query != null, "with_term_self_up builds a query")
    q.fini()
}
