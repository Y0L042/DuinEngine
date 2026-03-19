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

    if (ImGui::BeginMenuBar())
    {
        if (ImGui::Button("Add Entity"))
        {
            if (selectedEntityId != 0 && !cachedActiveScene.expired())
            {
                std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
                duin::Entity e(selectedEntityId, activeWorld.get());
                if (e.IsValid())
                {
                    onAddChildEntity.Emit(e);
                }
            }
            else
            {
                onAddEntity.Emit();
            }
        }
        ImGui::EndMenuBar();
    }

    for (const auto &node : entityTree)
    {
        DrawEntityNode(node);
    }

    // Drop-to-root zone: fills remaining space
    ImVec2 availableRegion = ImGui::GetContentRegionAvail();
    if (availableRegion.x < 20.f)
        availableRegion.x = 20.0f;
    if (availableRegion.y < 20.0f)
        availableRegion.y = 20.0f;
    ImGui::InvisibleButton("##DropToRoot", availableRegion);

    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_TREE_ENTITY"))
        {
            uint64_t childId = *(const uint64_t *)payload->Data;
            if (!cachedActiveScene.expired())
            {
                std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
                duin::Entity child(childId, activeWorld.get());
                if (child.IsValid())
                {
                    onReparentEntityToRoot.Emit(child);
                }
            }
        }
        ImGui::EndDragDropTarget();
    }

    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        selectedEntityId = 0;
    }

    ImGui::End();
}

void SceneTree::CacheActiveScene(std::weak_ptr<Scene> scene)
{
    cachedActiveScene = scene;
}

void SceneTree::DrawEntityNode(const EntityNode &node)
{
    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_DefaultOpen;

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

    // Single-click selection
    if (ImGui::IsItemClicked(ImGuiMouseButton_Left))
    {
        selectedEntityId = node.entityId;
        if (!cachedActiveScene.expired())
        {
            std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
            duin::Entity e(node.entityId, activeWorld.get());
            if (e.IsValid())
            {
                onSelectEntity.Emit(e);
            }
        }
    }

    // Right-click context menu
    std::string popupId = "EntityContextMenu##" + std::to_string(node.entityId);
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right))
    {
        ImGui::OpenPopup(popupId.c_str());
    }
    if (ImGui::BeginPopup(popupId.c_str()))
    {
        if (ImGui::MenuItem("Add Child Entity"))
        {
            if (!cachedActiveScene.expired())
            {
                std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
                duin::Entity e(node.entityId, activeWorld.get());
                if (e.IsValid())
                {
                    onAddChildEntity.Emit(e);
                }
            }
        }
        if (ImGui::MenuItem("Delete Entity"))
        {
            if (!cachedActiveScene.expired())
            {
                std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
                duin::Entity e(node.entityId, activeWorld.get());
                if (e.IsValid())
                {
                    onRemoveEntity.Emit(e);
                }
            }
        }
        ImGui::EndPopup();
    }

    // Drag source
    if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_SourceAllowNullID))
    {
        uint64_t entityId = node.entityId;
        ImGui::SetDragDropPayload("SCENE_TREE_ENTITY", &entityId, sizeof(uint64_t));
        ImGui::Text("%s", displayName.c_str());
        ImGui::EndDragDropSource();
    }

    // Drop target (reparent)
    if (ImGui::BeginDragDropTarget())
    {
        if (const ImGuiPayload *payload = ImGui::AcceptDragDropPayload("SCENE_TREE_ENTITY"))
        {
            uint64_t childId = *(const uint64_t *)payload->Data;
            if (childId != node.entityId && !cachedActiveScene.expired())
            {
                std::shared_ptr<EditorWorld> activeWorld = cachedActiveScene.lock()->ctx.editorWorld;
                duin::Entity child(childId, activeWorld.get());
                duin::Entity newParent(node.entityId, activeWorld.get());
                if (child.IsValid() && newParent.IsValid())
                {
                    onReparentEntity.Emit(child, newParent);
                }
            }
        }
        ImGui::EndDragDropTarget();
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
    if (cachedActiveScene.expired())
        return;
    std::shared_ptr<EditorWorld> world_ = cachedActiveScene.lock()->ctx.editorWorld;
    if (!world_)
    {
        DN_WARN("EditorWorld is null!");
        return;
    }
    // Get all root entities
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
    for (duin::Entity &e : entities)
    {
        entityTree.emplace_back(UpdateChild(e));
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
