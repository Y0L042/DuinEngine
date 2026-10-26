options gen2
require dastest/testing_boost public

require dn_log
require dn_assets

[test]
def test_model_is_invalid_before_load() {
    var model = new DnModel()
    verify(!model.is_valid(), "a fresh DnModel is not valid")
    verify(model.get_uuid() == 0ul, "a fresh DnModel has no uuid")
    unsafe {
        delete model
    }
}

[test]
def test_model_load_missing_path_fails() {
    var model = new DnModel()
    verify(!model.load("./artifacts/does_not_exist_model.obj"), "loading a missing file fails")
    verify(!model.is_valid(), "model stays invalid after a failed load")
    unsafe {
        delete model
    }
}

[test]
def test_model_load_empty_path_fails() {
    var model = new DnModel()
    verify(!model.load(""), "loading an empty path fails")
    verify(!model.is_valid(), "model stays invalid after an empty-path load")
    unsafe {
        delete model
    }
}

[test]
def test_model_base_scale_is_noop_without_model() {
    var model = new DnModel()
    model.set_base_scale(float3(2.0f, 2.0f, 2.0f))
    let s = model.get_base_scale()
    verify(s.x == 0.0f && s.y == 0.0f && s.z == 0.0f, "base scale is not stored without a loaded model")
    unsafe {
        delete model
    }
}

[test]
def test_model_destroy_unloaded_is_safe() {
    var model = new DnModel()
    model.destroy()
    model.destroy()
    verify(!model.is_valid(), "destroying an unloaded model is a no-op")
    unsafe {
        delete model
    }
}

[test]
def test_model_draw_unloaded_is_safe() {
    var model = new DnModel()
    model.draw(float3(0.0f), float4(0.0f, 0.0f, 0.0f, 1.0f), float3(1.0f))
    verify(!model.is_valid(), "drawing an unloaded model is a no-op")
    unsafe {
        delete model
    }
}

[test]
def test_asset_manager_has_asset_unknown_path() {
    verify(!dn_asset_manager_has_asset("./artifacts/never_loaded.obj"), "unknown path is not a loaded asset")
}

[test]
def test_asset_manager_uuid_is_stable() {
    verify(dn_asset_manager_get_uuid() == dn_asset_manager_get_uuid(), "asset manager uuid is stable")
}
