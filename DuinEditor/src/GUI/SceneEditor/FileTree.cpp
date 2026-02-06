#include "FileTree.h"
#include "EditorStates/State_SceneEditor.h"
#include "FileManager.h"

FileTree::FileTree()
{
}

FileTree::FileTree(State_SceneEditor *sceneEditor) : sceneEditor(sceneEditor)
{
}

FileTree::~FileTree()
{
}

void FileTree::SetSceneEditor(State_SceneEditor *sceneEditor)
{
    this->sceneEditor = sceneEditor;
}

void FileTree::SetFileManager(FileManager *fm)
{
    this->fm = fm;
}

void FileTree::Init()
{
    imguiWindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoCollapse;
    if (sceneEditor != nullptr)
    {
        sceneEditor->onUpdateFileManager.Connect([&](FileManager *fm) { SetFileManager(fm); });
    }
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
    if (fm != nullptr)
    {
        DrawNode(&fm->GetRootNode(), fm->GetRootNode().name);
    }

    ImGui::End();
}

void FileTree::DrawNode(FSNode *node, const std::string &nodeLabel)
{
    if (node == nullptr)
    {
        return;
    }

    ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (node == selectedNode)
        flags |= ImGuiTreeNodeFlags_Selected;

    if (node->type == ArcheType::P_DIRECTORY)
    {
        // Open a tree node
        bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), flags);

        if (ImGui::IsItemClicked())
        {
            selectedNode = node;
        }

        if (nodeOpen)
        {
            for (FSNode *child : node->subNodes)
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
            selectedNode = node;
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {
            selectedNode = node;
            if (node->type == ArcheType::P_FILE && node->fileExt == FILEEXT_ECST)
            {
                onSceneSelect.Emit(node);
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {
            selectedNode = node;
        }
    }
}