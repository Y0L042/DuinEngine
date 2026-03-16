#pragma once

#include <SDL3/SDL_events.h>

namespace duin::Input
{
void ProcessSDLKeyboardEvent(::SDL_Event e);
void CacheCurrentKeyState();
void ClearCurrentKeyState();
void CacheCurrentMouseKeyState();
void ClearCurrentMouseKeyState();
void ClearCurrentModKeyState();
void ResetMouseFrameState();
void ResetAllInputState();

void ProcessSDLMouseEvent(::SDL_Event e);
void UpdateMouseFrameDelta();
void ClearCurrentMouseDelta();
} // namespace duin::Input
