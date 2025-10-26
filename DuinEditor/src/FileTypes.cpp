#include "FileTypes.h"
#include <string>
#include <array>

const std::array<FileExtension, 48> AllExtensions = { {
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
    } };

ArcheType GetArcheType(const std::string& path)
{
    return ArcheType();
}

FileType GetFileType(const std::string& extension)
{
    return FileType();
}
