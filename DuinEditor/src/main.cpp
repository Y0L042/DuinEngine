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


duin::Application *duin::CreateApplication()
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

    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
    SetWindowName("DuinEditor");
    duin::SetAllowDockingInMain(true);
}

void Editor::Ready()
{
    duin::SetWindowResizable(true);

    script = CreateChildObject<duin::GameScript>("./scripts/_entry.das");
    script->SetDasRoot("C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
    script->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\DuinEditor\\duineditor.das_project");
    script->InitModules([]() {
        NEED_MODULE(Module_flecs);
        NEED_MODULE(Module_imgui);

        NEED_MODULE(Module_DnLiveHost);
        NEED_MODULE(Module_DnLog);
        NEED_MODULE(Module_DnApplication);
        NEED_MODULE(Module_DnRenderer);
        NEED_MODULE(Module_DnCamera);
        NEED_MODULE(Module_DnGameObject);
        NEED_MODULE(Module_DnGameStateMachine);
        NEED_MODULE(Module_DnECS);
        NEED_MODULE(Module_DecsGameWorld);
        NEED_MODULE(Module_DnInput);
        NEED_MODULE(Module_DnPhysicsServer);
        NEED_MODULE(Module_DnCharacterBody);
    });
    script->EnableHotCompile(true, false);
    script->HotCompileAndSimulate();
}

void Editor::OnEvent(duin::Event event)
{
}

void Editor::Update(double rDelta)
{
    if (duin::Input::IsKeyPressed(DN_SCANCODE_F5))
    {
        script->HotCompileAndSimulate();
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
