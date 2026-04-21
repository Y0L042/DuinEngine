options gen2
options remove_unused_symbols = false
options rtti

require daslib/class_boost public
require rtti_core public
require daslib/rtti public

module dn_gameobject shared public

require dn_log public
require dn_gameobject_class public
require dn_gameobject_core public

class dn_GameObject : dn_GameObject_Adapter {
    def dn_GameObject() : void {
        unsafe {
            _handle = dn_create_gameobject_impl(addr(self), class_info(self))
            dn_info("dn_gameobject_init: _handle = {_handle}\n")
        }
    }

    def finalize() : void {
        if (_handle != null) {
            dn_destroy_gameobject_impl(_handle)
            _handle = null
        }
    }

    [class_method]
    def static add_child(var child : auto?) : auto? {
        if ((_handle != null) && (child != null)) {
            if (child._handle == null) {
                dn_gameobject_init(child)
            }
            dn_add_child_object_impl(_handle, child._handle)

            return child
        }
        return null
    }

    [class_method]
    def static add_root_child(var child : auto?) : auto? {
        if (child != null) {
            if (child._handle == null) {
                dn_gameobject_init(child)
            }
            dn_add_root_object_impl(child._handle)
            return child
        }
        return null
    }

    [class_method]
    def static remove_child(var child : auto?) : void {
        if ((_handle != null) && (child != null) && (child._handle != null)) {
            dn_remove_child_object_impl(_handle, child._handle)
        }
    }

    def get_children_count() : int {
        if (_handle != null) {
            return dn_get_children_count_impl(_handle)
        }
        return 0
    }

    def enable(enable : bool) : void {
        if (_handle != null) {
            dn_enable_impl(_handle, enable)
        }
    }

    def enable_on_event(enable : bool) : void {
        if (_handle != null) {
            dn_enable_on_event_impl(_handle, enable)
        }
    }

    def enable_update(enable : bool) : void {
        if (_handle != null) {
            dn_enable_update_impl(_handle, enable)
        }
    }

    def enable_physics_update(enable : bool) : void {
        if (_handle != null) {
            dn_enable_physics_update_impl(_handle, enable)
        }
    }

    def enable_draw(enable : bool) : void {
        if (_handle != null) {
            dn_enable_draw_impl(_handle, enable)
        }
    }

    def enable_draw_ui(enable : bool) : void {
        if (_handle != null) {
            dn_enable_draw_ui_impl(_handle, enable)
        }
    }

    def enable_debug(enable : bool) : void {
        if (_handle != null) {
            dn_enable_debug_impl(_handle, enable)
        }
    }

    def enable_children(enable : bool) : void {
        if (_handle != null) {
            dn_enable_children_impl(_handle, enable)
        }
    }

    def is_on_event_enabled() : bool {
        if (_handle != null) {
            return dn_is_on_event_enabled_impl(_handle)
        }
        return false
    }

    def is_update_enabled() : bool {
        if (_handle != null) {
            return dn_is_update_enabled_impl(_handle)
        }
        return false
    }

    def is_physics_update_enabled() : bool {
        if (_handle != null) {
            return dn_is_physics_update_enabled_impl(_handle)
        }
        return false
    }

    def is_draw_enabled() : bool {
        if (_handle != null) {
            return dn_is_draw_enabled_impl(_handle)
        }
        return false
    }

    def is_draw_ui_enabled() : bool {
        if (_handle != null) {
            return dn_is_draw_ui_enabled_impl(_handle)
        }
        return false
    }

    def is_debug_enabled() : bool {
        if (_handle != null) {
            return dn_is_debug_enabled_impl(_handle)
        }
        return false
    }

    def is_children_enabled() : bool {
        if (_handle != null) {
            return dn_is_children_enabled_impl(_handle)
        }
        return false
    }
}






def dn_gameobject_init(var self : auto?) : auto? {
    self._handle = dn_create_gameobject_impl(self, class_info(*self))
    dn_info("dn_gameobject_init: _handle = {self._handle}\n")

    return self
}

def dn_gameobject_finalize(var self : auto?) : void {
    if (self._handle != null) {
        dn_destroy_gameobject_impl(self._handle)
        self._handle = null
    }
}

def dn_gameobject_add_child(var self : auto?; var child : auto?) : auto? {
    if ((self._handle != null) && (child != null)) {
        if (child._handle == null) {
            dn_gameobject_init(child)
        }
        dn_add_child_object_impl(self._handle, child._handle)
        return child
    }
    return null
}

def dn_gameobject_add_root_child(var child : auto?) : auto? {
    if (child != null) {
        if (child._handle == null) {
            dn_gameobject_init(child)
        }
        dn_add_root_object_impl(child._handle)
        return child
    }
    return null
}

def dn_gameobject_remove_child(var self : auto?; var child : auto?) : void {
    if ((self._handle != null) && (child != null) && (child._handle != null)) {
        dn_remove_child_object_impl(self._handle, child._handle)
    }
}

def dn_get_children_count(var self : auto?) : int {
    if (self._handle != null) {
        return dn_get_children_count_impl(self._handle)
    }
    return 0
}

def dn_enable(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_impl(self._handle, enable)
    }
}

def dn_enable_on_event(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_on_event_impl(self._handle, enable)
    }
}

def dn_enable_update(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_update_impl(self._handle, enable)
    }
}

def dn_enable_physics_update(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_physics_update_impl(self._handle, enable)
    }
}

def dn_enable_draw(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_draw_impl(self._handle, enable)
    }
}

def dn_enable_draw_ui(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_draw_ui_impl(self._handle, enable)
    }
}

def dn_enable_debug(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_debug_impl(self._handle, enable)
    }
}

def dn_enable_children(var self : auto?; enable : bool) : void {
    if (self._handle != null) {
        dn_enable_children_impl(self._handle, enable)
    }
}

def dn_is_on_event_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_on_event_enabled_impl(self._handle)
    }
    return false
}

def dn_is_update_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_update_enabled_impl(self._handle)
    }
    return false
}

def dn_is_physics_update_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_physics_update_enabled_impl(self._handle)
    }
    return false
}

def dn_is_draw_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_draw_enabled_impl(self._handle)
    }
    return false
}

def dn_is_draw_ui_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_draw_ui_enabled_impl(self._handle)
    }
    return false
}

def dn_is_debug_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_debug_enabled_impl(self._handle)
    }
    return false
}

def dn_is_children_enabled(var self : auto?) : bool {
    if (self._handle != null) {
        return dn_is_children_enabled_impl(self._handle)
    }
    return false
}