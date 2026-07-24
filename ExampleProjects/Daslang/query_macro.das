options gen2

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

[export]
def game_ready() : void {
    var w = DnWorld()
    w.initialize()

    var e = w.create_entity("demo")
    e.set(Transform3D(pos = float3(0.0, 0.0, 0.0)))
    e.set(Velocity3D(val = float3(1.0, 0.0, 0.0)))

    system_integrate_velocity(w.ecs_world(), 1.0d)
    system_integrate_velocity_manual(w.ecs_world(), 1.0d)

    dn_set_game_should_quit(true)
}
