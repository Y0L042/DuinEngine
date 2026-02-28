#include "FileTree.h"
#include "EditorStates/State_SceneEditor.h"
#include "FileManager.h"

FileTree::FileTree()
{
}

FileTree::~FileTree()
{
}

void FileTree::SetFileManager(std::shared_ptr<FileManager> fm)
{
    this->fm = fm;
}

void FileTree::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
}

void FileTree::PhysicsUpdate(double delta)
{
}

void FileTree::Draw()
{
}

void FileTree::DrawUI()
{
    ImGui::Begin("FileTree", nullptr, imguiWindowFlags);
    if (fm != nullptr && !fm->GetRootNode().expired())
    {
        DrawNode(fm->GetRootNode(), fm->GetRootNode().lock()->name);
    }

    ImGui::End();
}

void FileTree::DrawNode(std::weak_ptr<FSNode> node, const std::string &nodeLabel)
{
    if (node.expired())
    {
        return;
    }
    std::shared_ptr<FSNode> node_ = node.lock();

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (!selectedNode.expired() && node_ == selectedNode.lock())
        flags |= ImGuiTreeNodeFlags_Selected;

    if (node_->type == ArcheType::P_DIRECTORY)
    {
        // Open a tree node
        bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            selectedNode = node_;
        }

        if (nodeOpen)
        {
            for (auto& child : node_->subNodes)
            {
                DrawNode(child, child->name);
            }
            ImGui::TreePop();
        }
    }
    else
    {
        ImGuiTreeNodeFlags leafFlags = flags | ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
        ImGui::TreeNodeEx(nodeLabel.c_str(), leafFlags);
        if (ImGui::IsItemClicked())
        {
            selectedNode = node_;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            selectedNode = node_;
            if (node_->type == ArcheType::P_FILE && (node_->fileExt == FILEEXT_ECST || node_->fileExt == FILEEXT_JSON))
            {
                onSceneSelect.Emit(selectedNode);
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {
            selectedNode = node_;
        }
    }
}