#include "dnpch.h"
#include "Event.h"

namespace duin {
    void Event::SetSDLEvent(::SDL_Event e)
    {
        sdlEvent = e;
    }

    int InputEvent::IsKeyDown(DN_Keycode key)
	{
	    if (sdlEvent.type == SDL_EVENT_KEY_DOWN) {
            if (sdlEvent.key.key == key) {
                return 1;
            }
        }

        return 0;
	}
}
