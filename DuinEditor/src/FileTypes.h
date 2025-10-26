#pragma once
#include <string>


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

ArcheType GetArcheType(const std::string& path);
FileType GetFileType(const std::string& extension);

struct FileExtension
{
    std::string extension;
    FileType type;
};

extern const std::array<FileExtension, 48> AllExtensions;