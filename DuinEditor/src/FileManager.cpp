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

#include "Singleton.h"

FSNode::FSNode(std::string path) : path(path), fileType(duin::FileType::FS_FILETYPE_INVALID_EXT), fileExt(duin::FileExt::FS_FILEEXT_NULL)
{
    if (fs::is_directory(path))
    {
        type = duin::ArcheType::FS_ARCHETYPE_DIRECTORY;
        name = fs::path(path).filename().string();
    }
    else
    {
        type = duin::ArcheType::FS_ARCHETYPE_FILE;
        name = fs::path(path).filename().string();
        SetFileType();
    }
}

void FSNode::Traverse()
{
    try
    {
        for (const auto &dirEntry : fs::directory_iterator(path))
        {
            subNodes.emplace_back(std::make_shared<FSNode>(dirEntry.path().string()));
            FSNode *child = subNodes.back().get();
            if (child->type == duin::ArcheType::FS_ARCHETYPE_DIRECTORY)
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
    fileExtension = std::string(::duin::fs::GetFileExtension(name));
    for (const duin::FileExtension &type : duin::AllExtensions)
    {
        if (fileExtension.compare(type.extension) == 0)
        {
            fileType = type.type;
            fileExt = type.ext;
            break;
        }
    }
}

void FSNode::Recurse()
{
}

FileManager &FileManager::Get()
{
    static FileManager instance;
    return instance;
}

FileManager::FileManager(std::string rootPath) : rootPath(rootPath), rootNode(std::make_shared<FSNode>(rootPath))
{
}

void FileManager::BuildFileSystemTree()
{
    if (rootPath.empty())
    {
        DN_WARN("Project root path invalid!");
        return;
    }

    rootNode = std::make_shared<FSNode>(rootPath.string());
    if (rootNode->type == duin::ArcheType::FS_ARCHETYPE_DIRECTORY)
    {
        rootNode->Traverse();
    }
}

void FileManager::WalkTree(FSNodeFN_ fn, std::shared_ptr<FSNode> currentNode)
{
    if (currentNode)
    {
        fn(currentNode);
        for (auto child : currentNode->subNodes)
        {
            WalkTree(fn, child);
        }
    }
    else
    {
        WalkTree(fn, rootNode);
    }
}

void FileManager::SetRootPath(const std::string &rootPath)
{
    this->rootPath = rootPath;
    rootNode = std::make_shared<FSNode>(this->rootPath.string());
}

//std::weak_ptr<FSNode> FileManager::GetRootNode()
//{
//    return rootNode;
//}

std::vector<std::string> FileManager::GetFilesByExt(const std::string &ext)
{
    int count = 0;
    char **array = duin::fs::GlobDirectory(Singleton::GetActiveProject().GetPathAsString(), ext, 0, &count);
    return std::vector<std::string>(array, array + count);
}

static void RecurseTreePrint(std::weak_ptr<FSNode> node);
void FileManager::PrintTree()
{
    std::string depth = "";
    DN_INFO("File Tree:");
    DN_INFO("{}", rootNode->name);
    RecurseTreePrint(rootNode);
}

static void RecurseTreePrint(std::weak_ptr<FSNode> node)
{
    if (node.expired()) return;

    for (auto& nodePtr : node.lock()->subNodes)
    {
        if (nodePtr->type == duin::ArcheType::FS_ARCHETYPE_DIRECTORY)
        {
            DN_INFO("{}/", nodePtr->path);
            RecurseTreePrint(nodePtr);
        }
        else
        {
            DN_INFO(" - {}", nodePtr->name);
            RecurseTreePrint(nodePtr);
        }
    }
}
