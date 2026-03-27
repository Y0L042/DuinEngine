#include "dnpch.h"
#include "GameScript.h"
#include "Duin/IO/FileModule.h"
#include "./GameObject/Module_DnGameObject.h"

duin::GameScript::GameScript(const std::string &relScriptPath) : Script(relScriptPath), GameObject()
{
}

duin::GameScript::~GameScript()
{
    Exit();
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

bool duin::GameScript::HotCompileAndSimulate()
{
    bool compiled = false;

    compiled = CompileAndSimulate();
    if (compiled)
    {
        SetContextRootObject();
        RestartScriptGameObjects();
    }

    return compiled;
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

bool duin::GameScript::CompileAndSimulate(bool skipReady)
{
    ResetMuteWarningFlags();
    bool compiled = false;
    compiled = Compile();
    if (compiled)
    {
        SimulateContext();
        SetGameFunctions();
        if (!skipReady)
        {
            Ready();
        }
    }
    else
    {
        if (haltOnCompilationFail)
        {
            scriptReady = false;
        }
    }

    return compiled;
}

void duin::GameScript::Exit()
{
    ResetGameFunctions();
    Script::Exit();
}

void duin::GameScript::Init()
{
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

void duin::GameScript::Update(double delta)
{
    if (hotCompileEnabled)
    {
        duin::fs::PathInfo pInfo;
        bool scriptFound = duin::vfs::GetPathInfo("bin://" + scriptPath, &pInfo);
        if (scriptFound && (pInfo.modifyTime != scriptLastModified))
        {
            scriptLastModified = pInfo.modifyTime;

            if (!HotCompileAndSimulate())
            {
                DN_CORE_WARN("Hot compilation of {} failed!", "bin://" + scriptPath);
            }
        }
    }

    if (scriptReady)
    {
        CallScript(fnGameUpdate);
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
        CallScript(fnGamePhysicsUpdate);
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

void duin::GameScript::RestartScriptGameObjects()
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
