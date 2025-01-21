#pragma once

#include <Duin.h>

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

typedef enum ArcheType {
    P_DIRECTORY,
    P_FILE
};

typedef enum FileType {
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

    std::vector<FSNode *> subNodes;

    FSNode() = default;
    FSNode(std::string path);

    void Traverse();
    void IdentifyFile();

private:
};





class FileManager 
{
public:
    fs::path rootPath;

    FileManager() = default;
    FileManager(std::string rootPath);

    void BuildFileSystemTree();
    void DrawGUI();
    void DrawNode(FSNode* node, const std::string& nodeLabel);

    void PrintTree();

private:
    FSNode rootNode;
};

