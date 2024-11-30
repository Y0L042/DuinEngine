#include "FileManager.h"

#include <memory>
#include <string>
#include <array>
#include <iostream>


struct FileExtension 
{
    std::string extension;
    FileType type;
};

const std::array<FileExtension, 48> AllExtensions = {{
        // Image Extensions
        {".png", IMAGE_EXT},
        {".jpg", IMAGE_EXT},
        {".jpeg", IMAGE_EXT},
        {".bmp", IMAGE_EXT},
        {".tga", IMAGE_EXT},
        {".gif", IMAGE_EXT},
        {".hdr", IMAGE_EXT},
        {".tiff", IMAGE_EXT},
        {".dds", IMAGE_EXT},
        {".ico", IMAGE_EXT},
        {".svg", IMAGE_EXT},

        // Audio Extensions
        {".wav", AUDIO_EXT},
        {".mp3", AUDIO_EXT},
        {".ogg", AUDIO_EXT},
        {".flac", AUDIO_EXT},
        {".aac", AUDIO_EXT},
        {".wma", AUDIO_EXT},
        {".aiff", AUDIO_EXT},
        {".alac", AUDIO_EXT},

        // Video Extensions
        {".mp4", VIDEO_EXT},
        {".avi", VIDEO_EXT},
        {".mov", VIDEO_EXT},
        {".wmv", VIDEO_EXT},
        {".mkv", VIDEO_EXT},
        {".flv", VIDEO_EXT},
        {".webm", VIDEO_EXT},
        {".mpeg", VIDEO_EXT},

        // Model Extensions
        {".fbx", MODEL_EXT},
        {".obj", MODEL_EXT},
        {".dae", MODEL_EXT},
        {".3ds", MODEL_EXT},
        {".blend", MODEL_EXT},
        {".gltf", MODEL_EXT},
        {".glb", MODEL_EXT},
        {".max", MODEL_EXT},
        {".mb", MODEL_EXT},
        {".stl", MODEL_EXT},

        // Text Extensions
        {".txt", TEXT_EXT},
        {".json", TEXT_EXT},
        {".xml", TEXT_EXT},
        {".lua", TEXT_EXT},
        {".csv", TEXT_EXT},
        {".md", TEXT_EXT},
        {".ini", TEXT_EXT},
        {".cfg", TEXT_EXT},
        {".shader", TEXT_EXT},
        {".vert", TEXT_EXT},
        {".frag", TEXT_EXT}
    }};


FSNode::FSNode(std::string path)
    : path(path) 
{
    if (fs::is_directory(path)) {
        type = ArcheType::P_DIRECTORY;
        name = fs::path(path).filename().string();
    } else {
        type = ArcheType::P_FILE;
        IdentifyFile();
    }
}

void FSNode::Traverse()
{
    if (fs::is_empty(path)) { return; }

    for (const auto& dirEntry : fs::directory_iterator(path)) {
        std::cout << dirEntry.path() << "\n";

        FSNode *child = new FSNode(dirEntry.path().string());
        subNodes.push_back(child);
        if (child->type == ArcheType::P_DIRECTORY) {
            child->Traverse();
        }
    }
}

void FSNode::IdentifyFile()
{
    name = std::string(::GetFileName(path.c_str()));
    fileExtension = std::string(::GetFileExtension(name.c_str()));
    for (const FileExtension& type : AllExtensions) {
        if (fileExtension.compare(type.extension) == 0) {
            fileType = type.type;
            break;
        }
    }
}




FileManager::FileManager(std::string rootPath)
    : rootPath(rootPath), rootNode(rootPath)
{}

void FileManager::BuildFileSystemTree()
{
    if (rootPath.empty()) { 
        DN_WARN("Project root path invalid!");
        return; 
    }

    rootNode = FSNode(rootPath.string());
    if (rootNode.type == ArcheType::P_DIRECTORY) {
        rootNode.Traverse();
    }
}

void FileManager::DrawGUI()
{
    ImGui::Begin("File System Tree");
    DrawNode(&rootNode, rootNode.name);
    ImGui::End();
}

void FileManager::DrawNode(FSNode* node, const std::string& nodeLabel)
{
    if (node->type == ArcheType::P_DIRECTORY) {
        // Open a tree node
        bool nodeOpen = ImGui::TreeNodeEx(nodeLabel.c_str(), 
                                          ImGuiTreeNodeFlags_OpenOnArrow 
                                          | ImGuiTreeNodeFlags_OpenOnDoubleClick);
        if (nodeOpen) {
            for (FSNode* child : node->subNodes) {
                DrawNode(child, child->name);
            }
            ImGui::TreePop();
        }
    }
    else {
        // Render as leaf node
        ImGui::BulletText("%s", nodeLabel.c_str());
    }
}

static void RecurseTree(FSNode *node);
void FileManager::PrintTree()
{
    std::string depth = "";
    DN_INFO("File Tree:");
    DN_INFO("{}", rootNode.name);
    RecurseTree(&rootNode);
}
static void RecurseTree(FSNode *node)
{
    for (FSNode *nodePtr : node->subNodes) {
        if (nodePtr->type == ArcheType::P_DIRECTORY) {
            DN_INFO("{}/", nodePtr->path);
            RecurseTree(nodePtr);
        } else {
            DN_INFO(" - {}", nodePtr->name);
            RecurseTree(nodePtr);
        }
    }
}

