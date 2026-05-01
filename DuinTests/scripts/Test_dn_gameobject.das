options gen2
require dastest/testing_boost public

require dn_log
require dn_gameobject

[test]
def test_init(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> success(obj != null, "object was allocated")
    t |> success(obj._handle != null, "handle was initialized")
}

[test]
def test_default_children_count(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.get_children_count(), 0, "new object has no children")
}

[test]
def test_add_child_returns_child(t : T?) : void {
    var parent : dn_GameObject? = new dn_GameObject()
    var child : dn_GameObject? = new dn_GameObject()
    let result = parent |> dn_gameobject_add_child(child)
    t |> success(result != null, "dn_gameobject_add_child returns non-null")
}

[test]
def test_add_child_increments_count(t : T?) : void {
    var parent : dn_GameObject? = new dn_GameObject()
    var child : dn_GameObject? = new dn_GameObject()
    parent |> dn_gameobject_add_child(child)
    t |> equal(parent.get_children_count(), 1, "count is 1 after adding child")
}

[test]
def test_add_child_null_returns_null(t : T?) : void {
    var parent : dn_GameObject? = new dn_GameObject()
    var null_child : dn_GameObject? = null
    let result = parent |> dn_gameobject_add_child(null_child)
    t |> success(result == null, "dn_gameobject_add_child(null) returns null")
}

[test]
def test_remove_child_decrements_count(t : T?) : void {
    var parent : dn_GameObject? = new dn_GameObject()
    var child : dn_GameObject? = new dn_GameObject()
    parent |> dn_gameobject_add_child(child)
    parent |> dn_gameobject_remove_child(child)
    t |> equal(parent.get_children_count(), 0, "count is 0 after removing child")
}

[test]
def test_enable_update(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_update_enabled(), true, "update enabled by default")
    obj.enable_update(false)
    t |> equal(obj.is_update_enabled(), false, "update disabled after enable_update(false)")
    obj.enable_update(true)
    t |> equal(obj.is_update_enabled(), true, "update re-enabled after enable_update(true)")
}

[test]
def test_enable_on_event(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_on_event_enabled(), true, "on_event enabled by default")
    obj.enable_on_event(false)
    t |> equal(obj.is_on_event_enabled(), false, "on_event disabled")
    obj.enable_on_event(true)
    t |> equal(obj.is_on_event_enabled(), true, "on_event re-enabled")
}

[test]
def test_enable_physics_update(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_physics_update_enabled(), true, "physics_update enabled by default")
    obj.enable_physics_update(false)
    t |> equal(obj.is_physics_update_enabled(), false, "physics_update disabled")
    obj.enable_physics_update(true)
    t |> equal(obj.is_physics_update_enabled(), true, "physics_update re-enabled")
}

[test]
def test_enable_draw(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_draw_enabled(), true, "draw enabled by default")
    obj.enable_draw(false)
    t |> equal(obj.is_draw_enabled(), false, "draw disabled")
    obj.enable_draw(true)
    t |> equal(obj.is_draw_enabled(), true, "draw re-enabled")
}

[test]
def test_enable_draw_ui(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_draw_ui_enabled(), true, "draw_ui enabled by default")
    obj.enable_draw_ui(false)
    t |> equal(obj.is_draw_ui_enabled(), false, "draw_ui disabled")
    obj.enable_draw_ui(true)
    t |> equal(obj.is_draw_ui_enabled(), true, "draw_ui re-enabled")
}

[test]
def test_enable_debug(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_debug_enabled(), true, "debug enabled by default")
    obj.enable_debug(false)
    t |> equal(obj.is_debug_enabled(), false, "debug disabled")
    obj.enable_debug(true)
    t |> equal(obj.is_debug_enabled(), true, "debug re-enabled")
}

[test]
def test_enable_children(t : T?) : void {
    var obj : dn_GameObject? = new dn_GameObject()
    t |> equal(obj.is_children_enabled(), true, "children enabled by default")
    obj.enable_children(false)
    t |> equal(obj.is_children_enabled(), false, "children disabled")
    obj.enable_children(true)
    t |> equal(obj.is_children_enabled(), true, "children re-enabled")
}
