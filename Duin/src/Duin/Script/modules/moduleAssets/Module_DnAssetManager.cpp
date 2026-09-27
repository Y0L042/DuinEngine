#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_interop.h>

#include "Duin/Assets/AssetManager.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Script/ScriptMemory.h"

// ========================================================================
// Wrappers
//
// duin::AssetManager is a singleton (AssetManager::Get()), so it is bound as
// free functions rather than an annotated class.
// ========================================================================

// LoadModel returns std::optional<Model>, which cannot cross the binding
// boundary. The model is heap-allocated and registered with the context's
// ScriptMemory; script sees an opaque void? handle. Returns null on failure.
//
// The decoded ModelData stays cached in AssetManager::assetMap independently,
// so releasing a handle never frees the GPU buffers.
static void *dn_asset_manager_load_model_impl(const char *path, das::Context *context)
{
    if (!path || !*path)
        return nullptr;

    std::optional<duin::Model> model = duin::AssetManager::Get().LoadModel(path);
    if (!model.has_value())
    {
        DN_CORE_ERROR("Model {} failed to load!", path);
        return nullptr;
    }

    auto obj = std::make_shared<duin::Model>(std::move(*model));

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
}

static bool dn_asset_manager_has_asset_impl(const char *path)
{
    if (!path || !*path)
        return false;
    return duin::AssetManager::Get().HasAsset(path);
}

static uint64_t dn_asset_manager_get_uuid_impl()
{
    return static_cast<uint64_t>(duin::AssetManager::Get().GetUUID());
}

// ========================================================================
// Module
// ========================================================================

class Module_DnAssetManager : public das::Module
{
    bool initialized = false;

  public:
    Module_DnAssetManager() : das::Module("dn_assetmanager_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        das::addExtern<DAS_BIND_FUN(dn_asset_manager_load_model_impl)>(
            *this,
            lib,
            "dn_asset_manager_load_model_impl",
            das::SideEffects::modifyExternal,
            "dn_asset_manager_load_model_impl")
            ->args({"path", "context"});

        das::addExtern<DAS_BIND_FUN(dn_asset_manager_has_asset_impl)>(
            *this, lib, "dn_asset_manager_has_asset", das::SideEffects::accessGlobal,
            "dn_asset_manager_has_asset_impl")
            ->args({"path"});

        das::addExtern<DAS_BIND_FUN(dn_asset_manager_get_uuid_impl)>(
            *this, lib, "dn_asset_manager_get_uuid", das::SideEffects::accessGlobal,
            "dn_asset_manager_get_uuid_impl");

        DN_CORE_INFO("Script Module [dn_assetmanager_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnAssetManager);
