#include <daScript/daScript.h>

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/FileModule.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>
#include <Duin/Physics/jolt/PhysicsServer.h>
#include <external/imgui.h>

#include <flecs_das.h>

#include <iostream>



class DuinFPSDaslangApp : public duin::Application
{
    const std::string ENTRY_SCRIPT = "scripts/main.das";
    std::shared_ptr<duin::GameScript> mainScript;

    int64_t mainModifyTime = 0;

    void Initialize() override
    {
        duin::SetFramerate(244);
        SetWindowStartupSize(1600, 900);
        SetWindowName("DuinFPS (Daslang)");

    }

    void Ready() override
    {
        duin::PhysicsServer::Get();
        mainScript = CreateChildObject<duin::GameScript>(ENTRY_SCRIPT);
        mainScript->SetDasRoot("C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
        //mainScript->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\Duin\\duin_engine.das_project");
        mainScript->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\ExampleProjects\\DuinFPSDaslang\\scripts\\duinfpsdaslang.das_project");
        mainScript->InitModules([]() {
            NEED_MODULE(Module_flecs);
            NEED_MODULE(Module_imgui);

            NEED_MODULE(Module_DnLiveHost);
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnRenderer);
            NEED_MODULE(Module_DnCamera);
            NEED_MODULE(Module_DnGameObject);
            NEED_MODULE(Module_DnECS);
            NEED_MODULE(Module_DecsGameWorld);
            NEED_MODULE(Module_DnInput);
            NEED_MODULE(Module_DnPhysicsServer);
            NEED_MODULE(Module_DnCharacterBody);
            NEED_MODULE(Module_DnApplication);
            NEED_MODULE(Module_DnFilesystem);
        });
        mainScript->EnableHotCompile(false, false);
        mainScript->CompileAndSimulate();
    }

    void Update(double delta) override
    {
        if (duin::Input::IsKeyPressed(DN_SCANCODE_F5))
        {
            mainScript->CompileAndSimulate();
        }
    }

    void PhysicsUpdate(double delta) override
    {
    }

    void Draw() override
    {
    }

    void DrawUI() override
    {
        auto *cam = duin::GetActiveCamera();
        auto pos = cam->GetPosition();
        auto target = cam->GetTarget();
        ImGui::Begin("C++ Dashboard");
        ImGui::Text("Camera:");
        ImGui::Text("Position {%.2f, %.2f, %.2f}", pos.x, pos.y, pos.z);
        ImGui::Text("Target {%.2f, %.2f, %.2f}", target.x, target.y, target.z);
        ImGui::End();
    }

    void Exit() override
    {
        mainScript->ResetScript();
    }
};

duin::Application *duin::CreateApplication()
{
#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif
    return new DuinFPSDaslangApp();
}
