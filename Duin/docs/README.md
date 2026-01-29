/**
@mainpage Duin Engine

@section intro Introduction

Duin is a C++20 game engine built on an Entity Component System (ECS) architecture.
It combines FLECS for data-oriented entity management with a traditional GameObject
hierarchy for scene organization.

@section features Features

- **FLECS ECS** - Entity/component data storage and system queries
- **PhysX Physics** - NVIDIA PhysX for rigid body dynamics and collision
- **BGFX Rendering** - Cross-platform graphics (D3D11/12, OpenGL, Vulkan, Metal)
- **SDL3** - Window management and input handling
- **ImGui** - Immediate mode GUI for tools and debugging

@section arch Architecture Overview

The engine combines two organizational patterns:

@subsection arch_ecs ECS (Data-Oriented)

Entities are identifiers with attached components (pure data). Systems query
entities by component patterns and process them efficiently.

@code{.cpp}
// Create entity with components
flecs::entity e = ecsManager.GetWorld().entity()
    .set<ECSComponent::Position3D>({0, 0, 0})
    .set<ECSComponent::Rotation3D>({})
    .add<ECSTag::PxDynamic>();
@endcode

@subsection arch_gameobject GameObject (Object-Oriented)

GameObjects form parent-child hierarchies with lifecycle callbacks (Init, Ready,
Update, Draw). Useful for logical organization and behavior.

@code{.cpp}
class Player : public duin::GameObject {
    void Update(double delta) override {
        // Per-frame logic
    }
};
@endcode

@section modules Modules

- @ref Core - Application lifecycle, events, signals, math, utilities
- @ref ECS - Entity Component System (FLECS wrapper)
- @ref Physics - PhysX integration
- @ref Render - BGFX rendering
- @ref Objects - GameObject hierarchy and state machine
- @ref IO - File I/O, JSON, configuration
- @ref Assets - Asset management

@section building Building

Duin uses Premake5 for project generation.

@subsection building_gen Generate Project Files

@code{.bash}
# Windows (Visual Studio 2022)
vendor\bin\premake\premake5.exe vs2022

# Linux/macOS
vendor/bin/premake/premake5 gmake2
@endcode

@subsection building_build Build

@code{.bash}
# MSBuild (Windows)
msbuild Duin.sln -property:Configuration=Debug -property:Platform=x64

# Or open Duin.sln in Visual Studio 2022
@endcode

@subsection building_deps Dependencies (First Time)

@code{.bash}
vendor\bin\premake\premake5.exe --deps vs2022
@endcode

@section coord Coordinate System

Right-hand rule with +Z toward the viewer (same convention as Godot).

@section status Status

Early development. API may change between versions.

*/
