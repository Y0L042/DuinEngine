#include <daScript/daScript.h>

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>

#include <flecs_das.h>

class DaslangDocGeneratorApp : public duin::Application
{
    std::shared_ptr<duin::GameScript> docScript;

    void Initialize() override
    {
    }

    void Ready() override
    {
        docScript = CreateChildObject<duin::GameScript>("scripts/docgen.das");
        docScript->SetDasRoot("C:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
        docScript->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\DaslangDocGenerator\\scripts\\docgen.das_project");
        docScript->InitModules([]() {
            NEED_MODULE(Module_flecs);
            NEED_MODULE(Module_imgui);
            NEED_MODULE(Module_TOMLC17);

            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnRenderer);
            NEED_MODULE(Module_DnCamera);
            NEED_MODULE(Module_DnGameObject);
            NEED_MODULE(Module_DnGameStateMachine);
            NEED_MODULE(Module_DnECS);
            NEED_MODULE(Module_DecsGameWorld);
            NEED_MODULE(Module_DnSceneBuilder);
            NEED_MODULE(Module_DnInput);
            NEED_MODULE(Module_DnPhysicsServer);
            NEED_MODULE(Module_DnCharacterBody);
            NEED_MODULE(Module_DnApplication);
            NEED_MODULE(Module_DnFilesystem);
            NEED_MODULE(Module_DnUUID);
        });
        docScript->EnableHotCompile(false, false);
        docScript->CompileAndSimulate();
    }
};

duin::Application *duin::CreateApplication(int argc, char **argv)
{
#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif
    return new DaslangDocGeneratorApp();
}
