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


            void RegisterInputEventListener(std::function<void(InputEvent)> listener);

        private:
            std::vector<std::function<void(InputEvent)>> inputEventListeners;

            void CallInputEventListeners(InputEvent event);
    };
}
