options gen2

require dn_ecs

struct RenderCube {
    size : float3
}

def register_components(world : ecs_world_t?) : void {
    ecs_component_register(world, type<RenderCube>)
}