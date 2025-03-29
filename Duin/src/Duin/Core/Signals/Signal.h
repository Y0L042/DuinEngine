#pragma once

#include "Duin/Core/Utils/UUID.h"

#include <vector>
#include <functional>

namespace duin {

    template <typename... types>
    class Signal
    {
        public:
            bool Connect(std::function<void(types...)> callback)
            {
                listeners.push_back(callback);
            }

            bool Disconnect(std::function<void(types...)>);

            void Emit(types... args)
            {
                for (auto& listener : listeners) {
                    listener(&args...);
                }
            }

            UUID GetUUID() { return uuid; }

        private:
            UUID uuid;
            std::vector<std::function<void(types...)>> listeners;
    };
}
