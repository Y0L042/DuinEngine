#pragma once

#include <vector>
#include <functional>

#include "Event.h"

namespace duin {
    class EventHandler
    {
        public:
            static EventHandler& Get();
            static void GetPolledEvent(::SDL_Event e);

            EventHandler();
            ~EventHandler();


            void RegisterInputEventListener(std::function<void(Event)> listener);

        private:
            std::vector<std::function<void(Event)>> inputEventListeners;

            void CallInputEventListeners(Event event);
    };
}
