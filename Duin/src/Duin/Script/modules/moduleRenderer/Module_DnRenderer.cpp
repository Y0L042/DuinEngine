#include "dnpch.h"
#include "Module_DnRenderer.h"

#include <daScript/daScript.h>
#include <daScript/daScriptBind.h>
#include <daScript/misc/vectypes.h>

#include "Duin/Render/Renderer.h"
#include "Duin/Render/Camera.h"
#include "Duin/Render/Color.h"
#include "Duin/Render/RenderGeometry.h"
#include "Duin/Core/Maths/DuinMaths.h"
#include "Duin/Core/Utils/UUID.h"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/DaslangConversionHelpers.h"

// ---- Direct type bindings ----

DAS_TYPE_DECL(dn_Color, duin::Color);

DAS_TYPE_ANNOTATION(dn_Color)
{
    DAS_ADD_FIELD_BIND(r);
    DAS_ADD_FIELD_BIND(g);
    DAS_ADD_FIELD_BIND(b);
    DAS_ADD_FIELD_BIND(a);
}

// ---- Color factories ----

static duin::Color dn_make_color(float r, float g, float b, float a)
{
    return duin::Color(r, g, b, a);
}

static duin::Color dn_make_color_hex(uint32_t hex)
{
    return duin::Color(hex);
}

// ---- Render mode wrappers ----

static void dn_begin_draw3d_impl(const das::float3 &position, const das::float3 &target,
                                  const das::float3 &globalUp, float fovy)
{
    duin::Camera cam;
    cam.SetPosition(duin::from_f3(position));
    cam.SetTarget(duin::from_f3(target));
    cam.SetGlobalUp(duin::from_f3(globalUp));
    cam.SetFOVY(fovy);
    duin::BeginDraw3D(cam);
}

static void dn_begin_texture_mode_impl(void *rtPtr)
{
    if (!rtPtr)
        return;
    duin::BeginTextureMode(*static_cast<duin::RenderTexture *>(rtPtr));
}

// ---- RenderTexture management ----

static void *dn_create_render_texture_impl(int width, int height)
{
    return static_cast<void *>(new duin::RenderTexture(width, height, duin::UUID()));
}

static void dn_destroy_render_texture_impl(void *rtPtr)
{
    if (!rtPtr)
        return;
    auto *rt = static_cast<duin::RenderTexture *>(rtPtr);
    duin::DestroyRenderTexture(*rt);
    delete rt;
}

static int dn_render_texture_width_impl(void *rtPtr)
{
    if (!rtPtr)
        return 0;
    return static_cast<duin::RenderTexture *>(rtPtr)->width;
}

static int dn_render_texture_height_impl(void *rtPtr)
{
    if (!rtPtr)
        return 0;
    return static_cast<duin::RenderTexture *>(rtPtr)->height;
}

static int dn_render_texture_is_valid_impl(void *rtPtr)
{
    if (!rtPtr)
        return 0;
    return static_cast<duin::RenderTexture *>(rtPtr)->IsValid() ? 1 : 0;
}

// ---- Clear ----

static void dn_clear_background_impl(const duin::Color &color)
{
    duin::ClearBackground(color);
}

// ---- Draw primitives ----

static void dn_draw_box_impl(const das::float3 position, const das::float4 rotation, const das::float3 size)
{
    duin::DrawBox(duin::from_f3(position), duin::from_f4(rotation), duin::from_f3(size));
}

static void dn_draw_sphere_impl(const das::float3 position, const das::float4 rotation, const das::float3 size)
{
    duin::DrawSphere(duin::from_f3(position), duin::from_f4(rotation), duin::from_f3(size));
}

static void dn_draw_square_impl(const das::float3 position, const das::float4 rotation, const das::float3 size)
{
    duin::DrawSquare(duin::from_f3(position), duin::from_f4(rotation), duin::from_f3(size));
}

static void dn_draw_grid_impl(float size)
{
    duin::DrawGrid(size);
}

static void dn_draw_plane_impl(const das::float3 size)
{
    duin::DrawPlane(duin::from_f3(size));
}

static void dn_draw_debug_sphere_impl(const das::float3 position, float radius)
{
    duin::DrawDebugSphere(duin::from_f3(position), radius);
}

static void dn_draw_debug_capsule_impl(const das::float3 from, const das::float3 to, float radius)
{
    duin::DrawDebugCapsule(duin::from_f3(from), duin::from_f3(to), radius);
}

static void dn_draw_debug_box_impl(const das::float3 min, const das::float3 max)
{
    duin::DrawDebugBox(duin::from_f3(min), duin::from_f3(max));
}

// ---- Render queue ----

static void dn_queue_render_impl(int type)
{
    duin::QueueRender(static_cast<duin::RenderGeometryType::Type>(type));
}

static void dn_queue_render_full_impl(int type, const das::float3 &position, const das::float4 &rotation,
                                      const das::float3 &size)
{
    duin::QueueRender(static_cast<duin::RenderGeometryType::Type>(type), duin::from_f3(position),
                      duin::from_f4(rotation), duin::from_f3(size));
}

// ---- ImGui texture ----

static unsigned int dn_draw_imgui_texture_impl(void *rtPtr, float width, float height)
{
    if (!rtPtr)
        return 0;
    auto *rt = static_cast<duin::RenderTexture *>(rtPtr);
    return duin::DrawIMGUITexture(*rt, duin::Vector2{width, height});
}

// ---- Module ----

class Module_DnRenderer : public das::Module
{
    bool initialized = false;

  public:
    Module_DnRenderer() : das::Module("dn_renderer")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnRenderer: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnRenderer: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        auto *cameraMod = das::Module::require("dn_camera_core");
        if (!cameraMod)
        {
            DN_CORE_ERROR("Module_DnRenderer: required module 'dn_camera' not found");
            return false;
        }
        if (!cameraMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnRenderer: failed to initialize dependencies of 'dn_camera'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);
        addBuiltinDependency(lib, cameraMod);

        // Color type
        addAnnotation(make_smart<dn_ColorAnnotation>(lib));

        addExtern<DAS_BIND_FUN(dn_make_color), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_color", das::SideEffects::none, "dn_make_color")
            ->args({"r", "g", "b", "a"});
        addExtern<DAS_BIND_FUN(dn_make_color_hex), das::SimNode_ExtFuncCallAndCopyOrMove>(
            *this, lib, "dn_make_color_hex", das::SideEffects::none, "dn_make_color_hex")
            ->args({"hex"});

        // Renderer lifecycle
        addExtern<DAS_BIND_FUN(duin::InitRenderer)>(*this, lib, "dn_init_renderer",
                                                    das::SideEffects::modifyExternal, "duin::InitRenderer");
        addExtern<DAS_BIND_FUN(duin::CleanRenderer)>(*this, lib, "dn_clean_renderer",
                                                     das::SideEffects::modifyExternal, "duin::CleanRenderer");
        addExtern<DAS_BIND_FUN(duin::ResetRenderer)>(*this, lib, "dn_reset_renderer",
                                                     das::SideEffects::modifyExternal, "duin::ResetRenderer");
        addExtern<DAS_BIND_FUN(duin::IsRenderContextAvailable)>(*this, lib, "dn_is_render_context_available",
                                                                das::SideEffects::none,
                                                                "duin::IsRenderContextAvailable");
        addExtern<DAS_BIND_FUN(duin::SetRenderContextAvailable)>(
            *this, lib, "dn_set_render_context_available", das::SideEffects::modifyExternal,
            "duin::SetRenderContextAvailable")
            ->args({"available"});

        // Render modes
        addExtern<DAS_BIND_FUN(dn_begin_draw3d_impl)>(*this, lib, "dn_begin_draw3d",
                                                      das::SideEffects::modifyExternal, "dn_begin_draw3d_impl")
            ->args({"position", "target", "globalUp", "fovy"});
        addExtern<DAS_BIND_FUN(duin::EndDraw3D)>(*this, lib, "dn_end_draw3d", das::SideEffects::modifyExternal,
                                                 "duin::EndDraw3D");
        addExtern<DAS_BIND_FUN(dn_begin_texture_mode_impl)>(*this, lib, "dn_begin_texture_mode",
                                                            das::SideEffects::modifyExternal,
                                                            "dn_begin_texture_mode_impl")
            ->args({"target"});
        addExtern<DAS_BIND_FUN(duin::EndTextureMode)>(*this, lib, "dn_end_texture_mode",
                                                      das::SideEffects::modifyExternal, "duin::EndTextureMode");
        addExtern<DAS_BIND_FUN(duin::BeginDebugDraw)>(*this, lib, "dn_begin_debug_draw",
                                                      das::SideEffects::modifyExternal, "duin::BeginDebugDraw");
        addExtern<DAS_BIND_FUN(duin::EndDebugDraw)>(*this, lib, "dn_end_debug_draw",
                                                    das::SideEffects::modifyExternal, "duin::EndDebugDraw");

        // Background
        addExtern<DAS_BIND_FUN(dn_clear_background_impl)>(*this, lib, "dn_clear_background",
                                                          das::SideEffects::modifyExternal, "dn_clear_background_impl")
            ->args({"color"});

        // Draw primitives
        addExtern<DAS_BIND_FUN(dn_draw_box_impl)>(*this, lib, "dn_draw_box", das::SideEffects::modifyExternal,
                                                  "dn_draw_box_impl")
            ->args({"position", "rotation", "size"});
        addExtern<DAS_BIND_FUN(dn_draw_sphere_impl)>(*this, lib, "dn_draw_sphere", das::SideEffects::modifyExternal,
                                                     "dn_draw_sphere_impl")
            ->args({"position", "rotation", "size"});
        addExtern<DAS_BIND_FUN(dn_draw_square_impl)>(*this, lib, "dn_draw_square", das::SideEffects::modifyExternal,
                                                     "dn_draw_square_impl")
            ->args({"position", "rotation", "size"});
        addExtern<DAS_BIND_FUN(dn_draw_grid_impl)>(*this, lib, "dn_draw_grid", das::SideEffects::modifyExternal,
                                                   "dn_draw_grid_impl")
            ->args({"size"});
        addExtern<DAS_BIND_FUN(dn_draw_plane_impl)>(*this, lib, "dn_draw_plane", das::SideEffects::modifyExternal,
                                                    "dn_draw_plane_impl")
            ->args({"size"});
        addExtern<DAS_BIND_FUN(dn_draw_debug_sphere_impl)>(*this, lib, "dn_draw_debug_sphere",
                                                           das::SideEffects::modifyExternal,
                                                           "dn_draw_debug_sphere_impl")
            ->args({"position", "radius"});
        addExtern<DAS_BIND_FUN(dn_draw_debug_capsule_impl)>(*this, lib, "dn_draw_debug_capsule",
                                                            das::SideEffects::modifyExternal,
                                                            "dn_draw_debug_capsule_impl")
            ->args({"from", "to", "radius"});
        addExtern<DAS_BIND_FUN(dn_draw_debug_box_impl)>(*this, lib, "dn_draw_debug_box",
                                                        das::SideEffects::modifyExternal, "dn_draw_debug_box_impl")
            ->args({"min", "max"});

        // Render queue
        addExtern<DAS_BIND_FUN(dn_queue_render_impl)>(*this, lib, "dn_queue_render", das::SideEffects::modifyExternal,
                                                      "dn_queue_render_impl")
            ->args({"type"});
        addExtern<DAS_BIND_FUN(dn_queue_render_full_impl)>(*this, lib, "dn_queue_render_full",
                                                           das::SideEffects::modifyExternal, "dn_queue_render_full_impl")
            ->args({"type", "position", "rotation", "size"});
        addExtern<DAS_BIND_FUN(duin::ExecuteRenderPipeline)>(*this, lib, "dn_execute_render_pipeline",
                                                             das::SideEffects::modifyExternal,
                                                             "duin::ExecuteRenderPipeline");
        addExtern<DAS_BIND_FUN(duin::EmptyRenderStack)>(*this, lib, "dn_empty_render_stack",
                                                        das::SideEffects::modifyExternal, "duin::EmptyRenderStack");

        // RenderTexture management
        addExtern<DAS_BIND_FUN(dn_create_render_texture_impl)>(*this, lib, "dn_create_render_texture",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_create_render_texture_impl")
            ->args({"width", "height"});
        addExtern<DAS_BIND_FUN(dn_destroy_render_texture_impl)>(*this, lib, "dn_destroy_render_texture",
                                                                das::SideEffects::modifyExternal,
                                                                "dn_destroy_render_texture_impl")
            ->args({"renderTexture"});
        addExtern<DAS_BIND_FUN(dn_render_texture_width_impl)>(*this, lib, "dn_render_texture_width",
                                                              das::SideEffects::none, "dn_render_texture_width_impl")
            ->args({"renderTexture"});
        addExtern<DAS_BIND_FUN(dn_render_texture_height_impl)>(*this, lib, "dn_render_texture_height",
                                                               das::SideEffects::none, "dn_render_texture_height_impl")
            ->args({"renderTexture"});
        addExtern<DAS_BIND_FUN(dn_render_texture_is_valid_impl)>(*this, lib, "dn_render_texture_is_valid",
                                                                 das::SideEffects::none,
                                                                 "dn_render_texture_is_valid_impl")
            ->args({"renderTexture"});

        // ImGui texture
        addExtern<DAS_BIND_FUN(dn_draw_imgui_texture_impl)>(*this, lib, "dn_draw_imgui_texture",
                                                            das::SideEffects::modifyExternal,
                                                            "dn_draw_imgui_texture_impl")
            ->args({"renderTexture", "width", "height"});

        DN_CORE_INFO("Script Module [dn_renderer] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnRenderer);
