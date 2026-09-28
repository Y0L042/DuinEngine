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
        const std::string entryScript = "scripts/docgen.das";
        const std::string projectFile = duin::fs::FindProjectFile(entryScript);

        // docgen.das writes to wrk://docs/generated; the workspace is the DaslangDocGenerator
        // source dir (parent of scripts/, where docgen.das_project lives)
        if (!duin::fs::IsPathInvalid(projectFile))
        {
            std::filesystem::path generatorDir = std::filesystem::path(projectFile).parent_path().parent_path();
            duin::fs::SetWorkspacePath(generatorDir.generic_string());
        }

        docScript = CreateChildObject<duin::GameScript>(entryScript);
        docScript->SetDasRoot();
        docScript->SetProjectFile(projectFile);
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
            NEED_MODULE(Module_DnPipeline);
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
