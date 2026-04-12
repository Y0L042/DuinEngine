#include <daScript/daScript.h>

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/FileModule.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>

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
        mainScript = CreateChildObject<duin::GameScript>(ENTRY_SCRIPT);
        mainScript->SetDasRoot("C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
        mainScript->InitModules([]() {
            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnGameObject);
            NEED_MODULE(Module_flecs);
            NEED_MODULE(Module_DnECS);
            NEED_MODULE(Module_DecsGameWorld);
        });
        mainScript->EnableHotCompile(true, true);
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
