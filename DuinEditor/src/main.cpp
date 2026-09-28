#include "main.h"

#include <DuinMeta.h>
#include <Duin/EntryPoint.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>
#include <flecs_das.h>
#include <external/imgui.h>

#include <memory>

#ifndef DEBUG
#define DEBUG
#endif

Editor *Editor::instance = nullptr;

duin::Application *duin::CreateApplication(int argc, char **argv)
{
    return new Editor();
}

Editor &Editor::Get()
{
    return *instance;
}

void Editor::Initialize()
{
    Editor::instance = this;

#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif

    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
    SetWindowName("DuinEditor");
    duin::SetAllowDockingInMain(true);
}

void Editor::Ready()
{
    duin::SetWindowResizable(true);

    const std::string entryScript = "./scripts/_entry.das";
    script = CreateChildObject<duin::GameScript>(entryScript);
    script->SetDasRoot();
    script->SetProjectFile(duin::fs::FindProjectFile(entryScript));
    script->InitModules([]() {
        NEED_MODULE(Module_flecs);
        NEED_MODULE(Module_imgui);
        NEED_MODULE(Module_TOMLC17);

        NEED_MODULE(Module_DnLog);
        NEED_MODULE(Module_DnApplication);
        NEED_MODULE(Module_DnUUID);
        NEED_MODULE(Module_DnRenderer);
        NEED_MODULE(Module_DnCamera);
        NEED_MODULE(Module_DnGameObject);
        NEED_MODULE(Module_DnGameStateMachine);
        NEED_MODULE(Module_DnECS);
        NEED_MODULE(Module_DnPipeline);
        NEED_MODULE(Module_DnSceneBuilder);
        NEED_MODULE(Module_DnInput);
        NEED_MODULE(Module_DnPhysicsServer);
        NEED_MODULE(Module_DnCharacterBody);
        NEED_MODULE(Module_DnFilesystem);
        NEED_MODULE(Module_DnAssetManager);
        NEED_MODULE(Module_DnModel);
    });
    script->EnableHotCompile(false, false);
    script->SetHotCompileFileChangeCooldown(2.5f);
    script->CompileAndSimulate();
}

void Editor::OnEvent(duin::Event event)
{
}

void Editor::Update(double rDelta)
{
    if (duin::Input::IsKeyPressed(DN_SCANCODE_F5))
    {
        script->CompileAndSimulate();
    }
}

void Editor::PhysicsUpdate(double pDelta)
{
}

void Editor::Draw()
{
}

void Editor::DrawUI()
{
}

void Editor::Debug()
{
}
