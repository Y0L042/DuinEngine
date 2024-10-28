#ifndef DEP_RAYGUI_H
#define DEP_RAYGUI_H

#if defined(_WIN32)
	#define NOGDI  // All GDI defines and routines
	#define NOUSER // All USER defines and routines
    #define WIN32_LEAN_AND_MEAN
    #define NOMINMAX
#endif


#ifdef DN_PLATFORM_WINDOWS
	#include <Windows.h>
#endif

#undef near
#undef far

#include <raylib.h>
#include <imgui.h>
#include <rlImGui.h>

#endif /* DEP_RAYGUI_H */
