options gen2
options rtti

require daslib/rtti

require dn_ecs

struct RenderCube {
    size : float3
}

def register_components(var world : DnWorld?) : void {
    world.dn_register_component(type<RenderCube>)
}