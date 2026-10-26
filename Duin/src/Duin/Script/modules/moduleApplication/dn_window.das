options gen2
options remove_unused_symbols = false

require daslib/class_boost

module dn_window shared public

require dn_application_core public
require dn_camera

class DnWindow {
    _handle : void?

    def DnWindow() {
        _handle = null
    }

    def DnWindow(title : string; width, height : int; flags : uint64 = DN_WINDOW_RESIZABLE) {
        _handle = dn_window_create_impl(title, width, height, flags)
    }

    def finalize() : void {
        dn_window_release_impl(_handle)
        _handle = null
    }

    def close() : bool {
        return dn_window_close_impl(_handle)
    }

    def is_valid() : bool {
        return dn_window_is_valid_impl(_handle)
    }

    def is_minimized() : bool {
        return dn_window_is_minimized_impl(_handle)
    }

    def can_draw() : bool {
        return dn_window_is_valid_impl(_handle) && !dn_window_is_minimized_impl(_handle)
    }

    def size() : int2 {
        if (_handle == null) {
            return int2(0, 0)
        }
        return int2(dn_window_get_width_impl(_handle), dn_window_get_height_impl(_handle))
    }

    def set_size(width, height : int) : void {
        dn_window_set_size_impl(_handle, width, height)
    }

    def title() : string {
        if (_handle == null) {
            return ""
        }
        return dn_window_get_title_impl(_handle)
    }

    def set_title(title : string) : void {
        dn_window_set_title_impl(_handle, title)
    }

    def flags() : uint64 {
        if (_handle == null) {
            return 0ul
        }
        return dn_window_get_flags_impl(_handle)
    }

    def set_flags(flags : uint64) : void {
        dn_window_set_flags_impl(_handle, flags)
    }

    def clear_color() : uint {
        if (_handle == null) {
            return 0u
        }
        return dn_window_get_clear_color_impl(_handle)
    }

    def set_clear_color(rgba : uint) : void {
        dn_window_set_clear_color_impl(_handle, rgba)
    }

    def set_camera(camera : DnCamera?) : void {
        dn_window_set_camera_impl(_handle, camera != null ? camera._handle : null)
    }

    def queue_draw(var fn : lambda<() : void>) : void {
        dn_window_queue_draw_impl(_handle, fn)
    }

    def queue_draw_ui(var fn : lambda<() : void>) : void {
        dn_window_queue_draw_ui_impl(_handle, fn)
    }
}
