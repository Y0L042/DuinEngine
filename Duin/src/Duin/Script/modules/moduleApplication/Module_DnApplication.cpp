#include "dnpch.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>

#include "Duin/Core/Application.h"
#include "Duin/Core/Window/WindowCtx.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/ScriptContext.h"
#include "Duin/Objects/GameObject.h"
#include "Duin/Objects/GameObjectImpl.h"

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

static void dn_app_add_child_object_impl(void *childHandle, das::Context *context)
{
    if (!childHandle)
        return;

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (!(dnCtx && dnCtx->rootGameObject))
    {
        return;
    }

    auto *parent = dnCtx->rootGameObject;
    auto *child = static_cast<duin::GameObject *>(childHandle);

    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;

    for (auto &existing : parentImpl->childImpls)
    {
        if (existing->uuid == childImpl->uuid)
            return;
    }

    auto childOwner = dnCtx->scriptMemory->Get<duin::GameObject>(childHandle);
    if (!childOwner)
        return;

    parentImpl->AddChild(childImpl, childOwner);
    childImpl->parentImpl = parentImpl.get();
    child->SetParent(parent);

    if (!child->IsInitialized())
        child->_InitializeImpl();
    if (parent->IsReady() && !child->IsReady())
        child->ObjectReady();
}

// ---- Window wrappers ----

static duin::WindowCtx *dn_window_from_handle(void *handle, const char *fn)
{
    if (!handle)
    {
        DN_CORE_WARN("{}: null window handle", fn);
        return nullptr;
    }
    return static_cast<duin::WindowCtx *>(handle);
}

static void *dn_window_create_impl(const char *title, int width, int height, uint64_t flags, das::Context *context)
{
    std::shared_ptr<duin::WindowCtx> window = duin::CreateNewWindow(title ? title : "", width, height, flags);
    if (!window)
    {
        DN_CORE_WARN("dn_window_create_impl: failed to create window '{}'", title ? title : "");
        return nullptr;
    }
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(window));
}

static void dn_window_release_impl(void *handle, das::Context *context)
{
    if (!handle)
        return;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    dnCtx->scriptMemory->Remove(handle);
}

static bool dn_window_close_impl(void *handle, das::Context *context)
{
    if (!dn_window_from_handle(handle, "dn_window_close_impl"))
        return false;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    std::shared_ptr<duin::WindowCtx> window = dnCtx->scriptMemory->Get<duin::WindowCtx>(handle);
    return duin::DestroyWindow(window);
}

static bool dn_window_is_valid_impl(void *handle)
{
    if (!handle)
        return false;
    auto *window = static_cast<duin::WindowCtx *>(handle);
    return window->IsWindowValid() && !window->IsPendingDestroy();
}

static bool dn_window_is_minimized_impl(void *handle)
{
    if (!dn_window_is_valid_impl(handle))
        return false;
    return (static_cast<duin::WindowCtx *>(handle)->GetSDLWindowFlags() & SDL_WINDOW_MINIMIZED) != 0;
}

static int dn_window_get_width_impl(void *handle)
{
    auto *window = dn_window_from_handle(handle, "dn_window_get_width_impl");
    return window ? window->GetWidth() : 0;
}

static int dn_window_get_height_impl(void *handle)
{
    auto *window = dn_window_from_handle(handle, "dn_window_get_height_impl");
    return window ? window->GetHeight() : 0;
}

static void dn_window_set_size_impl(void *handle, int width, int height)
{
    if (auto *window = dn_window_from_handle(handle, "dn_window_set_size_impl"))
        window->SetSize(width, height);
}

static const char *dn_window_get_title_impl(void *handle, das::Context *context)
{
    auto *window = dn_window_from_handle(handle, "dn_window_get_title_impl");
    std::string title = window ? window->GetTitle() : std::string();
    return context->allocateString(title, nullptr);
}

static void dn_window_set_title_impl(void *handle, const char *title)
{
    if (auto *window = dn_window_from_handle(handle, "dn_window_set_title_impl"))
        window->SetTitle(title ? title : "");
}

static uint64_t dn_window_get_flags_impl(void *handle)
{
    auto *window = dn_window_from_handle(handle, "dn_window_get_flags_impl");
    return window ? static_cast<uint64_t>(window->GetSDLWindowFlags()) : 0;
}

static void dn_window_set_flags_impl(void *handle, uint64_t flags)
{
    if (auto *window = dn_window_from_handle(handle, "dn_window_set_flags_impl"))
        window->SetSDLWindowFlags(static_cast<SDL_WindowFlags>(flags));
}

static uint32_t dn_window_get_clear_color_impl(void *handle)
{
    auto *window = dn_window_from_handle(handle, "dn_window_get_clear_color_impl");
    return window ? window->GetClearColor() : 0;
}

static void dn_window_set_clear_color_impl(void *handle, uint32_t rgba)
{
    if (auto *window = dn_window_from_handle(handle, "dn_window_set_clear_color_impl"))
        window->SetClearColor(rgba);
}

static void dn_window_set_camera_impl(void *handle, void *cameraHandle)
{
    if (auto *window = dn_window_from_handle(handle, "dn_window_set_camera_impl"))
        window->SetCamera(static_cast<duin::Camera *>(cameraHandle));
}

static std::function<void(void)> dn_window_make_callback(const das::TLambda<void> &lambda, das::Context *context)
{
    auto root = std::make_shared<das::GcRootLambda>(lambda, context);
    return [root, context]() {
        bool ok = context->runWithCatch([&]() { das::das_invoke_lambda<void>::invoke(context, nullptr, *root); });
        if (!ok)
        {
            if (auto ex = context->getException())
                DN_CORE_ERROR("[Script] Exception in window draw callback: {}", ex);
            context->clearException();
        }
    };
}

static void dn_window_queue_draw_impl(void *handle, das::TLambda<void> lambda, das::Context *context)
{
    if (!dn_window_is_valid_impl(handle))
        return;
    static_cast<duin::WindowCtx *>(handle)->QueueDraw(dn_window_make_callback(lambda, context));
}

static void dn_window_queue_draw_ui_impl(void *handle, das::TLambda<void> lambda, das::Context *context)
{
    if (!dn_window_is_valid_impl(handle))
        return;
    static_cast<duin::WindowCtx *>(handle)->QueueDrawUI(dn_window_make_callback(lambda, context));
}

static int dn_window_count_impl()
{
    return static_cast<int>(duin::GetSecondaryWindows().size());
}

// ---- Module ----

class Module_DnApplication : public das::Module
{
    bool initialized = false;

  public:
    Module_DnApplication() : das::Module("dn_application_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;
        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();

        // Add child to application root
        addExtern<DAS_BIND_FUN(dn_app_add_child_object_impl)>(
            *this,
            lib,
            "dn_app_add_child_object_impl",
            das::SideEffects::modifyExternal,
            "dn_app_add_child_object_impl")
            ->args({"child", "context"});

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

        // Quit control (used by headless test scripts to terminate the app).
        // The setter is wrapped by a defaulted dn_set_game_should_quit() in
        // dn_application.das so scripts can call it with no argument.
        addExtern<DAS_BIND_FUN(duin::GetGameShouldQuit)>(
            *this, lib, "dn_get_game_should_quit", das::SideEffects::accessGlobal, "duin::GetGameShouldQuit");
        addExtern<DAS_BIND_FUN(duin::SetGameShouldQuit)>(
            *this, lib, "dn_set_game_should_quit_impl", das::SideEffects::modifyExternal, "duin::SetGameShouldQuit")
            ->args({"shouldQuit"});

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

        // Secondary windows
        addConstant(*this, "DN_WINDOW_FULLSCREEN", static_cast<uint64_t>(SDL_WINDOW_FULLSCREEN));
        addConstant(*this, "DN_WINDOW_HIDDEN", static_cast<uint64_t>(SDL_WINDOW_HIDDEN));
        addConstant(*this, "DN_WINDOW_BORDERLESS", static_cast<uint64_t>(SDL_WINDOW_BORDERLESS));
        addConstant(*this, "DN_WINDOW_RESIZABLE", static_cast<uint64_t>(SDL_WINDOW_RESIZABLE));
        addConstant(*this, "DN_WINDOW_MINIMIZED", static_cast<uint64_t>(SDL_WINDOW_MINIMIZED));
        addConstant(*this, "DN_WINDOW_MAXIMIZED", static_cast<uint64_t>(SDL_WINDOW_MAXIMIZED));
        addConstant(*this, "DN_WINDOW_ALWAYS_ON_TOP", static_cast<uint64_t>(SDL_WINDOW_ALWAYS_ON_TOP));

        addExtern<DAS_BIND_FUN(dn_window_create_impl)>(
            *this, lib, "dn_window_create_impl", das::SideEffects::modifyExternal, "dn_window_create_impl")
            ->args({"title", "width", "height", "flags", "context"});
        addExtern<DAS_BIND_FUN(dn_window_release_impl)>(
            *this, lib, "dn_window_release_impl", das::SideEffects::modifyExternal, "dn_window_release_impl")
            ->args({"handle", "context"});
        addExtern<DAS_BIND_FUN(dn_window_close_impl)>(
            *this, lib, "dn_window_close_impl", das::SideEffects::modifyExternal, "dn_window_close_impl")
            ->args({"handle", "context"});
        addExtern<DAS_BIND_FUN(dn_window_is_valid_impl)>(
            *this, lib, "dn_window_is_valid_impl", das::SideEffects::accessExternal, "dn_window_is_valid_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_is_minimized_impl)>(
            *this, lib, "dn_window_is_minimized_impl", das::SideEffects::accessExternal, "dn_window_is_minimized_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_get_width_impl)>(
            *this, lib, "dn_window_get_width_impl", das::SideEffects::accessExternal, "dn_window_get_width_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_get_height_impl)>(
            *this, lib, "dn_window_get_height_impl", das::SideEffects::accessExternal, "dn_window_get_height_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_set_size_impl)>(
            *this, lib, "dn_window_set_size_impl", das::SideEffects::modifyExternal, "dn_window_set_size_impl")
            ->args({"handle", "width", "height"});
        addExtern<DAS_BIND_FUN(dn_window_get_title_impl)>(
            *this, lib, "dn_window_get_title_impl", das::SideEffects::accessExternal, "dn_window_get_title_impl")
            ->args({"handle", "context"});
        addExtern<DAS_BIND_FUN(dn_window_set_title_impl)>(
            *this, lib, "dn_window_set_title_impl", das::SideEffects::modifyExternal, "dn_window_set_title_impl")
            ->args({"handle", "title"});
        addExtern<DAS_BIND_FUN(dn_window_get_flags_impl)>(
            *this, lib, "dn_window_get_flags_impl", das::SideEffects::accessExternal, "dn_window_get_flags_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_set_flags_impl)>(
            *this, lib, "dn_window_set_flags_impl", das::SideEffects::modifyExternal, "dn_window_set_flags_impl")
            ->args({"handle", "flags"});
        addExtern<DAS_BIND_FUN(dn_window_get_clear_color_impl)>(
            *this,
            lib,
            "dn_window_get_clear_color_impl",
            das::SideEffects::accessExternal,
            "dn_window_get_clear_color_impl")
            ->args({"handle"});
        addExtern<DAS_BIND_FUN(dn_window_set_clear_color_impl)>(
            *this,
            lib,
            "dn_window_set_clear_color_impl",
            das::SideEffects::modifyExternal,
            "dn_window_set_clear_color_impl")
            ->args({"handle", "rgba"});
        addExtern<DAS_BIND_FUN(dn_window_set_camera_impl)>(
            *this, lib, "dn_window_set_camera_impl", das::SideEffects::modifyExternal, "dn_window_set_camera_impl")
            ->args({"handle", "camera"});
        addExtern<DAS_BIND_FUN(dn_window_queue_draw_impl)>(
            *this, lib, "dn_window_queue_draw_impl", das::SideEffects::modifyExternal, "dn_window_queue_draw_impl")
            ->args({"handle", "fn", "context"});
        addExtern<DAS_BIND_FUN(dn_window_queue_draw_ui_impl)>(
            *this,
            lib,
            "dn_window_queue_draw_ui_impl",
            das::SideEffects::modifyExternal,
            "dn_window_queue_draw_ui_impl")
            ->args({"handle", "fn", "context"});
        addExtern<DAS_BIND_FUN(dn_window_count_impl)>(
            *this, lib, "dn_window_count", das::SideEffects::accessGlobal, "dn_window_count_impl");

        DN_CORE_INFO("Script Module [dn_application_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnApplication);
