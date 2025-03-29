#pragma once

#include <vector>
#include <functional>

#include "Event.h"

namespace duin {
    class EventHandler
    {
        public:
            static EventHandler& Get();

            EventHandler();
            ~EventHandler();

            void PollEvents();
            void RegisterInputEventListener(std::function<void(InputEvent)> listener);

        private:
            std::vector<std::function<void(InputEvent)>> inputEventListeners;

            void CallInputEventListeners(InputEvent event);
    };
}
