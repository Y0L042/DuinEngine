#include "dnpch.h"
#include "GameScript.h"
#include "Duin/IO/FileModule.h"
#include "./modules/moduleGameObject/Module_DnGameObject.h"
#include "./modules/moduleLiveHost/Module_DnLiveHost.h"
#include <iostream>
#include <memory>
#include <regex>
#include <string>
#include <Duin/Core/Debug/DNLog.h>
#include <Duin/IO/Filesystem.h>
#include <Duin/IO/VirtualFilesystem.h>
#include <Duin/Objects/GameObject.h>
#include "Script.h"
#include <external/FileWatch.h>

duin::GameScript::GameScript(const std::string &relScriptPath) : Script(relScriptPath), GameObject()
{
}

duin::GameScript::~GameScript()
{
    ResetScript();
}

void duin::GameScript::SetGameFunctions()
{
    if (!context)
    {
        DN_CORE_FATAL("Context is invalid!");
        return;
    }

    auto bindFn = [&](das::Func &field, const char *name) -> das::SimFunction * {
        das::SimFunction *simFn = context->findFunction(name);
        field = das::Func(simFn);
        return simFn;
    };

    if (auto *sf = bindFn(fnGameReady, "game_ready"))
    {
        if (!das::verifyCall<void>(sf->debugInfo, libGroup))
        {
            DN_CORE_WARN("game_ready: signature mismatch — expected void()");
        }
    }

    if (auto *sf = bindFn(fnGameUpdate, "game_update"))
    {
        if (!das::verifyCall<void, double>(sf->debugInfo, libGroup))
        {
            DN_CORE_WARN("game_update: signature mismatch — expected void(double)");
        }
    }

    if (auto *sf = bindFn(fnGamePhysicsUpdate, "game_physics_update"))
    {
        if (!das::verifyCall<void, double>(sf->debugInfo, libGroup))
        {
            DN_CORE_WARN("game_physics_update: signature mismatch — expected void(double)");
        }
    }

    if (auto *sf = bindFn(fnGameDraw, "game_draw"))
    {
        if (!das::verifyCall<void>(sf->debugInfo, libGroup))
        {
            DN_CORE_WARN("game_draw: signature mismatch — expected void()");
        }
    }

    if (auto *sf = bindFn(fnGameDrawUI, "game_draw_ui"))
    {
        if (!das::verifyCall<void>(sf->debugInfo, libGroup))
        {
            DN_CORE_WARN("game_draw_ui: signature mismatch — expected void()");
        }
    }
}

void duin::GameScript::ResetGameFunctions()
{
    fnGameReady = das::Func{};
    fnGameUpdate = das::Func{};
    fnGamePhysicsUpdate = das::Func{};
    fnGameDraw = das::Func{};
    fnGameDrawUI = das::Func{};
}

void duin::GameScript::ResetMuteWarningFlags()
{
    muteReadyWarning = false;
    muteUpdateWarning = false;
    mutePhysicsUpdateWarning = false;
    muteDrawWarning = false;
    muteDrawUIWarning = false;
}

void duin::GameScript::EnableHotCompile(bool enable, bool halt)
{
    hotCompileEnabled = enable;
    haltOnCompilationFail = halt;
}

bool duin::GameScript::IsHotCompileEnabled()
{
    return hotCompileEnabled;
}

bool duin::GameScript::CompileAndSimulate()
{
    bool isReload = hasCompiledOnce;

    ResetMuteWarningFlags();
    ResetToBaseModules();

    bool success = Compile();
    if (success)
    {
        std::shared_ptr<ScriptContext> oldContext = context; // Cache Context
        context = nullptr;

        success = SimulateContext();
        if (success)
        {
            ClearScriptGameObjects();

            SetGameFunctions();

            hasCompiledOnce = true;
            Ready();
        }
        else
        {
            context = oldContext; // Restore old program if Simulation fails, else let context release
        }
    }
    else
    {
        if (haltOnCompilationFail)
        {
            scriptReady = false;
        }
    }

    return success;
}

bool duin::GameScript::SetContextRootObject()
{
    bool res = IsValid() && context;
    if (res)
    {
        context->rootGameObject = this;
    }

    return res;
}

void duin::GameScript::ResetScript()
{
    ResetGameFunctions();
    Script::ResetScript();
}

void duin::GameScript::Init()
{
    std::string path = duin::fs::MapVirtualToSystemPath("bin://");
    std::wregex wrgx(L".*\\.das$");
    std::wstring wpath(path.begin(), path.end());
    directoryWatch = std::make_unique<filewatch::FileWatch<std::wstring>>(
        wpath, wrgx, [this](const std::wstring &path, const filewatch::Event change_type) {
            // queueHotCompileFlag = true;
            hotCompileFileChangeCooldownTimer = HOT_COMPILE_FILE_CHANGE_COOLDOWN;
        });
}

void duin::GameScript::Ready()
{
    if (!SetContextRootObject())
    {
        DN_CORE_WARN("GameScript has not been added to App root, will not run callbacks!");
    }

    if (hotCompileEnabled)
    {
        duin::fs::PathInfo pInfo;
        if (duin::vfs::GetPathInfo("bin://" + scriptPath, &pInfo))
        {
            scriptLastModified = pInfo.modifyTime;
        }
    }

    if (scriptReady)
    {
        InvokeVoid(fnGameReady);
    }
    else
    {
        if (!muteReadyWarning)
        {
            DN_CORE_WARN("(Ready) Script not ready! {}", scriptPath);
            muteReadyWarning = true;
        }
    }
}

void duin::GameScript::OnEvent(Event e)
{
}


void duin::GameScript::Update(double delta)
{
    uptimeAccum += static_cast<float>(delta);

    // Only hot recompile if it has been X seconds since last file change (with no further changes).
    // Timer then disabled to prevent continual recompilation, only reset when file is changed again.
    float cooldownTimerPrev = hotCompileFileChangeCooldownTimer;
    if (hotCompileFileChangeCooldownTimer > 0.0f)
    {
        hotCompileFileChangeCooldownTimer -= static_cast<float>(delta);
    }
    bool cooldownReachedZero =
        (hotCompileFileChangeCooldownTimer <= 0.0f) && ((hotCompileFileChangeCooldownTimer * cooldownTimerPrev) < 0.0f);
    if (cooldownReachedZero)
    {
        queueHotCompileFlag = true;
        hotCompileFileChangeCooldownTimer = -999.999f;
    }

    if (queueHotCompileFlag)
    {
        queueHotCompileFlag = false;
        if (hotCompileEnabled)
        {
            duin::fs::PathInfo pInfo;
            if (duin::vfs::GetPathInfo("bin://" + scriptPath, &pInfo))
            {
                scriptLastModified = pInfo.modifyTime;
            }
        }
        if (!CompileAndSimulate())
        {
            DN_CORE_WARN("Compilation of {} failed! Continuing with previous version.", "bin://" + scriptPath);
        }
    }

    if (scriptReady)
    {
        InvokeWithDelta(fnGameUpdate, delta);
    }
    else
    {
        if (!muteUpdateWarning)
        {
            DN_CORE_WARN("(Update) Script not ready! {}", scriptPath);
            muteUpdateWarning = true;
        }
    }
}

void duin::GameScript::PhysicsUpdate(double delta)
{
    if (scriptReady)
    {
        InvokeWithDelta(fnGamePhysicsUpdate, delta);
    }
    else
    {
        if (!mutePhysicsUpdateWarning)
        {
            DN_CORE_WARN("(PhysicsUpdate) Script not ready! {}", scriptPath);
            mutePhysicsUpdateWarning = true;
        }
    }
}

void duin::GameScript::Draw()
{
    if (scriptReady)
    {
        InvokeVoid(fnGameDraw);
    }
    else
    {
        if (!muteDrawWarning)
        {
            DN_CORE_WARN("(Draw) Script not ready! {}", scriptPath);
            muteDrawWarning = true;
        }
    }
}

void duin::GameScript::DrawUI()
{
    if (scriptReady)
    {
        InvokeVoid(fnGameDrawUI);
    }
    else
    {
        if (!muteDrawUIWarning)
        {
            DN_CORE_WARN("(DrawUI) Script not ready! {}", scriptPath);
            muteDrawUIWarning = true;
        }
    }
}

void duin::GameScript::SetHotCompileFileChangeCooldown(float val)
{
    HOT_COMPILE_FILE_CHANGE_COOLDOWN = val;
}

void duin::GameScript::ClearScriptGameObjects()
{
    for (auto &child : GetChildren())
    {
        RemoveChildObject(child.lock());
    }
}

void duin::GameScript::RestartSGORecurse(std::shared_ptr<GameObject> obj)
{
    auto scriptGameObj = std::static_pointer_cast<ScriptGameObject>(obj);
    scriptGameObj->context = context.get();
    // scriptGameObj->classPtr = ??

    for (auto &child : obj->GetChildren())
    {
        if (child.lock())
        {
            RestartSGORecurse(child.lock());
        }
    }
}
