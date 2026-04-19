options gen2

module dn_physics_server shared public

require dn_physics_server_core public

require dn_log

class dn_PhysicsServer {
    _handle : void?
    
    def static step(delta : double) : void {
        dn_physics_server_step_impl(delta)
    }

    def static create_plane(normal : float3, height : float) : void {
        dn_physics_server_create_plane_impl(normal, height)
    }

    def static create_box(position : float3, size : float3 = float3(1.0, 1.0, 1.0)) : void {
        dn_physics_server_create_box_impl(position, size)
    }

    def static debug_draw() : void {
        dn_physics_server_debug_draw_impl()
    }
}

def dn_physics_server_step(delta : double) : void {
    dn_physics_server_step_impl(delta)
}

def dn_physics_server_create_plane(normal : float3, height : float) : void {
    dn_physics_server_create_plane_impl(normal, height)
}

def dn_physics_server_create_box(position : float3, size : float3 = float3(1.0, 1.0, 1.0)) : void {
    dn_physics_server_create_box_impl(position, size)
}

def dn_physics_server_debug_draw() : void {
    dn_physics_server_debug_draw_impl()
}