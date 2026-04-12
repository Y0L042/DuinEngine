#include "Module_DnGameObject.h"
#include <daScript/ast/ast.h>
#include <daScript/simulate/simulate.h>
#include "module_builtin_rtti.h"
#include "Duin/Script/Script.h"
#include "Duin/Objects/GameObject.h"

// Creates a C++ ScriptGameObject backed by the daslang class instance.
// Returns a void* handle to a heap-allocated shared_ptr.
static void *dn_create_gameobject_impl(void *classPtr, const das::StructInfo *info, das::Context *context)
{
    auto obj = duin::GameObject::Create<ScriptGameObject>((char *)classPtr, info, context);
    DN_CORE_INFO("dn_create_gameobject_impl: created ScriptGameObject for '{}'", info->name);

    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    void *handle = static_cast<void *>(dnCtx->scriptMemory->Add(obj));

    return handle;
}

static void dn_add_to_tree(void *handle)
{
    // if (handle)
    //{
    //     duin::Application::AddChildObject(handle);
    // }
}

// Destroys the C++ ScriptGameObject handle.
static void dn_destroy_gameobject_impl(void *handle, das::Context *context)
{
    if (handle)
    {
        DN_CORE_INFO("dn_destroy_gameobject_impl: destroying ScriptGameObject");
        duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
        dnCtx->scriptMemory->Remove(handle);
    }
}

// Removes a child object from a parent. Both are passed as handles.
static void dn_remove_child_object_impl(void *selfHandle, void *childHandle, das::Context *context)
{
    if (!selfHandle || !childHandle)
        return;
    auto *parent = static_cast<ScriptGameObject *>(selfHandle);
    auto *child = static_cast<ScriptGameObject *>(childHandle);

    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;

    parentImpl->RemoveChild(childImpl);
    child->ResetParent();
}

// Returns the number of children.
static int dn_get_children_count_impl(void *handle)
{
    if (!handle)
        return 0;
    auto *obj = static_cast<ScriptGameObject *>(handle);
    return (int)obj->GetChildrenCount();
}

// Enable/disable callbacks
static void dn_enable_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->Enable(enable);
}
static void dn_enable_on_event_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableOnEvent(enable);
}
static void dn_enable_update_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableUpdate(enable);
}
static void dn_enable_physics_update_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnablePhysicsUpdate(enable);
}
static void dn_enable_draw_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableDraw(enable);
}
static void dn_enable_draw_ui_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableDrawUI(enable);
}
static void dn_enable_debug_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableDebug(enable);
}
static void dn_enable_children_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameObject *>(handle)->EnableChildren(enable);
}

// Query callback state
static bool dn_is_on_event_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsOnEventEnabled();
}
static bool dn_is_update_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsUpdateEnabled();
}
static bool dn_is_physics_update_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsPhysicsUpdateEnabled();
}
static bool dn_is_draw_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsDrawEnabled();
}
static bool dn_is_draw_ui_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsDrawUIEnabled();
}
static bool dn_is_debug_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsDebugEnabled();
}
static bool dn_is_children_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameObject *>(handle)->IsChildrenEnabled();
}

// Adds a child object to a parent. Both are passed as handles.
// Bypasses AddChildObject (which calls Init()) to avoid re-entering the daslang context.
// Script objects manage their own lifecycle via dn_gameobject_init.
static void dn_add_child_object_impl(void *selfHandle, void *childHandle, das::Context *context)
{
    if (!selfHandle || !childHandle)
        return;
    auto *parent = static_cast<ScriptGameObject *>(selfHandle);
    auto *child = static_cast<ScriptGameObject *>(childHandle);

    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;

    // Check not already a child
    for (auto &existing : parentImpl->childImpls)
        if (existing->uuid == childImpl->uuid)
            return;

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    auto childOwner = dnCtx->scriptMemory->Get<duin::GameObject>(childHandle);
    if (!childOwner)
        return;

    parentImpl->AddChild(childImpl, childOwner);
    childImpl->parentImpl = parentImpl.get();
    child->SetParent(parent);
}

static void dn_add_root_object_impl(void *childHandle, das::Context *context)
{
    if (!childHandle)
        return;

    auto *dnCtx = static_cast<duin::ScriptContext *>(context);

    if (!(dnCtx && dnCtx->rootGameObject))
        return;

    auto *parent = static_cast<duin::GameObject *>(dnCtx->rootGameObject);
    auto *child = static_cast<ScriptGameObject *>(childHandle);

    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;

    // Check not already a child
    for (auto &existing : parentImpl->childImpls)
        if (existing->uuid == childImpl->uuid)
            return;

    auto childOwner = dnCtx->scriptMemory->Get<duin::GameObject>(childHandle);
    if (!childOwner)
        return;

    parentImpl->AddChild(childImpl, childOwner);
    childImpl->parentImpl = parentImpl.get();
    child->SetParent(parent);
}

class Module_DnGameObject : public das::Module
{
    bool initialized = false;

  public:
    Module_DnGameObject() : das::Module("dn_gameobject_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
        {
            return true;
        }

        auto *rttiMod = das::Module::require("rtti_core");
        if (!rttiMod)
        {
            DN_CORE_ERROR("Module_DnGameObject: required module 'rtti_core' not found");
            return false;
        }
        if (!rttiMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnGameObject: failed to initialize dependencies of 'rtti_core'");
            return false;
        }

        auto logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnGameObject: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR("Module_DnGameObject: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);
        addBuiltinDependency(lib, rttiMod);

        addExtern<DAS_BIND_FUN(dn_create_gameobject_impl)>(
            *this, lib, "dn_create_gameobject_impl", das::SideEffects::modifyExternal, "dn_create_gameobject_impl");

        addExtern<DAS_BIND_FUN(dn_destroy_gameobject_impl)>(
            *this, lib, "dn_destroy_gameobject_impl", das::SideEffects::modifyExternal, "dn_destroy_gameobject_impl");

        addExtern<DAS_BIND_FUN(dn_add_child_object_impl)>(*this, lib, "dn_add_child_object_impl",
                                                          das::SideEffects::modifyExternal, "dn_add_child_object_impl");

        addExtern<DAS_BIND_FUN(dn_add_root_object_impl)>(*this, lib, "dn_add_root_object_impl",
                                                         das::SideEffects::modifyExternal, "dn_add_root_object_impl");

        addExtern<DAS_BIND_FUN(dn_remove_child_object_impl)>(
            *this, lib, "dn_remove_child_object_impl", das::SideEffects::modifyExternal, "dn_remove_child_object_impl");

        addExtern<DAS_BIND_FUN(dn_get_children_count_impl)>(*this, lib, "dn_get_children_count_impl",
                                                            das::SideEffects::none, "dn_get_children_count_impl");

        addExtern<DAS_BIND_FUN(dn_enable_impl)>(*this, lib, "dn_enable_impl", das::SideEffects::modifyExternal,
                                                "dn_enable_impl");
        addExtern<DAS_BIND_FUN(dn_enable_on_event_impl)>(*this, lib, "dn_enable_on_event_impl",
                                                         das::SideEffects::modifyExternal, "dn_enable_on_event_impl");
        addExtern<DAS_BIND_FUN(dn_enable_update_impl)>(*this, lib, "dn_enable_update_impl",
                                                       das::SideEffects::modifyExternal, "dn_enable_update_impl");
        addExtern<DAS_BIND_FUN(dn_enable_physics_update_impl)>(*this, lib, "dn_enable_physics_update_impl",
                                                               das::SideEffects::modifyExternal,
                                                               "dn_enable_physics_update_impl");
        addExtern<DAS_BIND_FUN(dn_enable_draw_impl)>(*this, lib, "dn_enable_draw_impl",
                                                     das::SideEffects::modifyExternal, "dn_enable_draw_impl");
        addExtern<DAS_BIND_FUN(dn_enable_draw_ui_impl)>(*this, lib, "dn_enable_draw_ui_impl",
                                                        das::SideEffects::modifyExternal, "dn_enable_draw_ui_impl");
        addExtern<DAS_BIND_FUN(dn_enable_debug_impl)>(*this, lib, "dn_enable_debug_impl",
                                                      das::SideEffects::modifyExternal, "dn_enable_debug_impl");
        addExtern<DAS_BIND_FUN(dn_enable_children_impl)>(*this, lib, "dn_enable_children_impl",
                                                         das::SideEffects::modifyExternal, "dn_enable_children_impl");

        addExtern<DAS_BIND_FUN(dn_is_on_event_enabled_impl)>(*this, lib, "dn_is_on_event_enabled_impl",
                                                             das::SideEffects::none, "dn_is_on_event_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_update_enabled_impl)>(*this, lib, "dn_is_update_enabled_impl",
                                                           das::SideEffects::none, "dn_is_update_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_physics_update_enabled_impl)>(*this, lib, "dn_is_physics_update_enabled_impl",
                                                                   das::SideEffects::none,
                                                                   "dn_is_physics_update_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_draw_enabled_impl)>(*this, lib, "dn_is_draw_enabled_impl", das::SideEffects::none,
                                                         "dn_is_draw_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_draw_ui_enabled_impl)>(*this, lib, "dn_is_draw_ui_enabled_impl",
                                                            das::SideEffects::none, "dn_is_draw_ui_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_debug_enabled_impl)>(*this, lib, "dn_is_debug_enabled_impl",
                                                          das::SideEffects::none, "dn_is_debug_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_is_children_enabled_impl)>(*this, lib, "dn_is_children_enabled_impl",
                                                             das::SideEffects::none, "dn_is_children_enabled_impl");

        DN_CORE_INFO("Script Module [dn_gameobject_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnGameObject);
