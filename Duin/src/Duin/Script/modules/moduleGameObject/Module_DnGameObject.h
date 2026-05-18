#pragma once

#include "daScript/daScript.h"

#include "Duin/Objects/GameObject.h"

#include "../../DnGameObjectAdapter_gen.inc"

#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/Script.h"

// =========================================================================
// Layer 3: Dual-inheritance bridge (C++ GameObject + daslang adapter)
// =========================================================================

class ScriptGameObject : public duin::GameObject, public DnGameObjectAdapterBase
{
  public:
    ScriptGameObject(char *pClass, const das::StructInfo *info, das::Context *ctx)
        : DnGameObjectAdapterBase(info), classPtr(pClass), context(ctx)
    {
        DN_CORE_INFO("EnsureImpl()");
        EnsureImpl();
    }

    void Init() override
    {
        if (auto fn = get__init(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [init] is mangled.");
            }
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
            {
                DN_CORE_FATAL("Function [ready] is mangled.");
            }
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
        if (auto fn = get__update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [update] is mangled.");
            }
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
            {
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            }
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
            {
                DN_CORE_FATAL("Function [draw] is mangled.");
            }
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
            {
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            }
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
        if (auto fn = get__debug(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [debug] is mangled.");
            }
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
  protected:
    friend class GameScript;
      
};
