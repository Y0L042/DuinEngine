#include "dnpch.h"
#include "Module_DnPipeline.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>

#include "Duin/Core/Application.h"
#include "Duin/ECS/Pipeline.h"
#include "Duin/ECS/DECS/DECS.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Core/Debug/DNLog.h"

// ecs_world_t* is a bound external type (registered via MAKE_TYPE_FACTORY in
// flecs_core). This TU references it as an extern arg, so it needs the matching
// external factory to see the binding (see feedback_daslang_direct_type_binding).
MAKE_EXTERNAL_TYPE_FACTORY(ecs_world_t, ecs_world_t)

// Creates an EcsPipeline viewing the given flecs world and registers it in the
// context's ScriptMemory. Returns the raw EcsPipeline* as an opaque handle.
static void *dn_create_pipeline_impl(ecs_world_t *world, das::Context *context)
{
    if (!world)
        return nullptr;

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (!dnCtx || !dnCtx->scriptMemory)
        return nullptr;

    duin::World w(world);
    std::shared_ptr<duin::EcsPipeline> pipeline = duin::CreateEcsPipeline(w);
    DN_CORE_INFO("dn_create_pipeline_impl: created EcsPipeline");

    return static_cast<void *>(dnCtx->scriptMemory->Add(pipeline));
}

// Drops the pipeline from ScriptMemory. The last shared_ptr release also tears
// down the pipeline's lifecycle ScopedConnections.
static void dn_destroy_pipeline_impl(void *handle, das::Context *context)
{
    if (!handle)
        return;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (dnCtx && dnCtx->scriptMemory)
    {
        DN_CORE_INFO("dn_destroy_pipeline_impl: destroying EcsPipeline");
        dnCtx->scriptMemory->Remove(handle);
    }
}

static void dn_pipeline_add_stage_impl(void *handle, int step, const char *name)
{
    if (!handle || !name)
        return;
    auto *pipeline = static_cast<duin::EcsPipeline *>(handle);
    pipeline->AddStage(static_cast<duin::ExecutionStep>(step), name);
}

static void dn_pipeline_add_system_impl(
    void *handle, int step, const char *stageName, ecs_world_t *world, uint64_t systemEntityId)
{
    if (!handle || !stageName || !world || !systemEntityId)
        return;
    auto *pipeline = static_cast<duin::EcsPipeline *>(handle);
    duin::World w(world);
    duin::SystemHandle systemHandle{w, duin::Entity(w.GetFlecsWorld(), systemEntityId)};
    pipeline->AddSystem(static_cast<duin::ExecutionStep>(step), stageName, systemHandle);
}

static void dn_pipeline_run_all_impl(void *handle, float deltaTime)
{
    if (!handle)
        return;
    static_cast<duin::EcsPipeline *>(handle)->RunAll(deltaTime);
}

static void dn_pipeline_run_step_impl(void *handle, int step, float deltaTime)
{
    if (!handle)
        return;
    static_cast<duin::EcsPipeline *>(handle)->RunStep(static_cast<duin::ExecutionStep>(step), deltaTime);
}

static bool dn_pipeline_has_stage_impl(void *handle, int step, const char *name)
{
    if (!handle || !name)
        return false;
    auto *pipeline = static_cast<duin::EcsPipeline *>(handle);
    return pipeline->GetStage(static_cast<duin::ExecutionStep>(step), name).has_value();
}

class Module_DnPipeline : public das::Module
{
    bool initialized = false;

  public:
    Module_DnPipeline() : das::Module("dn_pipeline_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("dn_pipeline: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("dn_pipeline: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        auto *flecsMod = das::Module::require("flecs_core");
        if (!flecsMod)
        {
            DN_CORE_ERROR("dn_pipeline: required module 'flecs_core' not found");
            return false;
        }
        if (!flecsMod->initDependencies())
        {
            DN_CORE_ERROR("dn_pipeline: failed to initialize dependencies of 'flecs_core'");
            return false;
        }

        auto *ecsMod = das::Module::require("dn_ecs_core");
        if (!ecsMod)
        {
            DN_CORE_ERROR("dn_pipeline: required module 'dn_ecs_core' not found");
            return false;
        }
        if (!ecsMod->initDependencies())
        {
            DN_CORE_ERROR("dn_pipeline: failed to initialize dependencies of 'dn_ecs_core'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);
        addBuiltinDependency(lib, flecsMod);
        addBuiltinDependency(lib, ecsMod);

        addExtern<DAS_BIND_FUN(dn_create_pipeline_impl)>(
            *this, lib, "dn_create_pipeline_impl", das::SideEffects::modifyExternal, "dn_create_pipeline_impl")
            ->args({"world", "context"});

        addExtern<DAS_BIND_FUN(dn_destroy_pipeline_impl)>(
            *this, lib, "dn_destroy_pipeline_impl", das::SideEffects::modifyExternal, "dn_destroy_pipeline_impl")
            ->args({"handle", "context"});

        addExtern<DAS_BIND_FUN(dn_pipeline_add_stage_impl)>(
            *this, lib, "dn_pipeline_add_stage_impl", das::SideEffects::modifyExternal, "dn_pipeline_add_stage_impl")
            ->args({"handle", "step", "name"});

        addExtern<DAS_BIND_FUN(dn_pipeline_add_system_impl)>(
            *this, lib, "dn_pipeline_add_system_impl", das::SideEffects::modifyExternal, "dn_pipeline_add_system_impl")
            ->args({"handle", "step", "stageName", "world", "systemEntityId"});

        addExtern<DAS_BIND_FUN(dn_pipeline_run_all_impl)>(
            *this, lib, "dn_pipeline_run_all_impl", das::SideEffects::modifyExternal, "dn_pipeline_run_all_impl")
            ->args({"handle", "deltaTime"});

        addExtern<DAS_BIND_FUN(dn_pipeline_run_step_impl)>(
            *this, lib, "dn_pipeline_run_step_impl", das::SideEffects::modifyExternal, "dn_pipeline_run_step_impl")
            ->args({"handle", "step", "deltaTime"});

        addExtern<DAS_BIND_FUN(dn_pipeline_has_stage_impl)>(
            *this, lib, "dn_pipeline_has_stage_impl", das::SideEffects::none, "dn_pipeline_has_stage_impl")
            ->args({"handle", "step", "name"});

        DN_CORE_INFO("Script Module [dn_pipeline_core] initialized.");

        return true;
    }
};

REGISTER_MODULE(Module_DnPipeline);
REGISTER_DYN_MODULE(Module_DnPipeline, Module_DnPipeline);
