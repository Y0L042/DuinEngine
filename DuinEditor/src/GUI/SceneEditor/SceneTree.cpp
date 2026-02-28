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
}

void SceneTree::PhysicsUpdate(double delta)
{
    UpdateTree();
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

void SceneTree::SetActiveWorld(std::weak_ptr<duin::World> world)
{
    activeWorld = world;
}

void SceneTree::DrawEntityNode(const EntityNode &node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow;

    if (node.entityId == selectedEntityId)
        flags |= ImGuiTreeNodeFlags_Selected;

    std::string displayName = node.entityName.empty() ? "Unnamed Entity" : node.entityName;
    std::string nodeId = displayName + "##" + std::to_string(node.entityId);

    bool nodeOpen = false;
    if (node.children.empty())
    {
        flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", displayName.c_str());
    }
    else
    {
        nodeOpen = ImGui::TreeNodeEx(nodeId.c_str(), flags, "%s", displayName.c_str());
    }

    if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
    {
        selectedEntityId = node.entityId;
        if (!activeWorld.expired() && activeWorld.lock()->IsAlive(node.entityId))
        {
            duin::Entity e(node.entityId, activeWorld.lock().get());
            if (!e.IsValid())
            {
                DN_FATAL("Entity {} is not valid!", e.GetID());
            }
            onSelectEntity.Emit(e);
            DN_INFO("Entity {} [{}] double-clicked.", e.GetName(), e.GetID());
        }
    }

    if (nodeOpen)
    {
        for (const auto &child : node.children)
        {
            DrawEntityNode(child);
        }
        ImGui::TreePop();
    }
}

void SceneTree::UpdateTree()
{
    if (activeWorld.expired()) return;
    std::shared_ptr<duin::World> world_ = activeWorld.lock();
    std::vector<duin::Entity> vec;
    world_->IterateChildren([&](duin::Entity e) {
        if (!e.GetParent().IsValid() && !e.Has(flecs::Prefab) && (e.GetName() != ""))
        {
            vec.push_back(e);
        }
    });
    UpdateTreeImpl(vec);
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
    node.entityId = child.GetID();
    node.entityName = child.GetName();
    for (duin::Entity &child : child.GetChildren())
    {
        node.children.emplace_back(UpdateChild(child));
    }
    return node;
}
