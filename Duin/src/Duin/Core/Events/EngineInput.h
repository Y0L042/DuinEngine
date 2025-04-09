#pragma once

#include <SDL3/SDL_events.h>

namespace duin::Input {
    void EngineInput_GetEvent(::SDL_Event e);
    void ProcessSDLKeyboardEvent(::SDL_Event e);
    void CacheCurrentKeyState();
    void ClearCurrentKeyState();
}
