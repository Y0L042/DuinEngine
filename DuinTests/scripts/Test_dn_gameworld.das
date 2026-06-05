options gen2
require dastest/testing_boost public

require dn_log
require dn_gameworld

[test]
def unsafe test_gameworld_handle_not_null() {
    var gw = new DnGameWorld()
    verify(gw._handle != null, "DnGameWorld handle is non-null after construction")
    delete gw
}

[test]
def unsafe test_gameworld_create_entity_nonzero() {
    var gw = new DnGameWorld()
    let e = gw.create_entity("test_entity")
    verify(e.entity != 0ul, "create_entity returns non-zero entity")
    delete gw
}

[test]
def unsafe test_gameworld_set_get_position() {
    var gw = new DnGameWorld()
    let e = gw.create_entity("pos_entity")
    gw.set_global_position(e, 1.0f, 2.0f, 3.0f)
    let pos = gw.get_global_position(e)
    verify(pos.x == 1.0f, "position x matches")
    verify(pos.y == 2.0f, "position y matches")
    verify(pos.z == 3.0f, "position z matches")
    delete gw
}

[test]
def unsafe test_gameworld_set_get_scale() {
    var gw = new DnGameWorld()
    let e = gw.create_entity("scale_entity")
    gw.set_global_scale(e, 2.0f, 3.0f, 4.0f)
    let sc = gw.get_global_scale(e)
    verify(sc.x == 2.0f, "scale x matches")
    verify(sc.y == 3.0f, "scale y matches")
    verify(sc.z == 4.0f, "scale z matches")
    delete gw
}

[test]
def unsafe test_gameworld_set_get_rotation() {
    var gw = new DnGameWorld()
    let e = gw.create_entity("rot_entity")
    gw.set_global_rotation(e, 0.0f, 0.0f, 0.0f, 1.0f)
    let rot = gw.get_global_rotation(e)
    verify(rot.x == 0.0f, "rotation x matches")
    verify(rot.y == 0.0f, "rotation y matches")
    verify(rot.z == 0.0f, "rotation z matches")
    verify(rot.w == 1.0f, "rotation w matches")
    delete gw
}

[test]
def unsafe test_gameworld_fini_clears_handle() {
    var gw = new DnGameWorld()
    verify(gw._handle != null, "handle non-null before fini")
    gw.fini()
    verify(gw._handle == null, "handle null after fini")
    delete gw
}
