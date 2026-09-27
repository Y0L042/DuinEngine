#include <daScript/daScript.h>

#include <Duin.h>
#include <Duin/EntryPoint.h>
#include <Duin/Scene/SceneBuilder.h>
#include <Duin/IO/FileModule.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>
#include <Duin/Physics/jolt/PhysicsServer.h>
#include <Duin/Physics/jolt/CollisionShape.h>
#include <external/imgui.h>

#include <flecs_das.h>

#include <algorithm>
#include <iostream>
#include <optional>

#include "LoadModel.h"

// #define TRACY_ON_DEMAND

static std::optional<duin::Model> model;
static std::optional<duin::Model> model_too;
static std::optional<duin::CollisionShape> stairs;

static const duin::Vector3 STAIRS_POSITION{5.0f, 2.0f, 5.0f};
static constexpr float STAIRS_SCALE = 0.05f;

// Builds a triangle-mesh collision shape from the loaded stairs model and adds it to the
// physics world as a static body, matched to how the model is drawn in Draw().
static void SpawnStairsCollision()
{
    if (!model_too.has_value() || !model_too.value().data)
    {
        DN_ERROR("Stairs model not loaded; skipping collision shape.");
        return;
    }

    const auto &verts = model_too.value().data->vertices;
    const auto &indices = model_too.value().data->indices;

    if (verts.empty() || indices.size() < 3)
    {
        DN_ERROR("Stairs model has no usable geometry ({} verts, {} indices).", verts.size(), indices.size());
        return;
    }

    duin::PxMesh mesh;
    mesh.vertices.reserve(verts.size());
    std::ranges::transform(verts, std::back_inserter(mesh.vertices), [](const auto &v) {
        return duin::Vector3{v.x, v.y, v.z};
    });

    // ModelData stores a flat uint16_t index list; PxMesh wants uint32_t triples.
    mesh.triangles.reserve(indices.size() / 3);
    for (size_t i = 0; i + 2 < indices.size(); i += 3)
    {
        mesh.triangles.push_back({
            static_cast<uint32_t>(indices[i]),
            static_cast<uint32_t>(indices[i + 1]),
            static_cast<uint32_t>(indices[i + 2]),
        });
    }

    stairs.emplace(mesh);

    duin::PhysicsServer::Get().CreateStaticBody(stairs.value(), STAIRS_POSITION, duin::Vector3(STAIRS_SCALE));

    DN_INFO("Stairs collision: {} verts, {} triangles.", mesh.vertices.size(), mesh.triangles.size());
}

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

        // static const char *debugArgv[] = {"DuinFPSDaslang", "--das-stepping-debugger"};
        // das::setCommandLineArguments(2, const_cast<char **>(debugArgv));

        mainScript = CreateChildObject<duin::GameScript>(ENTRY_SCRIPT);
        mainScript->SetDasRoot("D:\\Projects\\CPP_Projects\\Duin\\Duin\\vendor\\daslang");
        // mainScript->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\Duin\\duin_engine.das_project");
        mainScript->SetProjectFile(
            "D:\\Projects\\CPP_Projects\\Duin\\ExampleProjects\\DuinFPSDaslang\\duinfpsdaslang.das_project");
        mainScript->InitModules([]() {
            NEED_MODULE(Module_UriParser);
            das::register_builtin_modules();

            NEED_MODULE(Module_flecs);
            NEED_MODULE(Module_imgui);

            NEED_MODULE(Module_DnLog);
            NEED_MODULE(Module_DnRenderer);
            NEED_MODULE(Module_DnCamera);
            NEED_MODULE(Module_DnGameObject);
            NEED_MODULE(Module_DnECS);
            NEED_MODULE(Module_DnPipeline);
            NEED_MODULE(Module_DnInput);
            NEED_MODULE(Module_DnPhysicsServer);
            NEED_MODULE(Module_DnCharacterBody);
            NEED_MODULE(Module_DnApplication);
            NEED_MODULE(Module_DnFilesystem);
            NEED_MODULE(Module_DnAssetManager);
            NEED_MODULE(Module_DnModel);
        });
        mainScript->EnableHotCompile(false, false);
        mainScript->SetHotCompileFileChangeCooldown(1.0f);
        mainScript->CompileAndSimulate();

        model = LoadMesh("bin://models/SM_Prop_Vase_01.obj");
        model_too = LoadMesh("bin://models/SM_Buildings_Stairs_1x3_02P.obj");

        SpawnStairsCollision();
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
        DrawMesh(model, duin::Vector3(0.0f, 3.0f, 0.0f), 0.05f);
        //DrawMesh(model_too, STAIRS_POSITION, STAIRS_SCALE);
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

duin::Application *duin::CreateApplication(int argc, char **argv)
{
#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif
    return new DuinFPSDaslangApp();
}
