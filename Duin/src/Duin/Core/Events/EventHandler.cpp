#include "dnpch.h"
#include "EventHandler.h"

#include "Input.h"
#include "EngineInput.h"

#include <SDL3/SDL_events.h>
#include "external/backends/imgui_impl_sdl3.h"


#define EVENT_IS_KEYBOARD(event) (event >= 0x300 && event < 0x400)
#define EVENT_IS_MOUSE(event) (event >= 0x400 && event < 0x600)

namespace duin {
            EventHandler& EventHandler::Get()
            {
                static EventHandler instance;
                return instance;
            }

            EventHandler::EventHandler() {}
            EventHandler::~EventHandler() {}

            void EventHandler::GetPolledEvent(::SDL_Event e)
            {
                EventHandler& instance = Get();
                if (EVENT_IS_KEYBOARD(e.type) || EVENT_IS_MOUSE(e.type)) {
                    InputEvent event;
                    event.SetSDLEvent(e);
                    instance.CallInputEventListeners(event);
                }
            }

            void EventHandler::RegisterInputEventListener(std::function<void(InputEvent)> listener)
            {
                inputEventListeners.push_back(listener);
            }

            void EventHandler::CallInputEventListeners(InputEvent event)
            {
                for (auto& listener : inputEventListeners) {
                    listener(event);
                }
            }
}
