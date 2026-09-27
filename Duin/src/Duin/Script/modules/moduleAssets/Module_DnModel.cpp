#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_interop.h>

#include "Duin/Assets/Model.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/DaslangConversionHelpers.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Script/ScriptMemory.h"

// --- Helpers ---

static duin::Model *get_model(void *handle)
{
    return static_cast<duin::Model *>(handle);
}

// --- Draw ---

// Only valid while the render encoder is live, i.e. between BeginDraw3D and
// EndDraw3D. Model::Draw forwards to QueueRender, which submits on the global
// encoder.
static void dn_model_draw_impl(void *handle, das::float3 position, das::float4 rotation, das::float3 scale)
{
    if (!handle)
        return;
    get_model(handle)->Draw(duin::from_f3(position), duin::from_f4(rotation), duin::from_f3(scale));
}

// --- Queries ---

static bool dn_model_is_valid_impl(void *handle)
{
    return handle && get_model(handle)->data != nullptr;
}

static const char *dn_model_get_path_impl(void *handle, das::Context *context)
{
    if (!handle)
        return nullptr;
    return context->allocateString(get_model(handle)->path, nullptr);
}

static uint64_t dn_model_get_uuid_impl(void *handle)
{
    if (!handle)
        return 0ul;
    auto *model = get_model(handle);
    if (!model->data)
        return 0ul;
    return static_cast<uint64_t>(model->data->uuid);
}

// --- Base scale ---

static void dn_model_set_base_scale_impl(void *handle, das::float3 scale)
{
    if (!handle)
        return;
    get_model(handle)->baseScale = duin::from_f3(scale);
}

static das::float3 dn_model_get_base_scale_impl(void *handle)
{
    if (!handle)
        return {};
    return duin::to_f3(get_model(handle)->baseScale);
}

// --- Destroy ---

static void dn_model_destroy_impl(void *handle, das::Context *context)
{
    if (!handle)
        return;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    dnCtx->scriptMemory->Remove(handle);
}

// ========================================================================
// Module
// ========================================================================

class Module_DnModel : public das::Module
{
    bool initialized = false;

  public:
    Module_DnModel() : das::Module("dn_model_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        das::addExtern<DAS_BIND_FUN(dn_model_draw_impl)>(
            *this, lib, "dn_model_draw_impl", das::SideEffects::modifyExternal, "dn_model_draw_impl")
            ->args({"handle", "position", "rotation", "scale"});

        das::addExtern<DAS_BIND_FUN(dn_model_is_valid_impl)>(
            *this, lib, "dn_model_is_valid_impl", das::SideEffects::none, "dn_model_is_valid_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_model_get_path_impl)>(
            *this, lib, "dn_model_get_path_impl", das::SideEffects::none, "dn_model_get_path_impl")
            ->args({"handle", "context"});

        das::addExtern<DAS_BIND_FUN(dn_model_get_uuid_impl)>(
            *this, lib, "dn_model_get_uuid_impl", das::SideEffects::none, "dn_model_get_uuid_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_model_set_base_scale_impl)>(
            *this, lib, "dn_model_set_base_scale_impl", das::SideEffects::modifyArgumentAndExternal,
            "dn_model_set_base_scale_impl")
            ->args({"handle", "scale"});

        das::addExtern<DAS_BIND_FUN(dn_model_get_base_scale_impl)>(
            *this, lib, "dn_model_get_base_scale_impl", das::SideEffects::none, "dn_model_get_base_scale_impl")
            ->args({"handle"});

        das::addExtern<DAS_BIND_FUN(dn_model_destroy_impl)>(
            *this, lib, "dn_model_destroy_impl", das::SideEffects::modifyExternal, "dn_model_destroy_impl")
            ->args({"handle", "context"});

        DN_CORE_INFO("Script Module [dn_model_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnModel);
