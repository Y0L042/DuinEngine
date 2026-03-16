#include "EntityProperties.h"
#include <memory>
#include <EditorStates/State_SceneEditor.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <flecs.h>
#include <rfl.hpp>
#include <rfl/to_view.hpp>
#include "CommandManager.h"
#include "EditorCommand.h"
#include <Duin/Core/Debug/DNLog.h>

EntityProperties::EntityProperties()
{
}

EntityProperties::~EntityProperties()
{
}

void EntityProperties::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
}

void EntityProperties::PhysicsUpdate(double delta)
{
}

void EntityProperties::Draw()
{
}

void EntityProperties::DrawUI()
{
    ImGui::Begin("Entity Properties", nullptr, imguiWindowFlags);

    if (focusedEntity.IsValid())
    {
        ImGui::SeparatorText(focusedEntity.GetName().c_str());

        if (ImGui::BeginMenu("Add Component"))
        {
            ImGui::BeginChild("Add New Component", {200, 200});
            ImGui::MenuItem("(demo menu)", NULL, false, false);
            for (const std::string component : duin::ComponentSerializer::Get().GetAllComponents())
            {
                if (ImGui::MenuItem(component.c_str()))
                {
                    auto cm = commandManager.lock();
                    if (cm)
                    {
                        duin::Entity entity = focusedEntity;
                        auto command = std::make_shared<EditorCommand>();
                        command->Do = [entity, component]() mutable {
                            duin::ComponentSerializer::Get().SetComponentByString(entity, component);
                        };
                        command->Undo = [entity, component]() mutable {
                            duin::ComponentSerializer::Get().RemoveComponentByString(entity, component);
                        };
                        cm->Do(command);
                    }
                }
            }
            ImGui::EndChild();
            ImGui::EndMenu();
        }

        focusedEntity.ForEachComponent([&](duin::Entity::ID id) {
            if (id.IsPair())
                return;
            if (!id.IsEntity())
                return;

            duin::Entity cmpType = id.GetEntity();
            std::string typeName = cmpType.GetName();

            ImGui::PushID(static_cast<int>(cmpType.GetID()));
            bool headerOpen = ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_AllowOverlap);

            // "X" remove button, right-aligned on the same line as the header
            ImGui::SameLine(ImGui::GetWindowContentRegionMax().x - 20.0f);
            if (ImGui::SmallButton("X"))
            {
                DN_INFO("Delete Component {}", typeName);
                if (auto cm = commandManager.lock())
                {
                    duin::Entity entity = focusedEntity;
                    std::string component = typeName;
                    auto cmd = std::make_shared<EditorCommand>();
                    cmd->Do = [entity, component]() mutable {
                        duin::ComponentSerializer::Get().RemoveComponentByString(entity, component);
                    };
                    cmd->Undo = [entity, component]() mutable {
                        duin::ComponentSerializer::Get().SetComponentByString(entity, component);
                    };
                    cm->Do(cmd);
                }
            }

            if (headerOpen)
            {
                void *rawPtr = nullptr;
                if (!cmpType.IsTag())
                {
                    rawPtr = focusedEntity.TryGetMut(id);
                }

                if (!DrawComponent(typeName, rawPtr))
                {
                    ImGui::TextDisabled("(no inspector)");
                }
            }
            ImGui::PopID();
        });
    }
    else
    {
        ImGui::TextDisabled("No entity selected");
    }

    ImGui::End();
}

bool EntityProperties::AddComponent()
{
    return false;
}

bool EntityProperties::RemoveComponent()
{
    return false;
}

bool EntityProperties::DrawComponent(const std::string &typeName, void *rawPtr)
{
    if (!rawPtr)
    {
        ImGui::TextDisabled("(tag)");
        return true;
    }

    // Dispatch to typed component renderers
    if (typeName == "Position2D")
        return DrawComponentTyped<duin::ECSComponent::Position2D>(rawPtr);
    else if (typeName == "Rotation2D")
        return DrawComponentTyped<duin::ECSComponent::Rotation2D>(rawPtr);
    else if (typeName == "Scale2D")
        return DrawComponentTyped<duin::ECSComponent::Scale2D>(rawPtr);
    else if (typeName == "Velocity2D")
        return DrawComponentTyped<duin::ECSComponent::Velocity2D>(rawPtr);
    else if (typeName == "Transform3D")
        return DrawComponentTyped<duin::ECSComponent::Transform3D>(rawPtr);
    else if (typeName == "Position3D")
        return DrawComponentTyped<duin::ECSComponent::Position3D>(rawPtr);
    else if (typeName == "Rotation3D")
        return DrawComponentTyped<duin::ECSComponent::Rotation3D>(rawPtr);
    else if (typeName == "Scale3D")
        return DrawComponentTyped<duin::ECSComponent::Scale3D>(rawPtr);
    else if (typeName == "Velocity3D")
        return DrawComponentTyped<duin::ECSComponent::Velocity3D>(rawPtr);
    // Add more component types as needed

    return false;
}

template <typename T>
bool EntityProperties::DrawComponentTyped(void *rawPtr)
{
    if (!rawPtr)
        return false;

    T *comp = static_cast<T *>(rawPtr);

    if constexpr (requires { typename T::ReflectionType; })
    {
        typename T::ReflectionType impl = comp->reflection();
        bool anyChanged = false;
        bool anyActivated = false;
        bool anyDeactivated = false;

        auto view = rfl::to_view(impl);
        view.apply([&](const auto &field) {
            FieldEditState state = DrawField(field.name(), *field.value());
            anyChanged |= state.changed;
            anyActivated |= state.activated;
            anyDeactivated |= state.deactivatedAfterEdit;
        });

        if (anyActivated)
        {
            pendingOldValue = *comp;
            pendingEditActive = true;
        }

        if (anyChanged)
        {
            *comp = T(impl);
            focusedEntity.Modified<T>();
        }

        if (anyDeactivated && pendingEditActive)
        {
            if (auto mgr = commandManager.lock())
            {
                T oldVal = std::any_cast<T>(pendingOldValue);
                T newVal = *comp;
                duin::Entity entity = focusedEntity;

                auto cmd = std::make_shared<EditorCommand>();
                cmd->Do = [entity, newVal]() mutable {
                    if (!entity.IsValid())
                        return;
                    entity.Set<T>(newVal);
                    entity.Modified<T>();
                };
                cmd->Undo = [entity, oldVal]() mutable {
                    if (!entity.IsValid())
                        return;
                    entity.Set<T>(oldVal);
                    entity.Modified<T>();
                };
                mgr->Record(cmd);
            }
            pendingEditActive = false;
        }

        return true;
    }
    else if constexpr (std::is_aggregate_v<T> && std::is_class_v<T>)
    {
        bool anyChanged = false;
        bool anyActivated = false;
        bool anyDeactivated = false;

        auto view = rfl::to_view(*comp);
        view.apply([&](const auto &field) {
            FieldEditState state = DrawField(field.name(), *field.value());
            anyChanged |= state.changed;
            anyActivated |= state.activated;
            anyDeactivated |= state.deactivatedAfterEdit;
        });

        if (anyActivated)
        {
            pendingOldValue = *comp;
            pendingEditActive = true;
        }

        if (anyChanged)
        {
            focusedEntity.Modified<T>();
        }

        if (anyDeactivated && pendingEditActive)
        {
            if (auto mgr = commandManager.lock())
            {
                T oldVal = std::any_cast<T>(pendingOldValue);
                T newVal = *comp;
                duin::Entity entity = focusedEntity;

                auto cmd = std::make_shared<EditorCommand>();
                cmd->Do = [entity, newVal]() mutable {
                    if (!entity.IsValid())
                        return;
                    entity.Set<T>(newVal);
                    entity.Modified<T>();
                };
                cmd->Undo = [entity, oldVal]() mutable {
                    if (!entity.IsValid())
                        return;
                    entity.Set<T>(oldVal);
                    entity.Modified<T>();
                };
                mgr->Record(cmd);
            }
            pendingEditActive = false;
        }

        return true;
    }

    return false;
}

template <typename T>
FieldEditState EntityProperties::DrawField(std::string_view name, T &value)
{
    char label[128];
    std::snprintf(label, sizeof(label), "%.*s", static_cast<int>(name.size()), name.data());

    if constexpr (std::is_same_v<T, float>)
    {
        FieldEditState state;
        state.changed = ImGui::DragFloat(label, &value, 0.05f);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (std::is_same_v<T, double>)
    {
        float v = static_cast<float>(value);
        FieldEditState state;
        state.changed = ImGui::DragFloat(label, &v, 0.05f);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        if (state.changed)
            value = static_cast<double>(v);
        return state;
    }
    else if constexpr (std::is_same_v<T, int> || std::is_same_v<T, int32_t>)
    {
        FieldEditState state;
        state.changed = ImGui::DragInt(label, &value);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (std::is_same_v<T, uint32_t>)
    {
        int v = static_cast<int>(value);
        FieldEditState state;
        state.changed = ImGui::DragInt(label, &v, 1, 0, INT_MAX);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        if (state.changed)
            value = static_cast<uint32_t>(v);
        return state;
    }
    else if constexpr (std::is_same_v<T, bool>)
    {
        FieldEditState state;
        state.changed = ImGui::Checkbox(label, &value);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (std::is_same_v<T, std::string>)
    {
        char buf[256];
        std::strncpy(buf, value.c_str(), sizeof(buf) - 1);
        buf[sizeof(buf) - 1] = '\0';
        FieldEditState state;
        state.changed = ImGui::InputText(label, buf, sizeof(buf));
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        if (state.changed)
            value = buf;
        return state;
    }
    else if constexpr (std::is_same_v<T, duin::Vector2>)
    {
        FieldEditState state;
        state.changed = ImGui::DragFloat2(label, &value.x, 0.05f);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (std::is_same_v<T, duin::Vector3>)
    {
        FieldEditState state;
        state.changed = ImGui::DragFloat3(label, &value.x, 0.05f);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (std::is_same_v<T, duin::Vector4>)
    {
        FieldEditState state;
        state.changed = ImGui::DragFloat4(label, &value.x, 0.05f);
        state.activated = ImGui::IsItemActivated();
        state.deactivatedAfterEdit = ImGui::IsItemDeactivatedAfterEdit();
        return state;
    }
    else if constexpr (requires { typename T::ReflectionType; })
    {
        FieldEditState state;
        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
        {
            typename T::ReflectionType impl = value.reflection();
            auto view = rfl::to_view(impl);
            view.apply([&state](const auto &f) {
                FieldEditState child = DrawField(f.name(), *f.value());
                state.changed |= child.changed;
                state.activated |= child.activated;
                state.deactivatedAfterEdit |= child.deactivatedAfterEdit;
            });
            if (state.changed)
                value = T(impl);
            ImGui::TreePop();
        }
        return state;
    }
    else if constexpr (std::is_aggregate_v<T> && std::is_class_v<T>)
    {
        FieldEditState state;
        if (ImGui::TreeNodeEx(label, ImGuiTreeNodeFlags_DefaultOpen))
        {
            auto view = rfl::to_view(value);
            view.apply([&state](const auto &f) {
                FieldEditState child = DrawField(f.name(), *f.value());
                state.changed |= child.changed;
                state.activated |= child.activated;
                state.deactivatedAfterEdit |= child.deactivatedAfterEdit;
            });
            ImGui::TreePop();
        }
        return state;
    }
    else
    {
        ImGui::LabelText(label, "(opaque)");
        return {};
    }
}

void EntityProperties::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    cachedActiveScene = scene;
}

void EntityProperties::SetFocusedEntity(duin::Entity entity)
{
    if (!entity.IsValid())
    {
        DN_FATAL("Entity {} is not valid!", entity.GetID());
    }
    focusedEntity = entity;
}

void EntityProperties::SetCommandManager(std::weak_ptr<CommandManager> mgr)
{
    commandManager = mgr;
}