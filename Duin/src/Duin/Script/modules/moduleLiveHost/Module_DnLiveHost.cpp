#include "dnpch.h"
#include "Module_DnLiveHost.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/ast/ast_interop.h>
#include <daScript/simulate/aot_builtin.h>

#include "Duin/Core/Debug/DNLog.h"

DnLiveHostState g_DnLiveHostState;

// ---- Free function bindings ----

static bool dn_live_is_live_mode_impl()
{
    return g_DnLiveHostState.live_mode;
}

static bool dn_live_is_reload_impl()
{
    return g_DnLiveHostState.is_reload;
}

static void dn_live_request_exit_impl()
{
    g_DnLiveHostState.exit_requested = true;
}

static bool dn_live_exit_requested_impl()
{
    return g_DnLiveHostState.exit_requested;
}

static void dn_live_request_reload_impl(bool /*full*/)
{
    g_DnLiveHostState.reload_requested = true;
}

static float dn_live_get_dt_impl()
{
    return g_DnLiveHostState.dt;
}

static float dn_live_get_uptime_impl()
{
    return g_DnLiveHostState.uptime;
}

static float dn_live_get_fps_impl()
{
    return g_DnLiveHostState.fps;
}

static bool dn_live_is_paused_impl()
{
    return g_DnLiveHostState.paused;
}

static void dn_live_set_paused_impl(bool v)
{
    g_DnLiveHostState.paused = v;
}

static const char *dn_live_get_last_error_impl(das::Context *ctx)
{
    return ctx->allocateString(g_DnLiveHostState.last_error, nullptr);
}

static void dn_live_store_bytes_impl(const char *key, const das::TArray<uint8_t> &data)
{
    if (!key)
        return;
    auto &slot = g_DnLiveHostState.store[key];
    slot.assign(data.data, data.data + data.size);
}

static bool dn_live_load_bytes_impl(const char *key, das::TArray<uint8_t> &arr, das::Context *ctx)
{
    if (!key)
        return false;
    auto it = g_DnLiveHostState.store.find(key);
    if (it == g_DnLiveHostState.store.end())
        return false;
    const auto &bytes = it->second;
    das::builtin_array_clear(reinterpret_cast<das::Array &>(arr), ctx, nullptr);
    das::builtin_array_resize(reinterpret_cast<das::Array &>(arr), (int)bytes.size(), sizeof(uint8_t), ctx, nullptr);
    if (!bytes.empty())
        memcpy(arr.data, bytes.data(), bytes.size());
    return true;
}

// ---- Marker annotations for [before_reload] / [after_reload] ----

struct BeforeReloadAnnotation : das::FunctionAnnotation
{
    BeforeReloadAnnotation() : das::FunctionAnnotation("before_reload")
    {
    }

    bool apply(
        const das::FunctionPtr &, das::ModuleGroup &, const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool finalize(
        const das::FunctionPtr &, das::ModuleGroup &, const das::AnnotationArgumentList &,
        const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool apply(das::ExprBlock *, das::ModuleGroup &, const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool finalize(
        das::ExprBlock *, das::ModuleGroup &, const das::AnnotationArgumentList &, const das::AnnotationArgumentList &,
        das::string &) override
    {
        return true;
    }
};

struct AfterReloadAnnotation : das::FunctionAnnotation
{
    AfterReloadAnnotation() : das::FunctionAnnotation("after_reload")
    {
    }

    bool apply(
        const das::FunctionPtr &, das::ModuleGroup &, const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool finalize(
        const das::FunctionPtr &, das::ModuleGroup &, const das::AnnotationArgumentList &,
        const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool apply(das::ExprBlock *, das::ModuleGroup &, const das::AnnotationArgumentList &, das::string &) override
    {
        return true;
    }

    bool finalize(
        das::ExprBlock *, das::ModuleGroup &, const das::AnnotationArgumentList &, const das::AnnotationArgumentList &,
        das::string &) override
    {
        return true;
    }
};

// ---- Module ----

class Module_DnLiveHost : public das::Module
{
    bool initialized = false;

  public:
    Module_DnLiveHost() : das::Module("dn_live_host_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        addAnnotation(new BeforeReloadAnnotation());
        addAnnotation(new AfterReloadAnnotation());

        das::addExtern<DAS_BIND_FUN(dn_live_is_live_mode_impl)>(
            *this, lib, "is_live_mode", das::SideEffects::accessGlobal, "dn_live_is_live_mode_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_is_reload_impl)>(
            *this, lib, "is_reload", das::SideEffects::accessGlobal, "dn_live_is_reload_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_request_exit_impl)>(
            *this, lib, "request_exit", das::SideEffects::modifyExternal, "dn_live_request_exit_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_exit_requested_impl)>(
            *this, lib, "exit_requested", das::SideEffects::accessGlobal, "dn_live_exit_requested_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_request_reload_impl)>(
            *this, lib, "request_reload", das::SideEffects::modifyExternal, "dn_live_request_reload_impl")
            ->args({"full"});
        das::addExtern<DAS_BIND_FUN(dn_live_get_dt_impl)>(
            *this, lib, "get_dt", das::SideEffects::accessGlobal, "dn_live_get_dt_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_get_uptime_impl)>(
            *this, lib, "get_uptime", das::SideEffects::accessGlobal, "dn_live_get_uptime_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_get_fps_impl)>(
            *this, lib, "get_fps", das::SideEffects::accessGlobal, "dn_live_get_fps_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_is_paused_impl)>(
            *this, lib, "is_paused", das::SideEffects::accessGlobal, "dn_live_is_paused_impl");
        das::addExtern<DAS_BIND_FUN(dn_live_set_paused_impl)>(
            *this, lib, "set_paused", das::SideEffects::modifyExternal, "dn_live_set_paused_impl")
            ->args({"paused"});
        das::addExtern<DAS_BIND_FUN(dn_live_get_last_error_impl)>(
            *this, lib, "get_last_error", das::SideEffects::accessGlobal, "dn_live_get_last_error_impl")
            ->args({"context"});
        das::addExtern<DAS_BIND_FUN(dn_live_store_bytes_impl)>(
            *this, lib, "live_store_bytes", das::SideEffects::modifyExternal, "dn_live_store_bytes_impl")
            ->args({"key", "data"});
        das::addExtern<DAS_BIND_FUN(dn_live_load_bytes_impl)>(
            *this, lib, "live_load_bytes", das::SideEffects::modifyExternal, "dn_live_load_bytes_impl")
            ->args({"key", "data", "context"});

        DN_CORE_INFO("Script Module [dn_live_host] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnLiveHost);
