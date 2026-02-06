#pragma once
#include <flecs.h>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <unordered_map>
#include <functional>
#include <string>
#include "./DECS/World.h"

namespace duin
{

class ComponentSerializer
{
  public:
    static ComponentSerializer &Get()
    {
        static ComponentSerializer instance;
        return instance;
    }

    using SerializeFn = std::function<std::string(const void *component_ptr)>;
    using DeserializeFn = std::function<void(void *component_ptr, const std::string &json)>;

    template <typename T>
    void RegisterComponent(flecs::world &world, flecs::entity typeEntity)
    {
        std::string typeName = static_cast<std::string>(typeEntity.name());

        // Serialize function
        serializers_[typeName] = [](const void *ptr) -> std::string {
            const T *typed_ptr = static_cast<const T *>(ptr);
            return rfl::json::write(*typed_ptr);
        };

        // Deserialize function
        deserializers_[typeName] = [](void *ptr, const std::string &json) {
            T *typed_ptr = static_cast<T *>(ptr);
            auto result = rfl::json::read<T>(json);
            if (result)
            {
                *typed_ptr = result.value();
            }
        };
    }

    std::string Serialize(const std::string &typeName, const void *componentPtr) const
    {
        auto it = serializers_.find(typeName);
        if (it != serializers_.end())
        {
            return it->second(componentPtr);
        }
        return "{}";
    }

    void Deserialize(const std::string &typeName, void *componentPtr, const std::string &json)
    {
        auto it = deserializers_.find(typeName);
        if (it != deserializers_.end())
        {
            it->second(componentPtr, json);
        }
    }

  private:
    std::unordered_map<std::string, SerializeFn> serializers_;
    std::unordered_map<std::string, DeserializeFn> deserializers_;
};

} // namespace duin