#include "main.h"

#include <DuinMeta.h>
#include <Duin/EntryPoint.h>
#include <Duin/Script/GameScript.h>
#include <Duin/Script/ScriptModules.h>
#include <Duin/IO/Filesystem.h>
#include <flecs_das.h>
#include <external/imgui.h>

#include <memory>
#include <csignal>

#include "ArgParser.h"

#ifndef DEBUG
#define DEBUG
#endif
#include <string_view>
#include <iostream>
#include <Duin/Core/Debug/DNLog.h>

DuinRT *DuinRT::instance = nullptr;

duin::Application *duin::CreateApplication(int argc, char **argv)
{
    return new DuinRT(argc, argv);
}

DuinRT &DuinRT::Get()
{
    return *instance;
}

DuinRT::DuinRT(int argc, char **argv)
{
    // TODO parse flags, will be improved later
    argsCount = static_cast<size_t>(argc);

    for (int i = 0; i < argc; i++)
    {
        args.emplace_back(std::string_view(argv[i]));
    }

    ParseArgs(args);
    if (headlessMode)
    {
        std::signal(SIGINT, TerminationCallback);
        std::signal(SIGTERM, TerminationCallback);
        std::signal(SIGBREAK, TerminationCallback);
    }
}

void DuinRT::TerminationCallback(int sig)
{
    DN_WARN("RECEIVED INTERRUPT SIGNAL HANDLE {}", sig);
    if (instance != nullptr)
    {
        DN_INFO("Exiting program...");
    }
    std::exit(sig);
}



void DuinRT::SetScriptPath(const std::string &path)
{
    scriptPath = path;
}

void DuinRT::SetDasRoot(const std::string &path)
{
    dasRoot = path;
}

void DuinRT::SetProjectFile(const std::string &path)
{
    projectFile = path;
}

void DuinRT::Initialize()
{
    DuinRT::instance = this;

#ifdef DN_DEBUG
    duin::fs::SetBinDebugMode(true);
#endif

    SetHeadless(headlessMode);
    duin::SetFramerate(244);
    SetWindowStartupSize(1600, 900);
    SetWindowName("DuinRT");
    duin::SetAllowDockingInMain(true);
}

void DuinRT::Ready()
{
    duin::SetWindowResizable(true);

    script = CreateChildObject<duin::GameScript>(duin::fs::MapVirtualToSystemPath(scriptPath));
    if (!dasRoot.empty() && dasRoot.size() > 1)
    {
        script->SetDasRoot(dasRoot); // Absolute path
    }
    if (!projectFile.empty() && projectFile.size() > 1)
    {
        // script->SetProjectFile(duin::fs::MapVirtualToSystemPath(projectFile)); // virtual path relative to BIN
        script->SetProjectFile("C:\\Projects\\CPP_Projects\\Duin\\DuinRT\\.das_project");
    }
    if (jitMode != duin::Script::JitMode::NONE)
    {
        script->SetJitMode(jitMode, !jitNoCache);
    }
    script->InitModules([&]() {
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
        NEED_MODULE(Module_DnSceneBuilder);
        NEED_MODULE(Module_DecsGameWorld);
        NEED_MODULE(Module_DnInput);
        NEED_MODULE(Module_DnPhysicsServer);
        NEED_MODULE(Module_DnCharacterBody);
        NEED_MODULE(Module_DnFilesystem);
    });

    script->EnableHotCompile(false, false);
    script->SetHotCompileFileChangeCooldown(2.5f);
    script->CompileAndSimulate();
}

void DuinRT::OnEvent(duin::Event event)
{
}

void DuinRT::Update(double rDelta)
{
    if (duin::Input::IsKeyPressed(DN_SCANCODE_F5))
    {
        script->CompileAndSimulate();
    }
}

void DuinRT::PhysicsUpdate(double pDelta)
{
}

void DuinRT::Draw()
{
}

void DuinRT::DrawUI()
{
}

void DuinRT::Debug()
{
}

static const std::string SHORT_FLAG_TOK = "-";
static const std::string LONG_FLAG_TOK = "--";
static const std::string PATH_SEPARATOR_TOK = "/";

void DuinRT::ParseArgs(const std::vector<std::string_view> &args)
{
    for (int i = 1; i < args.size(); i++)
    {
        std::string_view arg = args[i];
        DN_INFO("PARSING ARG {}: <{}>", i, args[i]);
        if (args[i].starts_with(LONG_FLAG_TOK))
        {
            std::string_view lFlag = arg;
            lFlag.remove_prefix(std::min(lFlag.find_first_not_of(LONG_FLAG_TOK), lFlag.size()));
            DN_INFO("Parsing LONG_FLAG arg <{}>", lFlag);

            if (lFlag.compare("headless") == 0)
            {
                headlessMode = true;
                DN_INFO("Headless mode set <{}>", headlessMode);
                continue;
            }
            if (lFlag.compare("jit") == 0)
            {
                jitMode = duin::Script::JitMode::DIRECT;
                DN_INFO("JIT mode set DIRECT");
                continue;
            }
            if (lFlag.compare("jit-dll") == 0)
            {
                jitMode = duin::Script::JitMode::DLL;
                DN_INFO("JIT mode set DLL");
                continue;
            }
        }
        else if (args[i].starts_with(SHORT_FLAG_TOK))
        {
            std::string_view sFlag = args[i];
            sFlag.remove_prefix(std::min(sFlag.find_first_not_of(SHORT_FLAG_TOK), sFlag.size()));
            DN_INFO("Parsing SHORT_FLAG arg <{}>", sFlag);

            // Parse short flag
        }
        else
        {
            DN_INFO("Parsing PARAM arg <{}>", arg);
            SetScriptPath("bin://" + std::string(arg));
        }
    }
}
