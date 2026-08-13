options gen2
options very_safe_context

require dn_log
require dn_ecs
require dn_ecs_boost
require dn_application


struct Transform3D {
    pos : float3
}

struct Velocity3D {
    val : float3
}

// Before: manual query declaration + construction + iteration
// (see docs/QueryMacro.md)
def system_integrate_velocity_manual(world : ecs_world_t?; delta : double) : void {
    var query_integrate_velocity : DnQuery = (DnWorld(world).query_builder()
        .with_term(type<Transform3D>)
        .with_term(type<Velocity3D>)
        .cached()
        .build())

    var it = ecs_query_iter(world, query_integrate_velocity.ecs_query())
    unsafe {
        while (ecs_query_next_iter(it)) {
            var tx = ecs_field_get(it, type<Transform3D>, 0)
            var v = ecs_field_get(it, type<Velocity3D>, 1)
            for (i in range(it.count)) {
                tx[i].pos += v[i].val * float(delta)
            }
        }
    }
}

// After: same system, expressed with the ecs_query macro
def system_integrate_velocity(world : ecs_world_t?; delta : double) : void {
    ecs_query(world) $ [CACHED] (vel : Velocity3D; var tx : Transform3D) {
        tx.pos += vel.val * float(delta)
    }
}

// Filter annotations combine freely with the traversal ones.
def system_integrate_velocity_filtered(world : ecs_world_t?; delta : double) : void {
    ecs_query(world) $ [
            CACHED
        ] (
            vel : Velocity3D,
            var tx : Transform3D
        ) {
            tx.pos += vel.val * float(delta)
        }
}

// Relationship traversal: `parent_tx` is read from the entity's ancestor rather
// than from the entity itself. CASCADE runs parents before children, which is
// what makes accumulating down a hierarchy correct in a single pass.
//
// The `?` marks the term optional so ROOT entities (which have no ancestor)
// still match, with parent_tx == null. Without it they drop out of the query
// entirely.
def system_propagate_transform(world : ecs_world_t?) : void {
    ecs_query(world) $ [CACHED, CASCADE(parent_tx)] (
            var tx : Transform3D,
            parent_tx : Transform3D?
        ) {
            if (parent_tx != null) {
                tx.pos += parent_tx.pos
            }
            dn_info("{tx.pos.x}, {tx.pos.y}, {tx.pos.z}")
        }
}

[export]
def game_ready() : void {
    var w = DnWorld()
    w.initialize()

    var e = w.create_entity("demo")
    e.set(Transform3D(pos = float3(0.0, 0.0, 0.0)))
    e.set(Velocity3D(val = float3(1.0, 2.0, 3.0)))

    // a child, so the traversal query below has a hierarchy to walk
    var child = w.create_entity("demo_child")
    child.child_of(e)
    child.set(Transform3D(pos = float3(10.0, 10.0, 10.0)))

    var dog = w.create_entity("dog")
    dog.child_of(child)
    dog.set(Transform3D(pos = float3(300.0, 300.0, 300.0)))

    system_integrate_velocity(w.ecs_world(), 1.0d)
    // system_integrate_velocity_manual(w.ecs_world(), 1.0d)
    system_propagate_transform(w.ecs_world())

    dn_set_game_should_quit(true)
}
