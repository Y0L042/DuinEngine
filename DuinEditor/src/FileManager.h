#pragma once

#include <vector>
#include <string>
#include <filesystem>
#include <Duin/IO/IOModule.h>
#include "FileTypes.h"

namespace fs = std::filesystem;

struct FSNode
{
  public:
    ArcheType type;
    std::string path;

    FileType fileType;
    FileExt fileExt;
    std::string name;
    std::string fileExtension;

    std::vector<std::unique_ptr<FSNode>> subNodes;

    FSNode() = default;
    FSNode(std::string path);

    void SetFileType();
    void Recurse();

  private:
    friend class FileManager;
    void Traverse();
};

class FileManager
{
  public:
    static FileManager &Get();
    FileManager() = default;
    FileManager(std::string rootPath);

    void BuildFileSystemTree();
    void SetRootPath(const std::string &rootPath);
    FSNode &GetRootNode();
    void PrintTree();

    std::vector<std::string> GetFilesByExt(const std::string &ext);

  private:
    fs::path rootPath;
    FSNode rootNode;
};
