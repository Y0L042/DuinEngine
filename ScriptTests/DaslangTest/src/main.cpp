#include "daScript/daScript.h"
#include "Module_DuinImGui.h"

#include <Duin.h>
#include <Duin/EntryPoint.h>

#include <iostream>

// NEED_ALL_DEFAULT_MODULES and NEED_MODULE must be at file scope
// (they expand to static registrations)

class DaslangTestApp : public duin::Application
{
    das::ProgramPtr program;
    das::Context* ctx = nullptr;
    das::SimFunction* fnDrawUI = nullptr;

    das::TextPrinter tout;
    das::ModuleGroup libGroup;

    void Initialize() override
    {
        SetWindowStartupSize(1280, 720);
        SetWindowName("Daslang ImGui Test");
    }

    void Ready() override
    {
NEED_ALL_DEFAULT_MODULES;
NEED_MODULE(Module_DuinImGui);
        das::Module::Initialize();

        ReloadScript();
    }

    void Update(double delta) override
    {
        if (duin::Input::IsKeyPressed(DN_SCANCODE_F5))
        {
            ReloadScript();
            DN_INFO("Reloading script...");
        }
    }

    void ReloadScript()
    {
        delete ctx;
        ctx = nullptr;
        fnDrawUI = nullptr;

        auto fAccess = das::make_smart<das::FsFileAccess>();
        program = das::compileDaScript("scripts/imgui_demo.das", fAccess, tout, libGroup);
        if (program->failed())
        {
            tout << "Compilation failed:\n";
            for (auto& err : program->errors)
            {
                tout << das::reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
            }
            return;
        }

        ctx = new das::Context(program->getContextStackSize());
        if (!program->simulate(*ctx, tout))
        {
            tout << "Simulation failed:\n";
            for (auto& err : program->errors)
            {
                tout << das::reportError(err.at, err.what, err.extra, err.fixme, err.cerr);
            }
            delete ctx;
            ctx = nullptr;
            return;
        }

        fnDrawUI = ctx->findFunction("draw_ui");
        if (!fnDrawUI)
        {
            tout << "Function 'draw_ui' not found in script\n";
        }
    }

    void DrawUI() override
    {
        if (fnDrawUI && ctx)
        {
            ctx->evalWithCatch(fnDrawUI, nullptr);
            if (auto ex = ctx->getException())
            {
                std::cerr << "Script exception: " << ex << std::endl;
            }
        }
    }

    void Exit() override
    {
        fnDrawUI = nullptr;
        program.reset();
        delete ctx;
        ctx = nullptr;
        das::Module::Shutdown();
    }
};

duin::Application* duin::CreateApplication()
{
    return new DaslangTestApp();
}
