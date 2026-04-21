#pragma once

#include "daScript/daScript.h"

#include "Duin/Objects/GameObject.h"

#include "../../DnGameObjectAdapter_gen.inc"

#include "Duin/Core/Debug/DNLog.h"

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
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [init] is mangled.");
            }
            else
            {
                invoke__init(context, fn, classPtr);
            }
        }
    }

    void Ready() override
    {
        if (auto fn = get__ready(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [ready] is mangled.");
            }
            else
            {
                invoke__ready(context, fn, classPtr);
            }
        }
    }

    void Update(double delta) override
    {
        if (auto fn = get__update(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [update] is mangled.");
            }
            else
            {
                invoke__update(context, fn, classPtr, delta);
            }
        }
    }

    void PhysicsUpdate(double delta) override
    {
        if (auto fn = get__physics_update(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [physics_update] is mangled.");
            }
            else
            {
                invoke__physics_update(context, fn, classPtr, delta);
            }
        }
    }

    void Draw() override
    {
        if (auto fn = get__draw(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [draw] is mangled.");
            }
            else
            {
                invoke__draw(context, fn, classPtr);
            }
        }
    }

    void DrawUI() override
    {
        if (auto fn = get__draw_ui(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [draw_ui] is mangled.");
            }
            else
            {
                invoke__draw_ui(context, fn, classPtr);
            }
        }
    }

    void Debug() override
    {
        if (auto fn = get__debug(classPtr))
        {
            constexpr uintptr_t kInvalidPtrPattern = 0xddddddddddddddddULL;
            if (fn.PTR == nullptr || reinterpret_cast<uintptr_t>(fn.PTR) == kInvalidPtrPattern)
            {
                DN_CORE_FATAL("Function [debug] is mangled.");
            }
            else
            {
                invoke__debug(context, fn, classPtr);
            }
        }
    }

    void *classPtr;
    das::Context *context;
  protected:
    friend class GameScript;
      
};
