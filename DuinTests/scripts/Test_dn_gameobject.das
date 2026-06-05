options gen2
require dastest/testing_boost public

require dn_log
require dn_gameobject

[test]
def test_init() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj != null, "object was allocated")
    verify(obj._handle != null, "handle was initialized")
}

[test]
def test_default_children_count() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.get_children_count() == 0, "new object has no children")
}

[test]
def test_add_child_returns_child() {
    var parent : DnGameObject? = new DnGameObject()
    var child : DnGameObject? = new DnGameObject()
    let result = parent |> dn_gameobject_add_child(child)
    verify(result != null, "dn_gameobject_add_child returns non-null")
}

[test]
def test_add_child_increments_count() {
    var parent : DnGameObject? = new DnGameObject()
    var child : DnGameObject? = new DnGameObject()
    parent |> dn_gameobject_add_child(child)
    verify(parent.get_children_count() == 1, "count is 1 after adding child")
}

[test]
def test_add_child_null_returns_null() {
    var parent : DnGameObject? = new DnGameObject()
    var null_child : DnGameObject? = null
    let result = parent |> dn_gameobject_add_child(null_child)
    verify(result == null, "dn_gameobject_add_child(null) returns null")
}

[test]
def test_remove_child_decrements_count() {
    var parent : DnGameObject? = new DnGameObject()
    var child : DnGameObject? = new DnGameObject()
    parent |> dn_gameobject_add_child(child)
    parent |> dn_gameobject_remove_child(child)
    verify(parent.get_children_count() == 0, "count is 0 after removing child")
}

[test]
def test_enable_update() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_update_enabled() == true, "update enabled by default")
    obj.enable_update(false)
    verify(obj.is_update_enabled() == false, "update disabled after enable_update(false)")
    obj.enable_update(true)
    verify(obj.is_update_enabled() == true, "update re-enabled after enable_update(true)")
}

[test]
def test_enable_on_event() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_on_event_enabled() == true, "on_event enabled by default")
    obj.enable_on_event(false)
    verify(obj.is_on_event_enabled() == false, "on_event disabled")
    obj.enable_on_event(true)
    verify(obj.is_on_event_enabled() == true, "on_event re-enabled")
}

[test]
def test_enable_physics_update() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_physics_update_enabled() == true, "physics_update enabled by default")
    obj.enable_physics_update(false)
    verify(obj.is_physics_update_enabled() == false, "physics_update disabled")
    obj.enable_physics_update(true)
    verify(obj.is_physics_update_enabled() == true, "physics_update re-enabled")
}

[test]
def test_enable_draw() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_draw_enabled() == true, "draw enabled by default")
    obj.enable_draw(false)
    verify(obj.is_draw_enabled() == false, "draw disabled")
    obj.enable_draw(true)
    verify(obj.is_draw_enabled() == true, "draw re-enabled")
}

[test]
def test_enable_draw_ui() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_draw_ui_enabled() == true, "draw_ui enabled by default")
    obj.enable_draw_ui(false)
    verify(obj.is_draw_ui_enabled() == false, "draw_ui disabled")
    obj.enable_draw_ui(true)
    verify(obj.is_draw_ui_enabled() == true, "draw_ui re-enabled")
}

[test]
def test_enable_debug() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_debug_enabled() == true, "debug enabled by default")
    obj.enable_debug(false)
    verify(obj.is_debug_enabled() == false, "debug disabled")
    obj.enable_debug(true)
    verify(obj.is_debug_enabled() == true, "debug re-enabled")
}

[test]
def test_enable_children() {
    var obj : DnGameObject? = new DnGameObject()
    verify(obj.is_children_enabled() == true, "children enabled by default")
    obj.enable_children(false)
    verify(obj.is_children_enabled() == false, "children disabled")
    obj.enable_children(true)
    verify(obj.is_children_enabled() == true, "children re-enabled")
}
