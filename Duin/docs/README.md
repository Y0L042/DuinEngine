@mainpage ReadMe
@{

Duin Engine is small C++ game engine. It is based on an ECS design.

## Features

- FLECS ECS-based entity management
- PhysX physics
- raylib functionality

Currently it is only available for Windows (x64). Sorry about that.

## Generating Project Files

Currently the engine is super early in development. As such, a proper build
system and project configuration has not been set up yet.

### Requirements

 - Visual Studio 2022
 - CMake (for dependencies)
 - Python 3
 - Git

##

To build the dependencies, run:

```bash
vendor/bin/premake/premake5 vs2022 --deps
```

Specific dependencies can be built with ``--[DEP]``

To just generate the project files, run:

```bash
vendor/bin/premake/premake5 vs2022
```

To create a new project, currently one has to add the project to the bottom of
the root ``premake5.lua``, and then use one of the ``ExampleProject``s as a
template for your own project. This will be fixed in the future.

See [ToDo](TODO.md) for planned features.

@}
