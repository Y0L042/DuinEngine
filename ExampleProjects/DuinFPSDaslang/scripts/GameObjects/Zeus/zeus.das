options gen2
options rtti

require daslib/rtti
require daslib/class_boost
require math

require dn_gameobject
require dn_camera
require dn_log
require dn_input
require dn_ecs

class Zeus : dn_GameObject {
    move_speed : float = 7.0f
    mouse_sensitivity : float = 0.0015f
    dev_camera : dn_Camera?

    def override _init() {
        dev_camera = dn_camera_init(new dn_Camera())
        dev_camera.set_impl(
            float3(15.0, 8.0, 15.0),
            float3(0.0, 0.0, 0.0),
            float3(0.0, 1.0, 0.0),
            80.0
        )
    }

    def override _ready() {
        dn_info("Zeus ready!");
    }

    def override _update(delta : double) {}

    def override _physics_update(delta : double) {
        process_input(delta)
    }

    def override _draw() {}

    def override _draw_ui() {}

    def override _debug() {}

    def set_camera_active() : void {
        dn_set_active_camera(dev_camera)
    }

    def process_input(delta : double) : void {
        if (dev_camera == null) {
            return
        }
        let dt = float(delta)

        if (dn_is_input_action_triggered("Zeus_MoveCameraFast")) {
            move_speed = 7.0f * 2.2f
        } else {
            move_speed = 7.0f 
        }

        if (dn_is_input_action_triggered("Zeus_ResetCameraPos")) {
            dev_camera.set_position(float3(5.0f, 5.0f, 5.0f))
        }

        // Mouse look
        let md = dn_get_mouse_delta()
        dev_camera.yaw(md.x * mouse_sensitivity)
        dev_camera.pitch(-md.y * mouse_sensitivity)

        // Horizontal movement relative to dev_camera yaw (WASD), vertical with Q/Z
        let fwd = dev_camera.forward()
        let rgt = dev_camera.right()

        var move = float3(0.0f, 0.0f, 0.0f)
        let input_vec = dn_get_input_vector(
            DN_Scancode.DN_SCANCODE_W,
            DN_Scancode.DN_SCANCODE_S,
            DN_Scancode.DN_SCANCODE_A,
            DN_Scancode.DN_SCANCODE_D
        )
        move.x = fwd.x * (-input_vec.y) + rgt.x * input_vec.x
        move.z = fwd.z * (-input_vec.y) + rgt.z * input_vec.x

        if (dn_is_input_action_triggered("Zeus_MoveCameraUp")) {
            move.y += 1.0f
        }
        if (dn_is_input_action_triggered("Zeus_MoveCameraDown")) {
            move.y -= 1.0f
        }

        let move_len = length(move)
        if (move_len > 0.001f) {
            let step = (move / move_len) * move_speed * dt
            let pos = dev_camera.get_position()
            let tgt = dev_camera.get_target()
            dev_camera.set_position(pos + step)
            dev_camera.set_target(tgt + step)
        }
    }



}

