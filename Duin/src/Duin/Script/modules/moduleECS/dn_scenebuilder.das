options gen2

module dn_scenebuilder shared public

require dn_scenebuilder_core public
require dn_gameworld public

class DnPackedScene {
    _handle : void?

    def finalize() : void {
        if (_handle != null) {
            dn_packedscene_destroy(_handle)
            _handle = null
        }
    }
}

class DnSceneBuilder {
    _handle : void?

    def DnSceneBuilder() : void {
        unsafe {
            _handle = dn_scenebuilder_create()
        }
    }

    def finalize() : void {
        if (_handle != null) {
            dn_scenebuilder_destroy(_handle)
            _handle = null
        }
    }

    def pack_world(var world : DnGameWorld?) : DnPackedScene? {
        if (_handle == null) {
            return null
        }
        var ps = new DnPackedScene()
        unsafe {
            ps._handle = dn_scenebuilder_pack_world(_handle, world._handle)
        }
        return <- ps
    }

    def instantiate(var scene : DnPackedScene?; var world : DnGameWorld?) : uint64 {
        if (_handle == null || scene == null) {
            return 0ul
        }
        return dn_scenebuilder_instantiate(_handle, scene._handle, world._handle)
    }

    def instantiate_as_children(var scene : DnPackedScene?; parent : uint64; var world : DnGameWorld?) : uint64 {
        if (_handle == null || scene == null) {
            return 0ul
        }
        return dn_scenebuilder_instantiate_as_children(_handle, scene._handle, parent, world._handle)
    }

    def serialize_to_file(var scene : DnPackedScene?; vpath : string) : void {
        if (_handle == null || scene == null) {
            return
        }
        dn_scenebuilder_serialize_to_file(_handle, scene._handle, vpath)
    }

    def deserialize_from_file(vpath : string) : DnPackedScene? {
        if (_handle == null) {
            return null
        }
        var ps = new DnPackedScene()
        unsafe {
            ps._handle = dn_scenebuilder_deserialize_from_file(_handle, vpath)
        }
        return <- ps
    }
}
