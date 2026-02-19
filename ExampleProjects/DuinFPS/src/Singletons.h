#pragma once

#include <memory>

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/ECS/ECSModule.h>

extern duin::DebugConsole debugConsole;
extern duin::DebugWatchlist debugWatchlist;

extern std::unique_ptr<duin::GameWorld> world;

extern duin::Entity player;
