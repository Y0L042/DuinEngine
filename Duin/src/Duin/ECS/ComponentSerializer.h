#pragma once
#include <flecs.h>
#include <rfl.hpp>
#include <rfl/json.hpp>
#include <rfl/type_name_t.hpp>
#include <unordered_map>
#include <functional>
#include <string>
#include "./DECS/World.h"
#include "./DECS/Entity.h"
#include "Duin/Core/Debug/DNLog.h"

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
    using DeserializeFn = std::function<void(Entity e, void *component_ptr, const std::string &json)>;

    template <typename T>
    void RegisterComponent(flecs::world &world, flecs::entity typeEntity)
    {
        std::string typeName = static_cast<std::string>(typeEntity.name());

        if (typeAliases_.contains(typeName))// && serializers_.contains(typeName) && deserializers_.contains(typeName))
        {
            return;
        }

        // Serialize function
        serializers_[typeName] = [](const void *ptr) -> std::string {
            if (ptr == nullptr) /* Tag */
            {
                const T tag;
                return rfl::json::write<rfl::AddStructName<"type">>(tag);
            }
            else /* Component */
            {
                const T *typed_ptr = static_cast<const T *>(ptr);
                return rfl::json::write<rfl::AddStructName<"type">>(*typed_ptr);
            }
        };

        // Deserialize function
        bool isTag = ecs_id_is_tag(world.c_ptr(), typeEntity.raw_id());
        if (isTag)
        {
            deserializers_[typeName] = [](Entity e, void *ptr, const std::string &json) { e.Add<T>(); };
        }
        else /* Component */
        {
            // Check if T has ReflectionType (uses the reflection pattern)
            if constexpr (requires { typename T::ReflectionType; })
            {
                deserializers_[typeName] = [](Entity e, void *ptr, const std::string &json) {
                    // Deserialize the ReflectionType (e.g., Transform3DImpl)
                    auto result = rfl::json::read<typename T::ReflectionType>(json);
                    if (result)
                    {
                        // Construct T from ReflectionType using T(const ReflectionType&) constructor
                        T component(result.value());
                        e.Set<T>(component);
                    }
                };
            }
            else
            {
                deserializers_[typeName] = [](Entity e, void *ptr, const std::string &json) {
                    T *typed_ptr = static_cast<T *>(ptr);
                    auto result = rfl::json::read<T>(json);
                    if (result)
                    {
                        typed_ptr = new T;
                        *typed_ptr = result.value();
                        e.Set<T>(*typed_ptr);
                    }
                };
            }
        }

        // Register type alias for types with ReflectionType (like Transform3D -> Transform3DImpl)
        if constexpr (requires { typename T::ReflectionType; })
        {
            std::string fullReflectionTypeName = rfl::type_name_t<typename T::ReflectionType>().str();

            // Extract unqualified name (e.g., "duin::ECSComponent::Velocity2D::Velocity2DImpl" -> "Velocity2DImpl")
            std::string unqualifiedName = fullReflectionTypeName;
            size_t lastColonPos = fullReflectionTypeName.find_last_of(':');
            if (lastColonPos != std::string::npos)
            {
                unqualifiedName = fullReflectionTypeName.substr(lastColonPos + 1);
            }

            typeAliases_[unqualifiedName] = typeName;
            DN_CORE_INFO("Registered type alias: {} -> {}", unqualifiedName, typeName);
        }

        DN_CORE_INFO("Registered {} {}", isTag ? "Tag" : "Component", typeName);
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

    bool IsRegistered(const std::string &typeName) const
    {
        return serializers_.find(typeName) != serializers_.end();
    }

    void Deserialize(Entity e, const std::string &typeName, void *componentPtr, const std::string &json)
    {
        // Check if this is an aliased type name (e.g., "Transform3DImpl" -> "Transform3D")
        std::string resolvedTypeName = typeName;
        auto aliasIt = typeAliases_.find(typeName);
        if (aliasIt != typeAliases_.end())
        {
            resolvedTypeName = aliasIt->second;
        }

        auto it = deserializers_.find(resolvedTypeName);
        if (it != deserializers_.end())
        {
            it->second(e, componentPtr, json);
        }
    }

  private:
    std::unordered_map<std::string, SerializeFn> serializers_;
    std::unordered_map<std::string, DeserializeFn> deserializers_;
    std::unordered_map<std::string, std::string> typeAliases_; // Maps reflection type name to component name
    int INVALID_PTR_ = -1;
};

} // namespace duin