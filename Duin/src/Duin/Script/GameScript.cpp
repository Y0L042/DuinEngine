#include "dnpch.h"
#include "GameScript.h"
#include "Duin/ECS/GameWorld.h"
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
#include <vecmath/dag_vecMathDecl.h>

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

    fnGameReady = context->findFunction("game_ready");
    fnGameUpdate = context->findFunction("game_update");
    fnGamePhysicsUpdate = context->findFunction("game_physics_update");
    fnGameDraw = context->findFunction("game_draw");
    fnGameDrawUI = context->findFunction("game_draw_ui");
}

void duin::GameScript::ResetGameFunctions()
{
    fnGameReady = nullptr;
    fnGameUpdate = nullptr;
    fnGamePhysicsUpdate = nullptr;
    fnGameDraw = nullptr;
    fnGameDrawUI = nullptr;
}

void duin::GameScript::SetGameWorld(GameWorld *gw)
{
    gameWorld_ = gw;
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
    g_DnLiveHostState.live_mode = enable;
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

    bool compiled = Compile();
    if (compiled)
    {
        // Call pre-Reload hooks of old context
        if (isReload && scriptReady)
        {
            g_DnLiveHostState.is_reload = true;
            for (auto *fn : g_DnLiveHostState.beforeReloadFns)
            {
                CallScript(fn);
            }
            CallLiveVarsFunctions("__before_reload_live_vars");
        }

        g_DnLiveHostState.beforeReloadFns.clear();
        g_DnLiveHostState.afterReloadFns.clear();

        std::shared_ptr<ScriptMemory> oldContextMemory;
        if (context)
        {
            oldContextMemory = context->scriptMemory;
        }

        bool simulated = SimulateContext();
        if (simulated)
        {
            if (oldContextMemory)
            {
                context->scriptMemory = oldContextMemory;
            }

            SetGameFunctions();
            SetContextRootObject();
            g_DnLiveHostState.last_error.clear();

            if (isReload)
            {
                for (auto *fn : g_DnLiveHostState.afterReloadFns)
                {
                    CallScript(fn);
                }
                CallLiveVarsFunctions("__after_reload_live_vars");
                g_DnLiveHostState.is_reload = false;
            }

            if (!hotCompileEnabled)
            {
                // Treat recompile as a fresh start: clear only once we have a working new script.
                ClearScriptGameObjects();
            }

            hasCompiledOnce = true;
            Ready();
        }
        else
        {
            g_DnLiveHostState.last_error = "Simulation failed";
            g_DnLiveHostState.is_reload = false;
        }

        compiled = simulated;
    }
    else
    {
        g_DnLiveHostState.last_error = lastCompileError;

        if (haltOnCompilationFail)
        {
            scriptReady = false;
        }
    }

    return compiled;
}

bool duin::GameScript::SetContextRootObject()
{
    bool res = IsValid() && context;
    if (res)
    {
        context->rootGameObject = this;
        context->gameWorld = gameWorld_;
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
        CallScript(fnGameReady);
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

void duin::GameScript::CallLiveVarsFunctions(const std::string &funcName)
{
    if (!context || !scriptReady)
    {
        return;
    }
    for (auto *fn : context->findFunctions(funcName.c_str()))
    {
        CallScript(fn);
    }
}

void duin::GameScript::CallAnnotatedScriptFunctions(const std::string &)
{
    // Annotation-registered functions are tracked via FunctionAnnotation::complete()
    // in Module_DnLiveHost. Use beforeReloadFns / afterReloadFns on g_DnLiveHostState.
}

void duin::GameScript::Update(double delta)
{
    uptimeAccum += static_cast<float>(delta);
    g_DnLiveHostState.dt = static_cast<float>(delta);
    g_DnLiveHostState.uptime = uptimeAccum;
    g_DnLiveHostState.fps = (delta > 0.0) ? static_cast<float>(1.0 / delta) : 0.0f;

    // Only hot recompile if it has been X seconds since last file change (with no further changes).
    // Timer then disabled to prevent continual recompilation, only reset when file is changed again.
    float cooldownTimerPrev = hotCompileFileChangeCooldownTimer;
    if (hotCompileFileChangeCooldownTimer > 0.0f)
    {
        hotCompileFileChangeCooldownTimer -= delta;
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
        vec4f args[1];
        args[0] = cast<double>::from(delta);
        CallScript(fnGameUpdate, args);
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
        vec4f args[1];
        args[0] = cast<double>::from(delta);
        CallScript(fnGamePhysicsUpdate, args);
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
        CallScript(fnGameDraw);
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
        CallScript(fnGameDrawUI);
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

    // if (context && context->rootGameObject)
    //{
    //     auto &root = *context->rootGameObject;
    //     for (auto &child : root.GetChildren())
    //     {
    //         if (child.lock())
    //         {
    //             RestartSGORecurse(child.lock());
    //         }
    //     }
    // }
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
