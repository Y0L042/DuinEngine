options gen2

module dn_camera_impl public

require dn_camera_core public

struct dn_CameraImpl {
    position : float3
    target : float3
    global_up : float3
    fovy : float
}

def dn_get_active_cameraImpl(var cam : dn_CameraImpl) : bool {
    return dn_get_active_cameraImpl_impl(cam.position, cam.target, cam.global_up, cam.fovy)
}

def dn_camera_forward(cam : dn_CameraImpl) {
    return dn_cameraImpl_forward_impl(cam.position, cam.target, cam.global_up, cam.fovy)
}

def dn_camera_right(cam : dn_CameraImpl) {
    return dn_cameraImpl_right_impl(cam.position, cam.target, cam.global_up, cam.fovy)
}

def dn_camera_up(cam : dn_CameraImpl) {
    return dn_cameraImpl_up_impl(cam.position, cam.target, cam.global_up, cam.fovy)
}

def dn_camera_is_valid(cam : dn_CameraImpl) : bool {
    return dn_cameraImpl_is_valid_impl(cam.position, cam.target, cam.global_up, cam.fovy)
}

def dn_camera_move_forward(var cam : dn_CameraImpl; distance : float) {
    dn_cameraImpl_move_forward_impl(cam.position, cam.target, cam.global_up, cam.fovy, distance)
}

def dn_camera_move_up(var cam : dn_CameraImpl; distance : float) {
    dn_cameraImpl_move_up_impl(cam.position, cam.target, cam.global_up, cam.fovy, distance)
}

def dn_camera_move_right(var cam : dn_CameraImpl; distance : float) {
    dn_cameraImpl_move_right_impl(cam.position, cam.target, cam.global_up, cam.fovy, distance)
}

def dn_camera_move_to_target(var cam : dn_CameraImpl; delta : float) {
    dn_cameraImpl_move_to_target_impl(cam.position, cam.target, cam.global_up, cam.fovy, delta)
}

def dn_camera_yaw(var cam : dn_CameraImpl; angle : float; rotateAroundTarget : bool) {
    dn_cameraImpl_yaw_impl(cam.position, cam.target, cam.global_up, cam.fovy, angle, rotateAroundTarget)
}

def dn_camera_pitch(var cam : dn_CameraImpl; angle : float; lockView : bool; rotateAroundTarget : bool; rotateUp : bool) {
    dn_cameraImpl_pitch_impl(cam.position, cam.target, cam.global_up, cam.fovy, angle, lockView, rotateAroundTarget, rotateUp)
}

def dn_camera_roll(var cam : dn_CameraImpl; angle : float) {
    dn_cameraImpl_roll_impl(cam.position, cam.target, cam.global_up, cam.fovy, angle)
}
