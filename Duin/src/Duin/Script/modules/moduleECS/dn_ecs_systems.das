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
        query_velocity3d_update_transform = ecs_query_builder(
            world,
            [
                ecs_term(world, type<dn_Transform3D>)
                ecs_term(world, type<dn_Velocity3D>),
            ],
            EcsQueryCacheAuto
        )
    }
}