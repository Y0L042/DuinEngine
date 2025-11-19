#include "FileManager.h"

#include <Duin/Core/Debug/DebugModule.h>
#include <Duin/Core/Utils/UtilsModule.h>
#include <Duin/IO/Filesystem.h>
#include <SDL3/SDL.h>
#include <external/imgui.h>

#include <array>
#include <iostream>
#include <memory>
#include <string>

FSNode::FSNode(std::string path) : path(path), fileType(FileType::INVALID_EXT), fileExt(FileExt::FILEEXT_NULL)
{
    if (fs::is_directory(path))
    {
        type = ArcheType::P_DIRECTORY;
        name = fs::path(path).filename().string();
    }
    else
    {
        type = ArcheType::P_FILE;
        SetFileType();
    }
}

void FSNode::Traverse()
{
    // if (fs::is_empty(path)) { return; }

    try
    {
        for (const auto &dirEntry : fs::directory_iterator(path))
        {
            std::cout << dirEntry.path() << "\n";

            FSNode *child = new FSNode(dirEntry.path().string());
            subNodes.push_back(child);
            if (child->type == ArcheType::P_DIRECTORY)
            {
                child->Traverse();
            }
        }
    }
    catch (const fs::filesystem_error &ex)
    {
        // Handle permission errors or other filesystem issues
        DN_ERROR("Error accessing directory: {} - {}", path, ex.what());
    }
}

void FSNode::SetFileType()
{
    name = std::string(::duin::fs::GetFileName(path));
    fileExtension = std::string(::duin::fs::GetFileExtension(name));
    for (const FileExtension &type : AllExtensions)
    {
        if (fileExtension.compare(type.extension) == 0)
        {
            fileType = type.type;
            fileExt = type.ext;
            break;
        }
    }
}

FileManager &FileManager::Get()
{
    static FileManager instance;
    return instance;
}

FileManager::FileManager(std::string rootPath) : rootPath(rootPath), rootNode(rootPath)
{
}

void FileManager::BuildFileSystemTree()
{
    if (rootPath.empty())
    {
        DN_WARN("Project root path invalid!");
        return;
    }

    rootNode = FSNode(rootPath.string());
    if (rootNode.type == ArcheType::P_DIRECTORY)
    {
        rootNode.Traverse();
    }
}

void FileManager::SetRootPath(const std::string &rootPath)
{
    this->rootPath = rootPath;
    rootNode = FSNode(this->rootPath.string());
}

FSNode &FileManager::GetRootNode()
{
    return rootNode;
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
    for (FSNode *nodePtr : node->subNodes)
    {
        if (nodePtr->type == ArcheType::P_DIRECTORY)
        {
            DN_INFO("{}/", nodePtr->path);
            RecurseTree(nodePtr);
        }
        else
        {
            DN_INFO(" - {}", nodePtr->name);
            RecurseTree(nodePtr);
        }
    }
}
