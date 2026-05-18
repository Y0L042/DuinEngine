#pragma once

#include "daScript/daScript.h"
#include "Duin/ECS/GameWorld.h"
#include "../../DnGameWorldAdapter_gen.inc"
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/Script/Script.h"

// =========================================================================
// Bridge: dual-inherits duin::GameWorld + generated daslang adapter.
// Created by dn_create_gameworld_impl when dn_gameworld_init is called.
// =========================================================================

class ScriptGameWorld : public duin::GameWorld, public DnGameWorldAdapterBase
{
  public:
    ScriptGameWorld(char *pClass, const das::StructInfo *info, das::Context *ctx)
        : DnGameWorldAdapterBase(info), classPtr(pClass), context(ctx)
    {
        Initialize(/*connectSignals=*/true);
    }

    void PostUpdateQueryExecution(double delta) override
    {
        duin::GameWorld::PostUpdateQueryExecution(delta);

        if (auto fn = get__post_update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [update] is mangled.");
            }
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__post_update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _post_update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void PostPhysicsUpdateQueryExecution(double delta) override
    {
        duin::GameWorld::PostPhysicsUpdateQueryExecution(delta);
        if (auto fn = get__post_physics_update(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            }
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__post_physics_update(context, fn, classPtr, delta); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _post_physics_update: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void PostDrawQueryExecution() override
    {
        duin::GameWorld::PostDrawQueryExecution();
        if (auto fn = get__post_draw(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [draw] is mangled.");
            }
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__post_draw(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _post_draw: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void PostDrawUIQueryExecution() override
    {
        duin::GameWorld::PostDrawUIQueryExecution();
        if (auto fn = get__post_draw_ui(classPtr))
        {
            if (!duin::VerifyFunction(context, fn))
            {
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            }
            else
            {
                bool ok = context->runWithCatch([&]() { invoke__post_draw_ui(context, fn, classPtr); });
                if (!ok)
                {
                    if (auto ex = context->getException())
                        DN_CORE_ERROR("[Script] Exception in _post_draw_ui: {}", ex);
                    context->clearException();
                }
            }
        }
    }

    void *classPtr;
    das::Context *context;
};
