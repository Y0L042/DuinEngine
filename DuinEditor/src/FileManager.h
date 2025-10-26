#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include "FileTypes.h"

namespace fs = std::filesystem;



struct FSNode
{
public:
    ArcheType type;
    std::string path;

    FileType fileType;
    std::string name;
    std::string fileExtension;

    std::vector<FSNode*> subNodes;

    FSNode() = default;
    FSNode(std::string path);

    void Traverse();
    void SetFileType();

private:
};

class FileManager
{
public:
    static FileManager& Get();
	FileManager() = default;
    FileManager(std::string rootPath);

    void BuildFileSystemTree();
	void SetRootPath(const std::string& rootPath);
    FSNode& GetRootNode();
    void PrintTree();

private:
    fs::path rootPath;
    FSNode rootNode;
};
