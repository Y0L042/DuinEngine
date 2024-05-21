#pragma once

#include "Duin/Core/Core.h"
#include "Duin/Core/UUID.h"
#include "Duin/Core/Debug/Log.h"

#include <cinttypes>
#include <vector>
#include <map>
#include <functional>
#include <any>

namespace Duin
{
    template<typename... Args>
    class DUIN_API Signal
    {
    public:
        using Callback = std::function<void(Args...)>;

        UUID Connect(std::function<void(Args...)> callback)
        {
            UUID uuid;
            if (uuidToIndexMap.find(uuid) != uuidToIndexMap.end()) 
            {
                DN_CORE_ERROR("Duplicate UUID!");
            }

            // Insert callback into the vector
            size_t index = callbacksVec.size();
            callbacksVec.emplace_back(uuid, std::move(callback));
            uuidToIndexMap[uuid] = index;

            return uuid;
        }

        // Disconnect a callback using its UUID
        void Disconnect(UUID uuid) 
        {
            auto it = uuidToIndexMap.find(uuid);
            if (it != uuidToIndexMap.end()) 
            {
                size_t index = it->second;
                size_t last_index = callbacksVec.size() - 1;

                if (index != last_index)
                {
                    // Swap with the last element
                    std::swap(callbacksVec[index], callbacksVec[last_index]);

                    // Update the map for the element that was swapped to the current position
                    uuidToIndexMap[callbacksVec[index].first] = index;
                }
                callbacksVec.pop_back();
                uuidToIndexMap.erase(it);
            }
        }

        void DisconnectAll()
        {
            callbacksVec.clear();
            uuidToIndexMap.clear();
        }

        // Emit a signal to all connected callbacks
        void Emit(Args... args)
        {
            for (auto& [uuid, callback] : callbacksVec) 
            {
                if (callback)
                {
                    callback(args...);
                }
                else
                {
                    invalidCallbacks.push_back(uuid);
                }
            }

            CleanupInvalidCallbacks();
        }

    private:
        std::vector<std::pair<UUID, std::function<void(Args...)>>> callbacksVec;
        std::map<UUID, size_t> uuidToIndexMap;
        std::vector<UUID> invalidCallbacks;

        void CleanupInvalidCallbacks()
        {
            auto new_end = std::remove_if
            (
                callbacksVec.begin(),
                callbacksVec.end(),
                [this](const std::pair<UUID, std::function<void(Args...)>>& entry)
                {
                    return std::find
                    (
                        invalidCallbacks.begin(),
                        invalidCallbacks.end(),
                        entry.first
                    ) != invalidCallbacks.end();
                }
            );
            callbacksVec.erase(new_end, callbacksVec.end());

            for (const UUID& uuid : invalidCallbacks)
            {
                uuidToIndexMap.erase(uuid);
            }

            invalidCallbacks.clear();
        }
    };


    class SignalBase
    {
    public:
        virtual ~SignalBase() = default;
        virtual void DisconnectAll() = 0;
        virtual void ConnectCallback(std::any callback) = 0;
        virtual void EmitSignal(std::any args) = 0;
    };

    template <typename... Args>
    class SignalWrapper : public SignalBase
    {
    public:
        Signal<Args...>& signal;

        SignalWrapper(Signal<Args...>& sig) : signal(sig) {}

        void DisconnectAll() override
        {
            signal.DisconnectAll();
        }

        void ConnectCallback(std::any callback) override
        {
            signal.Connect(std::any_cast<std::function<void(Args...)>>(callback));
        }

        void EmitSignal(std::any args) override
        {
            try {
                auto args_tuple = std::any_cast<std::tuple<Args...>>(args);
                std::apply([this](Args... unpackedArgs) { signal.Emit(unpackedArgs...); }, args_tuple);
            }
            catch (const std::bad_any_cast& e) {
                std::cerr << "Bad any cast: " << e.what() << std::endl;
            }
        }

    };
}
