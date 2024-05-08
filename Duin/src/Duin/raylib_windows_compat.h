#pragma once


// Windows workarounds for CloseWindow / ShowCursor errors

#if defined(_WIN32)
	#define NOGDI  // All GDI defines and routines
	#define NOUSER // All USER defines and routines
#endif


//#include <fmt/core.h>
//#include <spdlog/spdlog.h>
#include "Duin/Core/Debug/Log.h"

#ifdef DN_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#undef near
#undef far

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>
//
#include <raylib-cpp.hpp>


