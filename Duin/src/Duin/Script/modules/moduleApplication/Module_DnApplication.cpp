#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>

#include "Duin/Core/Application.h"
#include "Duin/Core/Debug/DNLog.h"

// ---- Wrappers ----

static const char *dn_get_root_directory_impl(das::Context *ctx)
{
    std::string result = duin::GetRootDirectory();
    return ctx->allocateString(result, nullptr);
}

static int dn_get_physics_frame_count_impl()
{
    return static_cast<int>(duin::GetPhysicsFrameCount());
}

static int dn_get_render_frame_count_impl()
{
    return static_cast<int>(duin::GetRenderFrameCount());
}

// ---- Module ----

class Module_DnApplication : public das::Module
{
    bool initialized = false;

  public:
    Module_DnApplication() : das::Module("dn_application")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        // Root directory
        addExtern<DAS_BIND_FUN(dn_get_root_directory_impl)>(
            *this, lib, "dn_get_root_directory", das::SideEffects::accessGlobal, "dn_get_root_directory_impl")
            ->args({"context"});

        // Debug pause/resume
        addExtern<DAS_BIND_FUN(duin::DebugPauseGame)>(
            *this, lib, "dn_debug_pause_game", das::SideEffects::modifyExternal, "duin::DebugPauseGame");
        addExtern<DAS_BIND_FUN(duin::DebugResumeGame)>(
            *this, lib, "dn_debug_resume_game", das::SideEffects::modifyExternal, "duin::DebugResumeGame");
        addExtern<DAS_BIND_FUN(duin::DebugIsGamePaused)>(
            *this, lib, "dn_debug_is_game_paused", das::SideEffects::accessGlobal, "duin::DebugIsGamePaused");

        // Physics pause/resume
        addExtern<DAS_BIND_FUN(duin::PausePhysics)>(
            *this, lib, "dn_pause_physics", das::SideEffects::modifyExternal, "duin::PausePhysics");
        addExtern<DAS_BIND_FUN(duin::ResumePhysics)>(
            *this, lib, "dn_resume_physics", das::SideEffects::modifyExternal, "duin::ResumePhysics");
        addExtern<DAS_BIND_FUN(duin::IsPhysicsPaused)>(
            *this, lib, "dn_is_physics_paused", das::SideEffects::accessGlobal, "duin::IsPhysicsPaused");

        // Update pause/resume
        addExtern<DAS_BIND_FUN(duin::PauseUpdate)>(
            *this, lib, "dn_pause_update", das::SideEffects::modifyExternal, "duin::PauseUpdate");
        addExtern<DAS_BIND_FUN(duin::ResumeUpdate)>(
            *this, lib, "dn_resume_update", das::SideEffects::modifyExternal, "duin::ResumeUpdate");
        addExtern<DAS_BIND_FUN(duin::IsUpdatePaused)>(
            *this, lib, "dn_is_update_paused", das::SideEffects::accessGlobal, "duin::IsUpdatePaused");

        // Framerate
        addExtern<DAS_BIND_FUN(duin::SetFramerate)>(
            *this, lib, "dn_set_framerate", das::SideEffects::modifyExternal, "duin::SetFramerate")
            ->args({"framerate"});

        // Draw FPS overlays
        addExtern<DAS_BIND_FUN(duin::DrawPhysicsFPS)>(
            *this, lib, "dn_draw_physics_fps", das::SideEffects::modifyExternal, "duin::DrawPhysicsFPS")
            ->args({"x", "y"});
        addExtern<DAS_BIND_FUN(duin::DrawRenderFPS)>(
            *this, lib, "dn_draw_render_fps", das::SideEffects::modifyExternal, "duin::DrawRenderFPS")
            ->args({"x", "y"});

        // FPS / frame-time queries
        addExtern<DAS_BIND_FUN(duin::GetPhysicsFPS)>(
            *this, lib, "dn_get_physics_fps", das::SideEffects::accessGlobal, "duin::GetPhysicsFPS");
        addExtern<DAS_BIND_FUN(duin::GetPhysicsFrameTime)>(
            *this, lib, "dn_get_physics_frame_time", das::SideEffects::accessGlobal, "duin::GetPhysicsFrameTime");
        addExtern<DAS_BIND_FUN(duin::GetRenderFPS)>(
            *this, lib, "dn_get_render_fps", das::SideEffects::accessGlobal, "duin::GetRenderFPS");
        addExtern<DAS_BIND_FUN(duin::GetRenderFrameTime)>(
            *this, lib, "dn_get_render_frame_time", das::SideEffects::accessGlobal, "duin::GetRenderFrameTime");

        // Frame counts (size_t → int)
        addExtern<DAS_BIND_FUN(dn_get_physics_frame_count_impl)>(
            *this,
            lib,
            "dn_get_physics_frame_count",
            das::SideEffects::accessGlobal,
            "dn_get_physics_frame_count_impl");
        addExtern<DAS_BIND_FUN(dn_get_render_frame_count_impl)>(
            *this, lib, "dn_get_render_frame_count", das::SideEffects::accessGlobal, "dn_get_render_frame_count_impl");

        // Timing
        addExtern<DAS_BIND_FUN(duin::GetTicks)>(
            *this, lib, "dn_get_ticks", das::SideEffects::accessGlobal, "duin::GetTicks");
        addExtern<DAS_BIND_FUN(duin::GetTicksMilli)>(
            *this, lib, "dn_get_ticks_milli", das::SideEffects::accessGlobal, "duin::GetTicksMilli");
        addExtern<DAS_BIND_FUN(duin::GetTicksNano)>(
            *this, lib, "dn_get_ticks_nano", das::SideEffects::accessGlobal, "duin::GetTicksNano");

        // Delay
        addExtern<DAS_BIND_FUN(duin::DelayProcess)>(
            *this, lib, "dn_delay_process", das::SideEffects::modifyExternal, "duin::DelayProcess")
            ->args({"seconds"});
        addExtern<DAS_BIND_FUN(duin::DelayProcessMilli)>(
            *this, lib, "dn_delay_process_milli", das::SideEffects::modifyExternal, "duin::DelayProcessMilli")
            ->args({"milliseconds"});

        // Window queries
        addExtern<DAS_BIND_FUN(duin::GetWindowWidth)>(
            *this, lib, "dn_get_window_width", das::SideEffects::accessGlobal, "duin::GetWindowWidth");
        addExtern<DAS_BIND_FUN(duin::GetWindowHeight)>(
            *this, lib, "dn_get_window_height", das::SideEffects::accessGlobal, "duin::GetWindowHeight");

        DN_CORE_INFO("Script Module [dn_application] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnApplication);
