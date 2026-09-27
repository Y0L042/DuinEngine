options gen2

module dn_assets shared public

require dn_assetmanager_core public
require dn_model_core public

// Script-side face of duin::Model. The underlying model is owned by the
// context's ScriptMemory; _handle is an opaque key into it.
//
// The decoded mesh is cached by the AssetManager, so loading the same path
// twice is cheap and the two handles stay independent.
class DnModel {
    _handle : void?

    def load(path : string) : bool {
        destroy()
        _handle = dn_asset_manager_load_model_impl(path)
        return _handle != null
    }

    def is_valid() : bool {
        return dn_model_is_valid_impl(_handle)
    }

    // Only valid between dn_begin_draw_3d and dn_end_draw_3d.
    def draw(position : float3; rotation : float4; scale : float3) : void {
        dn_model_draw_impl(_handle, position, rotation, scale)
    }

    def get_path() : string {
        return dn_model_get_path_impl(_handle)
    }

    def get_uuid() : uint64 {
        return dn_model_get_uuid_impl(_handle)
    }

    def set_base_scale(scale : float3) : void {
        dn_model_set_base_scale_impl(_handle, scale)
    }

    def get_base_scale() : float3 {
        return dn_model_get_base_scale_impl(_handle)
    }

    def destroy() : void {
        if (_handle != null) {
            dn_model_destroy_impl(_handle)
            _handle = null
        }
    }
}
