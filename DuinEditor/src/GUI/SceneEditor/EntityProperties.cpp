#include "EntityProperties.h"
#include <memory>
#include <EditorStates/State_SceneEditor.h>
#include <Duin/ECS/GameWorld.h>
#include <Duin/ECS/ComponentInspector.h>
#include <Duin/Core/Maths/DuinMaths.h>
#include <flecs.h>

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

        auto &inspector = duin::ComponentInspector::Get();

        focusedEntity.ForEachComponent([&](duin::Entity::ID id) {
            if (id.IsPair())
                return;
            if (!id.IsEntity())
                return;

            duin::Entity cmpType = id.GetEntity();
            std::string typeName = cmpType.GetName();

            ImGui::PushID(static_cast<int>(cmpType.GetID()));
            if (ImGui::CollapsingHeader(typeName.c_str(), ImGuiTreeNodeFlags_DefaultOpen))
            {
                // Get mutable pointer to the component data via the flecs C API.
                // Returns null for zero-size types (tags).
                void *rawPtr = nullptr;
                if (!cmpType.IsTag())
                {
                    rawPtr = focusedEntity.TryGetMut(id);
                }

                if (!inspector.Inspect(typeName, rawPtr, focusedEntity))
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

void EntityProperties::SetFocusedEntity(duin::Entity entity)
{
    if (!entity.IsValid())
    {
        DN_FATAL("Entity {} is not valid!", entity.GetID());
    }
    focusedEntity = entity;
}