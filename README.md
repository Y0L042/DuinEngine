# Duin Engine

Famous last words:
> "A small, simple C++ game engine. The goal is to have a small, simple game engine that makes it nice and simple for coders and hobbyists to make small games. I wanted an engine, without all the UI, tooling, and extra features that most engines have. While those features can be extremely useful, as someone who likes a code-first approach, most of those features went unused."

So, uh, *"a small, simple C++ game engine"* it is not anymore. Welcome 3D, Jolt Physics, BGFX-based renderer, Daslang scripting... This could take a while.

**Start Date**:  26 April 2024

[Documentation](https://y0l042.github.io/DuinEngine/)

## System Requirements and Limitations

Duin currently builds on one setup only:

- **OS:** Windows 11, x64. Linux and macOS are not supported, and Windows 10 is untested.
- **Compiler / IDE:** Visual Studio 2026 (MSVC v145) with the *Desktop development with C++* workload. Older Visual Studio versions are not supported, because the dependency build output paths and generators are hard-wired to VS2026.
- **Language standard:** C++23.
- **Dependency builds are Debug only.** Every workspace configuration (`Debug`, `DebugCoverage`, `Release`, `Dist`, `Archive`) links against the Debug builds of the vendor libraries.
- **Static linking:** the engine and most dependencies are linked statically with the dynamic CRT (`/MD`). daslang is the exception and ships as DLLs that are copied next to the executables.

Tools needed to build the dependencies:

| Tool | Needed for | Notes |
|---|---|---|
| Git | Cloning every dependency | Must be on `PATH`. |
| CMake (native Windows build) | Most dependencies | Found through `CMAKE_ROOT`, `C:/Program Files/CMake`, or the copy bundled with Visual Studio. The MSYS2 CMake is ignored on purpose, because it defaults to Ninja + GCC. |
| MSBuild | bgfx / bx / bimg | Taken from the Visual Studio install. Override with `MSBUILD_BIN`. |
| LLVM (MSVC-targeting, with `lld-link.exe` and `LLVM-C.dll`) | daslang's LLVM JIT module | `LLVM_ROOT`, `C:/Program Files/LLVM`, or the Visual Studio LLVM component. |
| OpenSSL, via vcpkg (`x64-windows`) **or** Strawberry Perl (+ optional NASM) | daslang's dasHV module (used by DuinDasHost) | With vcpkg, the prebuilt OpenSSL is used. Otherwise OpenSSL is built from source with Perl. Override with `VCPKG_ROOT`, `PERL_BIN`, `NASM_BIN`. |
| Python + a bash shell (Git Bash or MSYS2) | Helper scripts (`manage.sh`, `generateprojects.sh`, code generation) | Not needed to build the solution itself. |

`premakeCfg.lua` prints which tools it found every time premake runs. Check that line first if a dependency build fails.

## Building

Premake 5 is included at `vendor/bin/premake/premake5.exe`, so you do not need to install it. Run all commands from the repository root.

### 1. Build the dependencies

The dependencies are not checked in. `premake5 --deps` clones each one into `Duin/vendor/<name>` at a pinned tag or commit and builds it, in the order defined in [`Duin/vendor/dependencies.lua`](Duin/vendor/dependencies.lua).

```bat
:: Fetch and build ALL dependencies (asks for confirmation; type "yes"). This takes a long time.
vendor\bin\premake\premake5.exe --deps

:: Rebuild only selected dependencies (they still run in dependency order)
vendor\bin\premake\premake5.exe --deps --SDL3 --BGFX

:: Show the last build result of every dependency
vendor\bin\premake\premake5.exe --deps STATUS
```

Dependency flags: `--FMT`, `--SPDLOG`, `--TOML11`, `--RAPIDJSON`, `--REFLECTCPP`, `--DOCTEST`, `--SDL3`, `--BX`, `--BIMG`, `--BGFX`, `--IMGUI`, `--IMGUIZMO`, `--FLECS`, `--JOLT`, `--PHYSX`, `--ASSIMP`, `--TRACY`, `--DASLANG`, `--DASIMGUI`, `--FLECSDASLANGBINDINGS`, `--TOMLDASLANG`.

Results are recorded in `Duin/vendor/build_status.log`. Each dependency's fetch and build steps are in its own `Duin/vendor/dep_<name>.lua` script.

> PhysX is still part of the full dependency run, but the engine now uses Jolt and no longer links PhysX.

### 2. Generate the Visual Studio solution

```bat
generateprojects.bat
```

or, from a bash shell:

```sh
./generateprojects.sh
```

Both run `premake5 vs2026` and generate the `Duin` solution and a `.vcxproj` for every project. The bash version also warns about machine-specific absolute paths in tracked files. Open the solution in Visual Studio 2026 and build. The default startup project is **DuinFPSDaslang**.

Run the generate step again whenever you add or remove source files or change a `premake5.lua`.

Build output goes to `<Project>/bin/<Config>-windows-x86_64/<Project>/`, and intermediates go to `bin-int/`.

## Repository Overview

| Path | Type | What it is |
|---|---|---|
| [`Duin/`](Duin/) | Static library | **The engine.** Source is in `Duin/src/Duin/`: `Core` (application, windows, events, signals, maths, debug/logging), `Render` (bgfx renderer), `ECS` (flecs integration, components, prefabs, pipeline), `Physics` (Jolt), `Assets` (asset manager, model loading via assimp), `IO` (filesystem, virtual FS, JSON/TOML config, serialisation), `Scene`, `Objects`, `Resources`, and `Script` (daslang runtime and engine script modules). Third-party code that is compiled into the engine (ImGui, bgfx helpers, and similar) is in `Duin/src/external/`. Dependency build scripts are in `Duin/vendor/`. |
| [`DuinRT/`](DuinRT/) | Executable | The script runtime. It runs a daslang game project on the engine (`duinrt MyProject/main.das`). |
| [`DuinEditor/`](DuinEditor/) | Executable | The editor. The application shell is C++ (`src/`), and the editor UI (scene tree, entity properties, file browser, scene viewer) is written in daslang (`scripts/`). |
| [`DuinDasHost/`](DuinDasHost/) | Executable | A daslang tooling host. It runs as a per-project daemon (TCP or WebSocket) that compiles scripts with the engine modules loaded, for diagnostics, go-to-definition, find-references, and similar. `scripts/proxy.das` is the LSP proxy that editors connect to. |
| [`DaslangDocGenerator/`](DaslangDocGenerator/) | Executable | Generates documentation for the engine's daslang modules. |
| [`DuinTests/`](DuinTests/) | Executable | Engine unit tests (doctest), plus daslang script tests in `scripts/`. Run the script tests with `./manage.sh run-duin-das-tests`. |
| [`DuinEditorTests/`](DuinEditorTests/) | Executable | Unit tests for editor code. |
| [`ExampleProjects/DuinFPSDaslang/`](ExampleProjects/DuinFPSDaslang/) | Executable | FPS example with gameplay written in daslang. This is the default startup project. |
| [`ExampleProjects/DuinFPS/`](ExampleProjects/DuinFPS/) | Executable | [OUTDATED] FPS example with gameplay written in C++ (player state machine, game objects). |
| [`ExampleProjects/DuinFPSTests/`](ExampleProjects/DuinFPSTests/) | Executable | Tests for the C++ FPS example. |
| [`ExampleProjects/Sandbox/`](ExampleProjects/Sandbox/) | Executable | Small, isolated engine experiments (for example, multi-window rendering). |
| [`ExampleProjects/Daslang/`](ExampleProjects/Daslang/) | Scripts | Standalone daslang experiments. Not part of the solution. |
| [`ScriptTests/`](ScriptTests/) | Executables | Early scripting prototypes. `DaslangTest` is in the solution. `AngelScriptTest` is kept for reference and is not built. |
| [`DasProDebugger/`](DasProDebugger/) | Scripts | Experimental daslang debugger, run with `daslang-live` through its `Makefile`. Not part of the solution. |
| [`docs/`](docs/) | Docs | Design notes and planning. The generated API documentation is published at the documentation link above. |
| [`tools/`](tools/) | Scripts | Code-generation and repository helper scripts. |
| [`vendor/bin/premake/`](vendor/bin/premake/) | Tool | The bundled `premake5.exe`. |
| `premake5.lua`, `premakeCfg.lua` | Build | Workspace definition (shared include paths, libraries, defines) and toolchain configuration (Visual Studio version, CRT, tool lookup). Each project also has its own `premake5.lua`. |
| `manage.sh` | Script | Script code generation: embeds the engine's `.das` files as `.inc` files and generates the C++/daslang binding adapters (`./manage.sh codegen`). Run `./manage.sh help` for all commands. |


A thanks to @raysan5 and [Raylib](https://www.raylib.com/) for making C++ game development fun!
