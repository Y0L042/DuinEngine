#pragma once

#include "daScript/daScript.h"

#include "Duin/Objects/GameStateMachine.h"

#include "../../DnGameStateAdapter_gen.inc"
#include "../../DnGameStateMachineAdapter_gen.inc"

#include "Duin/Core/Debug/DNLog.h"

// =========================================================================
// Bridge class: ScriptGameState (duin::GameState + daslang adapter)
// =========================================================================

class ScriptGameState : public duin::GameState, public DnGameStateAdapterBase
{
  public:
    ScriptGameState(duin::GameStateMachine &owner, char *pClass,
                    const das::StructInfo *info, das::Context *ctx)
        : duin::GameState(owner), DnGameStateAdapterBase(info),
          classPtr(pClass), context(ctx)
    {
        EnsureImpl();
    }

    void Enter() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__enter(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [enter] is mangled.");
            else
                invoke__enter(context, fn, classPtr);
        }
    }

    void Update(double delta) override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__update(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [update] is mangled.");
            else
                invoke__update(context, fn, classPtr, delta);
        }
    }

    void PhysicsUpdate(double delta) override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__physics_update(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            else
                invoke__physics_update(context, fn, classPtr, delta);
        }
    }

    void Draw() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__draw(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [draw] is mangled.");
            else
                invoke__draw(context, fn, classPtr);
        }
    }

    void DrawUI() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__draw_ui(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            else
                invoke__draw_ui(context, fn, classPtr);
        }
    }

    void Exit() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__exit(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [exit] is mangled.");
            else
                invoke__exit(context, fn, classPtr);
        }
    }

    void SetPause() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__set_pause(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [set_pause] is mangled.");
            else
                invoke__set_pause(context, fn, classPtr);
        }
    }

    void SetUnpause() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__set_unpause(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [set_unpause] is mangled.");
            else
                invoke__set_unpause(context, fn, classPtr);
        }
    }

    void *classPtr;
    das::Context *context;
};

// =========================================================================
// Bridge class: ScriptGameStateMachine (duin::GameStateMachine + daslang adapter)
// =========================================================================

class ScriptGameStateMachine : public duin::GameStateMachine,
                                public DnGameStateMachineAdapterBase
{
  public:
    ScriptGameStateMachine(char *pClass, const das::StructInfo *info, das::Context *ctx)
        : DnGameStateMachineAdapterBase(info), classPtr(pClass), context(ctx)
    {
        EnsureImpl();
    }

    void Init() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__init(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [init] is mangled.");
            else
                invoke__init(context, fn, classPtr);
        }
    }

    void Ready() override
    {
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__ready(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [ready] is mangled.");
            else
                invoke__ready(context, fn, classPtr);
        }
    }

    void Update(double delta) override
    {
        GameStateMachine::Update(delta);
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__update(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [update] is mangled.");
            else
                invoke__update(context, fn, classPtr, delta);
        }
    }

    void PhysicsUpdate(double delta) override
    {
        GameStateMachine::PhysicsUpdate(delta);
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__physics_update(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            else
                invoke__physics_update(context, fn, classPtr, delta);
        }
    }

    void Draw() override
    {
        GameStateMachine::Draw();
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__draw(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [draw] is mangled.");
            else
                invoke__draw(context, fn, classPtr);
        }
    }

    void DrawUI() override
    {
        GameStateMachine::DrawUI();
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__draw_ui(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            else
                invoke__draw_ui(context, fn, classPtr);
        }
    }

    void Debug() override
    {
        GameStateMachine::Debug();
        constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
        if (auto fn = get__debug(classPtr))
        {
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
                DN_CORE_FATAL("Function [debug] is mangled.");
            else
                invoke__debug(context, fn, classPtr);
        }
    }

    void *classPtr;
    das::Context *context;
};
