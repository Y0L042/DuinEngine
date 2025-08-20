#pragma once

#include "Duin/Core/Utils/UUID.h"

#include <vector>
#include <functional>
#include <utility>

namespace duin {
    template <typename... types>
    class Signal
    {
        public:
            using Listener = std::pair<UUID, std::function<void(types...)>>;

            UUID Connect(std::function<void(types...)> callback)
            {
                UUID uuid;
                listeners.emplace_back(Listener(uuid, callback));

                return uuid;
            }

            bool Disconnect(UUID uuid)
            {
                for (int i = 0; i < listeners.size(); ++i) {
                    if (listeners[i].first == uuid) {
                        std::swap(listeners.back(), listeners.at(i));
                        listeners.pop_back();
                        
                        return true;
                    }
                }

                return false;
            }

            void Emit(types... args)
            {
                for (Listener& listener : listeners) {
                    if (listener.second) {
                        listener.second(std::forward<types>(args)...);
                    }
                }
            }

            UUID GetUUID() { return uuid; }

        private:
            UUID uuid;
            std::vector<Listener> listeners;
    };
}
