#include "EntityProperties.h"
#include <memory>
#include <EditorStates/State_SceneEditor.h>

EntityProperties::EntityProperties()
{
}

EntityProperties::~EntityProperties()
{
}

void EntityProperties::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

    if (!GetParent().expired())
    {
        sceneEditor = dynamic_pointer_cast<State_SceneEditor>(GetParent().lock());
    }
    sceneEditor->onSelectEntity.Connect([&](duin::Entity e) { SetFocusedEntity(e); });
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
        auto& e = focusedEntity;
        ImGui::SeparatorText(e.GetName().c_str());

        componentNodeList.clear();
        e.ForEachComponent([&](duin::Entity::ID id) {
            if (!id.IsEntity())
            {
                return;
            }
            duin::Entity cmp = id.GetEntity();
            ComponentNode n{.cmpID = cmp.GetID(), .cmpName = cmp.GetName()};
            componentNodeList.push_back(n);
        });

        for (const auto &node : componentNodeList)
        {
            ImGui::Text("%s", node.cmpName.c_str());
        }
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