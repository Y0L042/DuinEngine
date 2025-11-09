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

typedef enum FileExt {
    FILEEXT_NULL,

    // Image Extensions
    FILEEXT_PNG,
    FILEEXT_JPG,
    FILEEXT_JPEG,
    FILEEXT_BMP,
    FILEEXT_TGA,
    FILEEXT_GIF,
    FILEEXT_HDR,
    FILEEXT_TIFF,
    FILEEXT_DDS,
    FILEEXT_ICO,
    FILEEXT_SVG,

    // Audio Extensions
    FILEEXT_WAV,
    FILEEXT_MP3,
    FILEEXT_OGG,
    FILEEXT_FLAC,
    FILEEXT_AAC,
    FILEEXT_WMA,
    FILEEXT_AIFF,
    FILEEXT_ALAC,

    // Video Extensions
    FILEEXT_MP4,
    FILEEXT_AVI,
    FILEEXT_MOV,
    FILEEXT_WMV,
    FILEEXT_MKV,
    FILEEXT_FLV,
    FILEEXT_WEBM,
    FILEEXT_MPEG,

    // Model Extensions
    FILEEXT_FBX,
    FILEEXT_OBJ,
    FILEEXT_DAE,
    FILEEXT_3DS,
    FILEEXT_BLEND,
    FILEEXT_GLTF,
    FILEEXT_GLB,
    FILEEXT_MAX,
    FILEEXT_MB,
    FILEEXT_STL,

    // Text Extensions
    FILEEXT_TXT,
    FILEEXT_JSON,
    FILEEXT_XML,
    FILEEXT_LUA,
    FILEEXT_CSV,
    FILEEXT_MD,
    FILEEXT_INI,
    FILEEXT_CFG,
    FILEEXT_SHADER,
    FILEEXT_VERT,
    FILEEXT_FRAG,
    FILEEXT_ECST
} FileExt;

ArcheType GetArcheType(const std::string& path);
FileType GetFileType(const std::string& extension);

struct FileExtension
{
    std::string extension;
    FileType type;
    FileExt ext;
};

extern const std::array<FileExtension, 49> AllExtensions;