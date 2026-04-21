options gen2

require daslib/class_boost
require math

module dn_camera public

require dn_camera_core public
require dn_camera_impl public

require dn_log

class dn_Camera {
    _handle : void?

    def dn_Camera() {
        self.init()
    }

    def dn_Camera(impl : dn_CameraImpl) {
        self.init()
        self.set_impl(impl)
    }

    def dn_Camera(position, target, global_up : float3, fovy : float) {
        self.init()
        self.set_impl(
            position, 
            target, 
            global_up, 
            fovy
        )
    }

    [class_method]
    def static init() : dn_Camera? {
        _handle = dn_camera_init_impl();
        unsafe{ return addr(self); }
    }

    [class_method]
    def static init(camera_impl : dn_CameraImpl) : dn_Camera? {
        var camera : dn_Camera? = new dn_Camera()
        camera._handle = dn_camera_init_impl()
        camera.set_impl(camera_impl)
        
        return camera
    }

    [class_method]
    def static init(position, target, global_up : float3, fovy : float) : dn_Camera? {
        var camera : dn_Camera? = new dn_Camera()
        camera._handle = dn_camera_init_impl()
        camera.set_impl(position, target, global_up, fovy)
        
        return camera
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

    [class_method]
    def static set_impl(impl : dn_CameraImpl) : void {
        if (_handle != null) {
            dn_set_camera_cameraImpl_impl(_handle, impl.position, impl.target, impl.global_up, impl.fovy)
        } else {
            dn_warn("Camera is invalid! Cannot set impl!")
        }
    }

    [class_method]
    def static set_impl(position, target, global_up : float3, fovy : float) : void {
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

    def move_to(position : float3) : void {
        let delta =  position - get_position()
        set_position(position)
        set_target(get_target() + delta)
    }

    // -Y (floor) is -PI/2 , +Y (sky) is PI/2, Horizon is 0
    def get_pitch() : float {
        var dir_vec = get_target() - get_position()
        var Y_UP = float3(0.0, 1.0, 0.0)
        
        let dot = dot(dir_vec, Y_UP)
        let inv_mag = inv_length(dir_vec) * 1.0

        return ((PI / 2.0) - acos(dot / inv_mag))
    }
}





def dn_camera_init(camera_impl : dn_CameraImpl) : dn_Camera? {
    var camera : dn_Camera? = new dn_Camera()
    camera._handle = dn_camera_init_impl()
    camera.set_impl(camera_impl)
    
    return camera
}

def dn_camera_init(position, target, global_up : float3, fovy : float) : dn_Camera? {
    var camera : dn_Camera? = new dn_Camera()
    camera._handle = dn_camera_init_impl()
    camera.set_impl(position, target, global_up, fovy)
    
    return camera
}

def dn_camera_finalize(var self : dn_Camera?) : void {
    if (self != null) {
        dn_camera_finalize_impl(self._handle)
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

def dn_camera_init(var self : dn_Camera?) : dn_Camera? {
    if (self != null) {
        self._handle = dn_camera_init_impl()
    }

    return self
}

def dn_camera_set_position(var self : dn_Camera?; position : float3) : void {
    if (self != null) {
        self.set_position(position)
    }
}

def dn_camera_get_position(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.get_position()
    }
    return default<float3>
}

def dn_camera_set_target(var self : dn_Camera?; target : float3) : void {
    if (self != null) {
        self.set_target(target)
    }
}

def dn_camera_get_target(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.get_target()
    }
    return default<float3>
}

def dn_camera_set_global_up(var self : dn_Camera?; global_up : float3) : void {
    if (self != null) {
        self.set_global_up(global_up)
    }
}

def dn_camera_get_global_up(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.get_global_up()
    }
    return default<float3>
}

def dn_camera_set_fovy(var self : dn_Camera?; fovy : float) : void {
    if (self != null) {
        self.set_fovy(fovy)
    }
}

def dn_camera_get_fovy(var self : dn_Camera?) : float {
    if (self != null) {
        return self.get_fovy()
    }
    return default<float>
}

def dn_camera_get_impl(var self : dn_Camera?) : dn_CameraImpl {
    if (self != null) {
        return self.get_impl()
    }
    return default<dn_CameraImpl>
}

def dn_camera_set_impl(var self : dn_Camera?; impl : dn_CameraImpl) : void {
    if (self != null) {
        self.set_impl(impl)
    }
}

def dn_camera_set_impl(var self : dn_Camera?; position, target, global_up : float3; fovy : float) : void {
    if (self != null) {
        self.set_impl(position, target, global_up, fovy)
    }
}

def dn_camera_move_forward(var self : dn_Camera?; distance : float) : void {
    if (self != null) {
        self.move_forward(distance)
    }
}

def dn_camera_move_up(var self : dn_Camera?; distance : float) : void {
    if (self != null) {
        self.move_up(distance)
    }
}

def dn_camera_move_right(var self : dn_Camera?; distance : float) : void {
    if (self != null) {
        self.move_right(distance)
    }
}

def dn_camera_move_to_target(var self : dn_Camera?; delta : float) : void {
    if (self != null) {
        self.move_to_target(delta)
    }
}

def dn_camera_yaw(var self : dn_Camera?; angle : float; rotateAroundTarget : bool = false) : void {
    if (self != null) {
        self.yaw(angle, rotateAroundTarget)
    }
}

def dn_camera_pitch(var self : dn_Camera?; angle : float; lockView : bool = true; rotateAroundTarget : bool = false; rotateUp : bool = false) : void {
    if (self != null) {
        self.pitch(angle, lockView, rotateAroundTarget, rotateUp)
    }
}

def dn_camera_roll(var self : dn_Camera?; angle : float) : void {
    if (self != null) {
        self.roll(angle)
    }
}

def dn_camera_forward(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.forward()
    }
    return default<float3>
}

def dn_camera_right(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.right()
    }
    return default<float3>
}

def dn_camera_up(var self : dn_Camera?) : float3 {
    if (self != null) {
        return self.up()
    }
    return default<float3>
}

def dn_camera_is_valid(var self : dn_Camera?) : bool {
    if (self != null) {
        return self.is_valid()
    }
    return false
}