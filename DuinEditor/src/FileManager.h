#pragma once

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

typedef enum ArcheType {
    P_DIRECTORY,
    P_FILE,
    P_INVALID
} ArcheType;

typedef enum FileType {
    INVALID_EXT,
    IMAGE_EXT,
    AUDIO_EXT,
    VIDEO_EXT,
    MODEL_EXT,
    TEXT_EXT
} FileType;



class FSNode
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
