#include "dnpch.h"
#include "Module_DnGameStateMachine.h"
#include <daScript/ast/ast.h>
#include <daScript/simulate/simulate.h>
#include "module_builtin_rtti.h"
#include "Duin/Script/Script.h"
#include "Duin/Objects/GameStateMachine.h"
#include "Duin/Objects/GameObjectImpl.h"

// =========================================================================
// ScriptGameStateMachine helpers
// =========================================================================

static void *dn_create_gamestatemachine_impl(void *classPtr, const das::StructInfo *info,
                                             das::Context *context)
{
    auto obj = duin::GameObject::Create<ScriptGameStateMachine>((char *)classPtr, info, context);
    DN_CORE_INFO("dn_create_gamestatemachine_impl: created ScriptGameStateMachine for '{}'",
                 info->name);
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
}

static void dn_destroy_gamestatemachine_impl(void *handle, das::Context *context)
{
    if (handle)
    {
        DN_CORE_INFO("dn_destroy_gamestatemachine_impl: destroying ScriptGameStateMachine");
        duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
        dnCtx->scriptMemory->Remove(handle);
    }
}

static int dn_gsm_get_children_count_impl(void *handle)
{
    if (!handle)
        return 0;
    return (int)static_cast<ScriptGameStateMachine *>(handle)->GetChildrenCount();
}

static void dn_gsm_enable_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->Enable(enable);
}

static void dn_gsm_enable_on_event_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableOnEvent(enable);
}

static void dn_gsm_enable_update_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableUpdate(enable);
}

static void dn_gsm_enable_physics_update_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnablePhysicsUpdate(enable);
}

static void dn_gsm_enable_draw_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableDraw(enable);
}

static void dn_gsm_enable_draw_ui_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableDrawUI(enable);
}

static void dn_gsm_enable_debug_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableDebug(enable);
}

static void dn_gsm_enable_children_impl(void *handle, bool enable)
{
    if (handle)
        static_cast<ScriptGameStateMachine *>(handle)->EnableChildren(enable);
}

static bool dn_gsm_is_on_event_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsOnEventEnabled();
}

static bool dn_gsm_is_update_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsUpdateEnabled();
}

static bool dn_gsm_is_physics_update_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsPhysicsUpdateEnabled();
}

static bool dn_gsm_is_draw_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsDrawEnabled();
}

static bool dn_gsm_is_draw_ui_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsDrawUIEnabled();
}

static bool dn_gsm_is_debug_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsDebugEnabled();
}

static bool dn_gsm_is_children_enabled_impl(void *handle)
{
    return handle && static_cast<ScriptGameStateMachine *>(handle)->IsChildrenEnabled();
}

// =========================================================================
// ScriptGameState helpers
// =========================================================================

static void *dn_create_gamestate_impl(void *smHandle, void *classPtr,
                                      const das::StructInfo *info, das::Context *context)
{
    if (!smHandle)
        return nullptr;
    auto *sm = static_cast<ScriptGameStateMachine *>(smHandle);
    //auto obj = duin::GameObject::Create<ScriptGameState>(*sm, (char *)classPtr, info, context);
    auto obj = sm->CreateState<ScriptGameState>((char *)classPtr, info, context);
    DN_CORE_INFO("dn_create_gamestate_impl: created ScriptGameState for '{}'", info->name);
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    return static_cast<void *>(dnCtx->scriptMemory->Add(obj));
    //return static_cast<void *>(obj.get());  // the StateMachine manages State lifetime        
}

static void dn_destroy_gamestate_impl(void *handle, das::Context *context)
{
    if (handle)
    {
        DN_CORE_INFO("dn_destroy_gamestate_impl: destroying ScriptGameState");
        duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
        dnCtx->scriptMemory->Remove(handle);
    }
}

// =========================================================================
// State stack operations
// =========================================================================

static void dn_gsm_switch_state_impl(void *smHandle, void *stateHandle, das::Context *context)
{
    if (!smHandle)
        return;
    auto *sm = static_cast<ScriptGameStateMachine *>(smHandle);
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    auto statePtr = dnCtx->scriptMemory->Get<duin::GameState>(stateHandle);
    if (!statePtr)
        return;
    sm->SwitchState<duin::GameState>(statePtr);
}

static void dn_gsm_push_state_impl(void *smHandle, void *stateHandle, das::Context *context)
{
    if (!smHandle)
        return;
    auto *sm = static_cast<ScriptGameStateMachine *>(smHandle);
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    auto statePtr = dnCtx->scriptMemory->Get<duin::GameState>(stateHandle);
    if (!statePtr)
        return;
    sm->PushState<duin::GameState>(statePtr);
}

static void dn_gsm_flush_and_switch_state_impl(void *smHandle, void *stateHandle,
                                               das::Context *context)
{
    if (!smHandle)
        return;
    auto *sm = static_cast<ScriptGameStateMachine *>(smHandle);
    duin::ScriptContext *dnCtx = static_cast<duin::ScriptContext *>(context);
    auto statePtr = dnCtx->scriptMemory->Get<duin::GameState>(stateHandle);
    if (!statePtr)
        return;
    sm->FlushAndSwitchState<duin::GameState>(statePtr);
}

static void dn_gsm_pop_state_impl(void *smHandle)
{
    if (smHandle)
        static_cast<ScriptGameStateMachine *>(smHandle)->PopState();
}

static void dn_gsm_flush_stack_impl(void *smHandle)
{
    if (smHandle)
        static_cast<ScriptGameStateMachine *>(smHandle)->FlushStack();
}

static void dn_gsm_add_child_impl(void *selfHandle, void *childHandle, das::Context *context)
{
    if (!childHandle)
        return;
    auto *dnCtx = static_cast<duin::ScriptContext *>(context);
    if (!selfHandle)
    {
        if (!(dnCtx && dnCtx->rootGameObject))
            return;
        selfHandle = dnCtx->rootGameObject;
    }
    auto *parent = static_cast<duin::GameObject *>(selfHandle);
    auto *child = static_cast<duin::GameObject *>(childHandle);
    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;
    for (auto &existing : parentImpl->childImpls)
        if (existing->uuid == childImpl->uuid)
            return;
    auto childOwner = dnCtx->scriptMemory->Get<duin::GameObject>(childHandle);
    if (!childOwner)
        return;
    parentImpl->AddChild(childImpl, childOwner);
    childImpl->parentImpl = parentImpl.get();
    child->SetParent(parent);
    if (!child->IsInitialized())
        child->_InitializeImpl();
    if (parent->IsReady() && !child->IsReady())
        child->Ready();
}

static void dn_gsm_remove_child_impl(void *selfHandle, void *childHandle, das::Context *context)
{
    if (!selfHandle || !childHandle)
        return;
    auto *parent = static_cast<duin::GameObject *>(selfHandle);
    auto *child = static_cast<duin::GameObject *>(childHandle);
    auto parentImpl = parent->GetImpl();
    auto childImpl = child->GetImpl();
    if (!parentImpl || !childImpl)
        return;
    parentImpl->RemoveChild(childImpl);
    child->ResetParent();
}

// =========================================================================
// Module
// =========================================================================

class Module_DnGameStateMachine : public das::Module
{
    bool initialized = false;

  public:
    Module_DnGameStateMachine() : das::Module("dn_gamestatemachine_core")
    {
    }

    bool initDependencies() override
    {
        if (initialized)
            return true;

        auto *rttiMod = das::Module::require("rtti_core");
        if (!rttiMod)
        {
            DN_CORE_ERROR("Module_DnGameStateMachine: required module 'rtti_core' not found");
            return false;
        }
        if (!rttiMod->initDependencies())
        {
            DN_CORE_ERROR(
                "Module_DnGameStateMachine: failed to initialize dependencies of 'rtti_core'");
            return false;
        }

        auto *logMod = das::Module::require("dn_log");
        if (!logMod)
        {
            DN_CORE_ERROR("Module_DnGameStateMachine: required module 'dn_log' not found");
            return false;
        }
        if (!logMod->initDependencies())
        {
            DN_CORE_ERROR(
                "Module_DnGameStateMachine: failed to initialize dependencies of 'dn_log'");
            return false;
        }

        initialized = true;

        das::ModuleLibrary lib(this);
        lib.addBuiltInModule();
        addBuiltinDependency(lib, logMod);
        addBuiltinDependency(lib, rttiMod);

        // GameStateMachine create/destroy
        addExtern<DAS_BIND_FUN(dn_create_gamestatemachine_impl)>(
            *this, lib, "dn_create_gamestatemachine_impl", das::SideEffects::modifyExternal,
            "dn_create_gamestatemachine_impl");
        addExtern<DAS_BIND_FUN(dn_destroy_gamestatemachine_impl)>(
            *this, lib, "dn_destroy_gamestatemachine_impl", das::SideEffects::modifyExternal,
            "dn_destroy_gamestatemachine_impl");

        // GameState create/destroy
        addExtern<DAS_BIND_FUN(dn_create_gamestate_impl)>(
            *this, lib, "dn_create_gamestate_impl", das::SideEffects::modifyExternal,
            "dn_create_gamestate_impl");
        addExtern<DAS_BIND_FUN(dn_destroy_gamestate_impl)>(
            *this, lib, "dn_destroy_gamestate_impl", das::SideEffects::modifyExternal,
            "dn_destroy_gamestate_impl");

        // State stack operations
        addExtern<DAS_BIND_FUN(dn_gsm_switch_state_impl)>(
            *this, lib, "dn_gsm_switch_state_impl", das::SideEffects::modifyExternal,
            "dn_gsm_switch_state_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_push_state_impl)>(
            *this, lib, "dn_gsm_push_state_impl", das::SideEffects::modifyExternal,
            "dn_gsm_push_state_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_flush_and_switch_state_impl)>(
            *this, lib, "dn_gsm_flush_and_switch_state_impl", das::SideEffects::modifyExternal,
            "dn_gsm_flush_and_switch_state_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_pop_state_impl)>(
            *this, lib, "dn_gsm_pop_state_impl", das::SideEffects::modifyExternal,
            "dn_gsm_pop_state_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_flush_stack_impl)>(
            *this, lib, "dn_gsm_flush_stack_impl", das::SideEffects::modifyExternal,
            "dn_gsm_flush_stack_impl");

        addExtern<DAS_BIND_FUN(dn_gsm_add_child_impl)>(
            *this, lib, "dn_gsm_add_child_impl", das::SideEffects::modifyExternal,
            "dn_gsm_add_child_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_remove_child_impl)>(
            *this, lib, "dn_gsm_remove_child_impl", das::SideEffects::modifyExternal,
            "dn_gsm_remove_child_impl");

        addExtern<DAS_BIND_FUN(dn_gsm_get_children_count_impl)>(
            *this, lib, "dn_gsm_get_children_count_impl", das::SideEffects::none,
            "dn_gsm_get_children_count_impl");

        // GSM enable/disable
        addExtern<DAS_BIND_FUN(dn_gsm_enable_impl)>(*this, lib, "dn_gsm_enable_impl",
                                                     das::SideEffects::modifyExternal,
                                                     "dn_gsm_enable_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_on_event_impl)>(
            *this, lib, "dn_gsm_enable_on_event_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_on_event_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_update_impl)>(
            *this, lib, "dn_gsm_enable_update_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_update_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_physics_update_impl)>(
            *this, lib, "dn_gsm_enable_physics_update_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_physics_update_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_draw_impl)>(
            *this, lib, "dn_gsm_enable_draw_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_draw_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_draw_ui_impl)>(
            *this, lib, "dn_gsm_enable_draw_ui_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_draw_ui_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_debug_impl)>(
            *this, lib, "dn_gsm_enable_debug_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_debug_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_enable_children_impl)>(
            *this, lib, "dn_gsm_enable_children_impl", das::SideEffects::modifyExternal,
            "dn_gsm_enable_children_impl");

        // GSM enable queries
        addExtern<DAS_BIND_FUN(dn_gsm_is_on_event_enabled_impl)>(
            *this, lib, "dn_gsm_is_on_event_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_on_event_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_update_enabled_impl)>(
            *this, lib, "dn_gsm_is_update_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_update_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_physics_update_enabled_impl)>(
            *this, lib, "dn_gsm_is_physics_update_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_physics_update_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_draw_enabled_impl)>(
            *this, lib, "dn_gsm_is_draw_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_draw_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_draw_ui_enabled_impl)>(
            *this, lib, "dn_gsm_is_draw_ui_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_draw_ui_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_debug_enabled_impl)>(
            *this, lib, "dn_gsm_is_debug_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_debug_enabled_impl");
        addExtern<DAS_BIND_FUN(dn_gsm_is_children_enabled_impl)>(
            *this, lib, "dn_gsm_is_children_enabled_impl", das::SideEffects::none,
            "dn_gsm_is_children_enabled_impl");

        DN_CORE_INFO("Script Module [dn_gamestatemachine_core] initialized.");
        return true;
    }
};

REGISTER_MODULE(Module_DnGameStateMachine);
