options gen2


module dn_camera public

require dn_camera_core public
require dn_camera_impl public

require dn_log

class dn_Camera {
    _handle : void?

    def dn_Camera() {
        init()
    }

    def dn_Camera(impl : dn_CameraImpl) {
        init()
        set_impl_from_struct(impl)
    }

    def dn_Camera(position, target, global_up : float3, fovy : float) {
        init()
        set_impl_from_params(
            position, 
            target, 
            global_up, 
            fovy)
    }

    def init() : dn_Camera? {
        _handle = dn_camera_init_impl();
        unsafe{ return addr(self); }
    }

    def finalize() : void {
        dn_camera_finalize_impl(_handle);
        _handle = null;
    }

    def set_position(position : float3) : void {
        dn_camera_set_position_impl(_handle, position)
    }

    def get_position() : float3 {
        var pos : float3
        dn_camera_get_position_impl(_handle, pos)
        return pos
    }

    def set_target(target : float3) : void {
        dn_camera_set_target_impl(_handle, target)
    }

    def get_target() : float3 {
        var t : float3
        dn_camera_get_target_impl(_handle, t)
        return t
    }

    def set_global_up(global_up : float3) : void {
        dn_camera_set_global_up_impl(_handle, global_up)
    }

    def get_global_up() : float3 {
        var up_ : float3
        dn_camera_get_global_up_impl(_handle, up_)
        return up_
    }

    def set_fovy(fovy : float) : void {
        dn_camera_set_fovy_impl(_handle, fovy)
    }

    def get_fovy() : float {
        return dn_camera_get_fovy_impl(_handle)
    }

    def get_impl() : dn_CameraImpl {
        var c_impl : dn_CameraImpl = dn_CameraImpl()
        if (_handle != null) {
            dn_get_camera_cameraImpl_impl(_handle, c_impl.position, c_impl.target, c_impl.global_up, c_impl.fovy)
        } else {
            dn_warn("Camera is invalid! Cannot get impl!")
        }
        return c_impl
    }

    def set_impl_from_struct(impl : dn_CameraImpl) : void {
        if (_handle != null) {
            dn_set_camera_cameraImpl_impl(_handle, impl.position, impl.target, impl.global_up, impl.fovy)
        } else {
            dn_warn("Camera is invalid! Cannot set impl!")
        }
    }

    def set_impl_from_params(position, target, global_up : float3, fovy : float) : void {
        if (_handle != null) {
            dn_set_camera_cameraImpl_impl(_handle, position, target, global_up, fovy)
        } else {
            dn_warn("Camera is invalid! Cannot set impl!")
        }
    }

    def move_forward(distance : float) : void {
        dn_camera_move_forward_impl(_handle, distance)
    }

    def move_up(distance : float) : void {
        dn_camera_move_up_impl(_handle, distance)
    }

    def move_right(distance : float) : void {
        dn_camera_move_right_impl(_handle, distance)
    }

    def move_to_target(delta : float) : void {
        dn_camera_move_to_target_impl(_handle, delta)
    }

    def yaw(angle : float; rotateAroundTarget : bool = false) : void {
        dn_camera_yaw_impl(_handle, angle, rotateAroundTarget)
    }

    def pitch(angle : float; lockView : bool = true; rotateAroundTarget : bool = false; rotateUp : bool = false) : void {
        dn_camera_pitch_impl(_handle, angle, lockView, rotateAroundTarget, rotateUp)
    }

    def roll(angle : float) : void {
        dn_camera_roll_impl(_handle, angle)
    }

    def forward() : float3 {
        return dn_camera_forward_impl(_handle)
    }

    def right() : float3 {
        return dn_camera_right_impl(_handle)
    }

    def up() : float3 {
        return dn_camera_up_impl(_handle)
    }

    def is_valid() : bool {
        return dn_camera_is_valid_impl(_handle)
    }
}



def dn_camera_init(var camera : dn_Camera?) : dn_Camera? {
    if (camera != null) {
        camera._handle = dn_camera_init_impl()
    }
    
    return camera
}

def dn_camera_init(camera_impl : dn_CameraImpl) : dn_Camera? {
    var camera : dn_Camera? = new dn_Camera()
    camera._handle = dn_camera_init_impl()
    camera.set_impl_from_struct(camera_impl)
    
    return camera
}

def dn_camera_init(position, target, global_up : float3, fovy : float) : dn_Camera? {
    var camera : dn_Camera? = new dn_Camera()
    camera._handle = dn_camera_init_impl()
    camera.set_impl_from_params(position, target, global_up, fovy)
    
    return camera
}

def dn_camera_finalize(var camera : dn_Camera?) : void {
    if (camera != null) {
        dn_camera_finalize_impl(camera._handle)
    }
}

def dn_get_active_camera() : dn_Camera? {
    var cam : dn_Camera? = new dn_Camera()
    cam._handle = dn_get_active_camera_impl()

    return cam
}

def dn_set_active_camera(var cam : dn_Camera?) : void {
    if (cam != null) {
        dn_set_active_camera_impl(cam._handle)
    } else {
        dn_warn("Null camera passed! Failed to set active camera!")
        dn_set_active_camera_impl(null)
    }
}