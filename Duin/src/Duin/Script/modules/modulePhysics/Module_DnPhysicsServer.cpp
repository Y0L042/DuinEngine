#include "dnpch.h"
#include "Module_DnPhysicsServer.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/misc/vectypes.h>

#include "Duin/Physics/PhysicsIncludes.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/DaslangConversionHelpers.h"

static void dn_physics_server_step_impl(double delta)
{
    duin::PhysicsServer::Get().StepPhysics(delta);
}

static void dn_physics_server_create_plane_impl(das::float3 normal, float height)
{
    duin::PhysicsServer::Get().CreatePlane(duin::from_f3(normal), height);
}

static void dn_physics_server_create_box_impl(das::float3 position, das::float3 size)
{
    duin::PhysicsServer::Get().CreateBox(duin::from_f3(position), duin::from_f3(size));
}

static void dn_physics_server_debug_draw_impl()
{
    duin::PhysicsServer::Get().DebugDrawBodies();
}

// ---- Module: dn_physics_server_core ----

class Module_DnPhysicsServer : public das::Module
{
    bool initialized = false;

  public:
    Module_DnPhysicsServer() : das::Module("dn_physics_server_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnPhysicsServer: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnPhysicsServer: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);

        addExtern<DAS_BIND_FUN(dn_physics_server_step_impl)>(*this, lib, "dn_physics_server_step_impl",
                                                             das::SideEffects::modifyExternal,
                                                             "dn_physics_server_step_impl")
            ->args({"delta"});
        addExtern<DAS_BIND_FUN(dn_physics_server_create_plane_impl)>(*this, lib,
                                                                     "dn_physics_server_create_plane_impl",
                                                                     das::SideEffects::modifyExternal,
                                                                     "dn_physics_server_create_plane_impl")
            ->args({"normal", "height"});
        addExtern<DAS_BIND_FUN(dn_physics_server_create_box_impl)>(*this, lib, "dn_physics_server_create_box_impl",
                                                                   das::SideEffects::modifyExternal,
                                                                   "dn_physics_server_create_box_impl")
            ->args({"position", "size"});
        addExtern<DAS_BIND_FUN(dn_physics_server_debug_draw_impl)>(*this, lib, "dn_physics_server_debug_draw_impl",
                                                                   das::SideEffects::modifyExternal,
                                                                   "dn_physics_server_debug_draw_impl");

        DN_CORE_INFO("Script Module [dn_physics_server_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnPhysicsServer);
