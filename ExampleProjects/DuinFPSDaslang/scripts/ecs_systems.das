options gen2

require dn_ecs
require dn_renderer
require dn_gameworld
require dn_log

require ecs_components

var query_integrate_velocity : ecs_query_t?
var query_render_cube : ecs_query_t?

def construct_queries(world : ecs_world_t?) : void {
    query_integrate_velocity = ecs_query_builder(
        world,
        [
            ecs_term(world, type<dn_Velocity3D>),
            ecs_term(world, type<dn_Transform3D>)
        ],
        EcsQueryCacheAuto
    )

    query_render_cube = ecs_query_builder(
        world,
        [
            ecs_term(world, type<RenderCube>),
            ecs_term(world, type<dn_Transform3D>)
        ],
        EcsQueryCacheAuto
    )
}

def destruct_queries() : void {
    ecs_query_fini(query_integrate_velocity)
    ecs_query_fini(query_render_cube)
}

def system_integrate_velocity(world : ecs_world_t?; delta : double) : void {
    var it = ecs_query_iter(world, query_integrate_velocity)
    unsafe {
        while (ecs_query_next_iter(it)) {
            for (i in range(it.count)) {
                let eid = it.entities[i]
                var tr = dn_entity_get_transform3d(world, eid)
                let vel = dn_entity_get_velocity3d(world, eid)
                tr.pos.x += vel.value.x * float(delta)
                tr.pos.y += vel.value.y * float(delta)
                tr.pos.z += vel.value.z * float(delta)
                dn_entity_set_transform3d(world, eid, tr)
            }
        }
    }
}

def system_draw_rendercube(var game_world : dn_GameWorld?) : void {
    var world = game_world.ecs_world()
    var it = ecs_query_iter(world, query_render_cube)
    unsafe {
        while (ecs_query_next_iter(it)) {
            var cube : RenderCube? = ecs_field_get(it, type<RenderCube>, 0)
            for (i in range(it.count)) {
                dn_info("Rendering cube {i}")
                let eid = it.entities[i]
                var tr = dn_entity_get_transform3d(world, eid)
                var pos = dn_gameworld_get_global_position(game_world, new dn_Entity(world=world, entity=eid))
                dn_draw_box(pos, float4(0.2, 0.8, 0.2, 1.0), float3(1.0, 1.0, 10.0))
            }
        }
    }
}