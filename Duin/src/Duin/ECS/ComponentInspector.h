#pragma once
#include <rfl.hpp>
#include <rfl/to_view.hpp>
#include <external/imgui.h>
#include <unordered_map>
#include <functional>
#include <string>
#include <string_view>
#include <cstring>
#include "Duin/Core/Debug/DNLog.h"
#include "Duin/ECS/DECS/Entity.h"

namespace duin
{

/**
 * @class ComponentInspector
 * @brief Singleton registry that renders ImGui inspector widgets for any registered component.
 *
 * Uses rfl::to_view on the component's ReflectionType to iterate fields at compile time,
 * then dispatches ImGui widgets based on primitive type. Changes are written back via
 * the T(const ReflectionType&) constructor + entity.Set<T>().
 *
 * Usage:
 *   ComponentInspector::Get().RegisterComponent<MyComp>("MyComp", entity);
 *   ComponentInspector::Get().Inspect("MyComp", rawPtr, duinEntity);
 */
class ComponentInspector
{
  public:
    static ComponentInspector &Get()
    {
        static ComponentInspector instance;
        return instance;
    }

    // InspectFn receives:
    //   ptr  - mutable raw pointer to the live component data (may be null for tags)
    //   e    - the entity owning the component (for Set<T> writeback)
    using InspectFn = std::function<void(void *ptr, Entity &e)>;

    /**
     * @brief Register a component type for inspection.
     * @tparam T  Component type (must match the type registered with flecs).
     * @param typeName  Flecs component name (e.g. "Transform3D").
     */
    template <typename T>
    void RegisterComponent(const std::string &typeName)
    {
        if (inspectors_.contains(typeName))
            return;

        if constexpr (std::is_empty_v<T>)
        {
            // Tags have no data
            inspectors_[typeName] = [](void *, Entity &) { ImGui::TextDisabled("(tag)"); };
        }
        else if constexpr (requires { typename T::ReflectionType; })
        {
            // Component with ReflectionType pattern:
            //   - read fields from a copy of reflection()
            //   - write back via T(edited_impl) + entity.Set<T>
            inspectors_[typeName] = [](void *ptr, Entity &e) {
                if (!ptr)
                {
                    ImGui::TextDisabled("(null ptr)");
                    return;
                }
                T *comp = static_cast<T *>(ptr);
                typename T::ReflectionType impl = comp->reflection();
                bool changed = false;
                auto view = rfl::to_view(impl);
                view.apply([&changed](const auto &field) {
                    if (DrawField(field.name(), *field.value()))
                        changed = true;
                });
                if (changed)
                    e.Set<T>(T(impl));
            };
        }
        else if constexpr (std::is_aggregate_v<T> && std::is_class_v<T>)
        {
            // Aggregate struct without ReflectionType: use rfl::to_view directly.
            // Changes are written back in-place since ptr is already the live data.
            inspectors_[typeName] = [](void *ptr, Entity &) {
                if (!ptr)
                {
                    ImGui::TextDisabled("(null ptr)");
                    return;
                }
                T *comp = static_cast<T *>(ptr);
                auto view = rfl::to_view(*comp);
                view.apply([](const auto &field) { DrawField(field.name(), *field.value()); });
            };
        }
        else
        {
            // Not reflectable and no ReflectionType — show opaque placeholder
            inspectors_[typeName] = [](void *, Entity &) { ImGui::TextDisabled("(opaque)"); };
        }
    }

    /**
     * @brief Render inspector widgets for a component by type name.
     * @param typeName  The flecs component name.
     * @param ptr       Mutable raw pointer to the component data (null for tags).
     * @param e         The owning entity (used for writeback).
     * @return True if an inspector was found and called, false otherwise.
     */
    bool Inspect(const std::string &typeName, void *ptr, Entity &e) const
    {
        auto it = inspectors_.find(typeName);
        if (it == inspectors_.end())
            return false;
        it->second(ptr, e);
        return true;
    }

    bool IsRegistered(const std::string &typeName) const
    {
        return inspectors_.contains(typeName);
    }

  private:
    std::unordered_map<std::string, InspectFn> inspectors_;

    /**
     * @brief Render a single ImGui widget for a named field.
     * @return True if the value was modified.
     */
    template <typename T>
    static bool DrawField(std::string_view name, T &value)
    {
        // Build a unique ImGui label: "fieldName##addr"
        char label[128];
        std::snprintf(label, sizeof(label), "%.*s", static_cast<int>(name.size()), name.data());

        if constexpr (std::is_same_v<T, float>)
        {
            return ImGui::DragFloat(label, &value, 0.05f);
        }
        else if constexpr (std::is_same_v<T, double>)
        {
            float v = static_cast<float>(value);
            if (ImGui::DragFloat(label, &v, 0.05f))
            {
                value = static_cast<double>(v);
                return true;
            }
            return false;
        }
        else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int32_t>)
        {
            return ImGui::DragInt(label, &value);
        }
        else if constexpr (std::is_same_v<T, uint32_t>)
        {
            int v = static_cast<int>(value);
            if (ImGui::DragInt(label, &v, 1, 0, INT_MAX))
            {
                value = static_cast<uint32_t>(v);
                return true;
            }
            return false;
        }
        else if constexpr (std::is_same_v<T, bool>)
        {
            return ImGui::Checkbox(label, &value);
        }
        else if constexpr (std::is_same_v<T, std::string>)
        {
            char buf[256];
            std::strncpy(buf, value.c_str(), sizeof(buf) - 1);
            buf[sizeof(buf) - 1] = '\0';
            if (ImGui::InputText(label, buf, sizeof(buf)))
            {
                value = buf;
                return true;
            }
            return false;
        }
        else if constexpr (std::is_pointer_v<T> || requires { typename T::element_type; })
        {
            // Pointers / smart pointers — show read-only address
            ImGui::LabelText(label, "%p", static_cast<const void *>(&value));
            return false;
        }
        else if constexpr (requires { typename T::ReflectionType; })
        {
            // Nested type with ReflectionType — recurse via reflection()
            bool changed = false;
            if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
            {
                typename T::ReflectionType impl = value.reflection();
                auto view = rfl::to_view(impl);
                view.apply([&changed](const auto &f) {
                    if (DrawField(f.name(), *f.value()))
                        changed = true;
                });
                if (changed)
                    value = T(impl);
                ImGui::TreePop();
            }
            return changed;
        }
        else if constexpr (std::is_aggregate_v<T> && std::is_class_v<T>)
        {
            // Generic aggregate struct — recurse via rfl::to_view directly
            bool changed = false;
            if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
            {
                auto view = rfl::to_view(value);
                view.apply([&changed](const auto &f) {
                    if (DrawField(f.name(), *f.value()))
                        changed = true;
                });
                ImGui::TreePop();
            }
            return changed;
        }
        else
        {
            // Scalar or opaque type — show read-only placeholder
            ImGui::LabelText(label, "(opaque)");
            return false;
        }
    }
};

} // namespace duin
