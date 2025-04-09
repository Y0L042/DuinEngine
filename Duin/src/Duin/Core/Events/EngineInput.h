#pragma once

#include <SDL3/SDL_events.h>

namespace duin::Input {
    void ProcessSDLKeyboardEvent(::SDL_Event e);
    void CacheCurrentKeyState();
    void ClearCurrentKeyState();

    void ProcessSDLMouseEvent(::SDL_Event e);
    void UpdateMouseFrameDelta();
}
