#include "SceneTree.h"
#include "EditorStates/State_SceneEditor.h"
#include <Duin/Scene/SceneBuilder.h>
#include <memory>
#include <external/imgui.h>

SceneTree::SceneTree()
{
}

SceneTree::~SceneTree()
{
}

void SceneTree::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;

    if (GetParent().get() != nullptr)
    {
        sceneEditor = dynamic_pointer_cast<State_SceneEditor>(GetParent());
        sceneEditor->onSetActiveScene.Connect([&](duin::PackedScene &pscn) {});
    }
}

void SceneTree::PhysicsUpdate(double delta)
{
}

void SceneTree::Draw()
{
}

void SceneTree::DrawUI()
{
    ImGui::Begin("SceneTree", nullptr, imguiWindowFlags);

    for (const auto &node : entityTree)
    {
        DrawEntityNode(node);
    }

    ImGui::End();
}

void SceneTree::DrawEntityNode(const EntityNode &node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;

    std::string displayName = node.entityName.empty() ? "Unnamed Entity" : node.entityName;
    std::string nodeId = displayName + "##" + std::to_string(reinterpret_cast<uintptr_t>(&node));

    if (node.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", displayName.c_str());
    }
    else
    {
        bool nodeOpen = ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", displayName.c_str());

        if (nodeOpen)
        {
            for (const auto &child : node.children)
            {
                DrawEntityNode(child);
            }
            ImGui::TreePop();
        }
    }
}

void SceneTree::UpdateTree(duin::World *world)
{
     std::vector<duin::Entity> vec;
     world->IterateChildren([&](duin::Entity e) {
         //if (!e.HasPair(flecs::ChildOf, flecs::Wildcard))
         if (!e.GetParent().IsValid())
         {
             DN_INFO("Entity {}, parent {}", e.GetID(), e.GetParent().GetID());
             vec.push_back(e);
         }
     });
     UpdateTreeImpl(vec);

    //entityTree.clear();
    //duin::Entity plr = world->Lookup("Player");
    //for (duin::Entity &child : plr.GetChildren())
    //{
    //    entityTree.emplace_back(UpdateChild(child));
    //}
}

void SceneTree::UpdateTreeImpl(std::vector<duin::Entity> entities)
{
     entityTree.clear();
     for (duin::Entity &child : entities)
    {
         entityTree.emplace_back(UpdateChild(child));
     }
}

SceneTree::EntityNode SceneTree::UpdateChild(duin::Entity &child)
{
    EntityNode node;
    node.entityName = child.GetName();
    for (duin::Entity &child : child.GetChildren())
    {
        node.children.emplace_back(UpdateChild(child));
    }
    return node;
}
