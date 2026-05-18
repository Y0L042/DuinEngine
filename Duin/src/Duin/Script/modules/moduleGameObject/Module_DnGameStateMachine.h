#pragma once

#include "daScript/daScript.h"

#include "Duin/Objects/GameStateMachine.h"

#include "../../DnGameStateAdapter_gen.inc"
#include "../../DnGameStateMachineAdapter_gen.inc"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/Script.h"

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
        if (auto fn = get__enter(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [enter] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__enter(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _enter: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Update(double delta) override
    {
        if (auto fn = get__update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [update] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void PhysicsUpdate(double delta) override
    {
        if (auto fn = get__physics_update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__physics_update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _physics_update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Draw() override
    {
        if (auto fn = get__draw(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [draw] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__draw(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _draw: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void DrawUI() override
    {
        if (auto fn = get__draw_ui(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__draw_ui(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _draw_ui: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Exit() override
    {
        if (auto fn = get__exit(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [exit] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__exit(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _exit: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void SetPause() override
    {
        if (auto fn = get__set_pause(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [set_pause] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__set_pause(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _set_pause: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void SetUnpause() override
    {
        if (auto fn = get__set_unpause(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [set_unpause] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__set_unpause(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _set_unpause: {}", ex);
                    context->clearException();
                }
            }
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
        if (auto fn = get__init(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [init] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__init(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _init: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Ready() override
    {
        if (auto fn = get__ready(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [ready] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__ready(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _ready: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Update(double delta) override
    {
        GameStateMachine::Update(delta);
        if (auto fn = get__update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [update] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void PhysicsUpdate(double delta) override
    {
        GameStateMachine::PhysicsUpdate(delta);
        if (auto fn = get__physics_update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__physics_update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _physics_update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Draw() override
    {
        GameStateMachine::Draw();
        if (auto fn = get__draw(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [draw] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__draw(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _draw: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void DrawUI() override
    {
        GameStateMachine::DrawUI();
        if (auto fn = get__draw_ui(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__draw_ui(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _draw_ui: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void Debug() override
    {
        GameStateMachine::Debug();
        if (auto fn = get__debug(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
                DN_CORE_FATAL("Function [debug] is mangled.");
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__debug(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _debug: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void *classPtr;
    das::Context *context;
};
