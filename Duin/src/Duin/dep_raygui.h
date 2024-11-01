#ifndef DEP_RAYGUI_H
#define DEP_RAYGUI_H

#pragma warning(disable: 4006)

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
#include <raygui.h>
#include <raymath.h>
#include <rcamera.h>

#include <imgui.h>
#include <ImGuiFileDialog.h>
#include <rlImGui.h>

#pragma warning(default: 4006)

#endif /* DEP_RAYGUI_H */
