options gen2

module dn_ecs_systems shared

require dn_ecs public
require dn_ecs_components public

struct dn_queries_builtin {
    query_velocity3d_update_transform : dn_Query?
}

var dn_global_queries_builtin : dn_queries_builtin?

def dn_get_global_queries_builtin() : dn_queries_builtin? {
    if (dn_global_queries_builtin == null) {
        dn_global_queries_builtin = new dn_queries_builtin()
        with (dn_global_queries_builtin) {
            query_velocity3d_update_transform = new dn_Query()
        }
    }

    return dn_global_queries_builtin
}

def dn_ecs_build_queries_builtin(
            world : ecs_world_t?, 
            q_builtin : dn_global_queries_builtin? = dn_get_global_queries_builtin()
        ) : void {

    with (q_builtin) {
        query_velocity3d_update_transform = world.query_builder().terms([
                ecs_term(world, type<Transform3D>)
                ecs_term(world, type<Velocity3D>),
            ]).cached().build()
    }
}

def dn_ecs_system_velocity3d_update_transform(delta : double) : void {
    var query : dn_Query? = dn_get_global_queries_builtin().query_velocity3d_update_transform
    var it = ecs_query_iter(world, query.ecs_query())
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

// TODO Render cube