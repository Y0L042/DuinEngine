#pragma once
#include <string>

namespace duin
{

typedef enum ArcheType
{
    FS_ARCHETYPE_DIRECTORY,
    FS_ARCHETYPE_FILE,
    FS_ARCHETYPE_INVALID
} ArcheType;

typedef enum FileType
{
    FS_FILETYPE_INVALID_EXT,
    FS_FILETYPE_IMAGE_EXT,
    FS_FILETYPE_AUDIO_EXT,
    FS_FILETYPE_VIDEO_EXT,
    FS_FILETYPE_MODEL_EXT,
    FS_FILETYPE_TEXT_EXT,

    FS_FILETYPE_COUNT
} FileType;

typedef enum FileExt
{
    FS_FILEEXT_NULL,

    // Image Extensions
    FS_FILEEXT_PNG,
    FS_FILEEXT_JPG,
    FS_FILEEXT_JPEG,
    FS_FILEEXT_BMP,
    FS_FILEEXT_TGA,
    FS_FILEEXT_GIF,
    FS_FILEEXT_HDR,
    FS_FILEEXT_TIFF,
    FS_FILEEXT_DDS,
    FS_FILEEXT_ICO,
    FS_FILEEXT_SVG,

    // Audio Extensions
    FS_FILEEXT_WAV,
    FS_FILEEXT_MP3,
    FS_FILEEXT_OGG,
    FS_FILEEXT_FLAC,
    FS_FILEEXT_AAC,
    FS_FILEEXT_WMA,
    FS_FILEEXT_AIFF,
    FS_FILEEXT_ALAC,

    // Video Extensions
    FS_FILEEXT_MP4,
    FS_FILEEXT_AVI,
    FS_FILEEXT_MOV,
    FS_FILEEXT_WMV,
    FS_FILEEXT_MKV,
    FS_FILEEXT_FLV,
    FS_FILEEXT_WEBM,
    FS_FILEEXT_MPEG,

    // Model Extensions
    FS_FILEEXT_FBX,
    FS_FILEEXT_OBJ,
    FS_FILEEXT_DAE,
    FS_FILEEXT_3DS,
    FS_FILEEXT_BLEND,
    FS_FILEEXT_GLTF,
    FS_FILEEXT_GLB,
    FS_FILEEXT_MAX,
    FS_FILEEXT_MB,
    FS_FILEEXT_STL,

    // Text Extensions
    FS_FILEEXT_TXT,
    FS_FILEEXT_JSON,
    FS_FILEEXT_XML,
    FS_FILEEXT_LUA,
    FS_FILEEXT_CSV,
    FS_FILEEXT_MD,
    FS_FILEEXT_INI,
    FS_FILEEXT_CFG,
    FS_FILEEXT_SHADER,
    FS_FILEEXT_VERT,
    FS_FILEEXT_FRAG,
    FS_FILEEXT_ECST,

    FS_FILEEXT_COUNT
} FileExt;


struct FileExtension
{
    std::string extension;
    FileType type;
    FileExt ext;
};

struct PathInfo
{
    ArcheType archeType;
    FileExtension fileInfo;
};

PathInfo GetPathInfo(const std::string& path);

extern const std::array<FileExtension, 49> AllExtensions;
} // namespace duin