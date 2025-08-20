#pragma once

#include <SDL3/SDL_events.h>

namespace duin {
    typedef Uint32 DN_Keycode;

    class Event
    {
        public:
            ::SDL_Event sdlEvent;

        protected:
            friend class EventHandler;

            void SetSDLEvent(::SDL_Event e);
    };



    class InputEvent : public Event
    {
        public:
            InputEvent() = default;
            ~InputEvent() = default;

            int IsKeyDown(DN_Keycode key);
            int IsMouseButtonPressed();


        private:

    };

    class WindowEvent : public Event
    {
    public:
        WindowEvent() = default;
        ~WindowEvent() = default;

    private:
    };
}
