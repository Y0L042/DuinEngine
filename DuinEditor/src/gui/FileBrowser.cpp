#include "FileBrowser.h"
#include "PanelManager.h"
#include "FileManager.h"
#include "Singletons.h"

FileBrowser::FileBrowser(PanelManager *panelManager, const std::string& name)
    : Panel(panelManager, PanelType::FILEBROWSER, name)
{
	FileManager& fileManager = FileManager::Get();
    fileManager.BuildFileSystemTree();
}


void FileBrowser::ConnectOnFileSelected(std::function<void(FSNode *)> f)
{
    if (blackboard->signals->onFileSelect)
    {
	    blackboard->signals->onFileSelect->Connect(f);
    }
}

void FileBrowser::DrawContent()
{
	FileManager& fileManager = FileManager::Get();
    DrawNode(&fileManager.GetRootNode(), fileManager.GetRootNode().name);
}

void FileBrowser::DrawNode(FSNode* node, const std::string& nodeLabel)
{
	ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick;
    if (node == selectedNode) flags |= ImGuiTreeNodeFlags_Selected;

    if (node->type == ArcheType::P_DIRECTORY) 
    {
        // Open a tree node
        bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(),flags);

        if (ImGui::IsItemClicked()) 
        {
            selectedNode = node;
		}

        if (nodeOpen) 
        {
            for (FSNode* child : node->subNodes) 
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
            if (blackboard->signals->onFileSelect)
            {
                blackboard->signals->onFileSelect->Emit(selectedNode);
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(0))
        {                                                                                                                                                                                                                      
            selectedNode = node;
            if (blackboard->signals->onFileDoubleSelect)
            {
                blackboard->signals->onFileDoubleSelect->Emit(selectedNode);
            }
        }

        if (ImGui::IsItemHovered() && ImGui::IsMouseClicked(1))
        {                                                                                                                                                                                                                          
            selectedNode = node;
            if (blackboard->signals->onFileRightSelect)
            {
                blackboard->signals->onFileRightSelect->Emit(selectedNode);
            }
        }
    }
}