#include "dnpch.h"
#include "FileTypes.h"
#include <string>
#include <array>
#include "Duin/Core/Debug/DNLog.h"
#include <filesystem>
#include <algorithm>

namespace duin
{
const std::array<FileExtension, 50> AllExtensions = {{// Image Extensions
                                                      {"png", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_PNG},
                                                      {"jpg", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_JPG},
                                                      {"jpeg", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_JPEG},
                                                      {"bmp", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_BMP},
                                                      {"tga", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_TGA},
                                                      {"gif", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_GIF},
                                                      {"hdr", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_HDR},
                                                      {"tiff", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_TIFF},
                                                      {"dds", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_DDS},
                                                      {"ico", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_ICO},
                                                      {"svg", FS_FILETYPE_IMAGE_EXT, FS_FILEEXT_SVG},

                                                      // Audio Extensions
                                                      {"wav", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_WAV},
                                                      {"mp3", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_MP3},
                                                      {"ogg", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_OGG},
                                                      {"flac", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_FLAC},
                                                      {"aac", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_AAC},
                                                      {"wma", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_WMA},
                                                      {"aiff", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_AIFF},
                                                      {"alac", FS_FILETYPE_AUDIO_EXT, FS_FILEEXT_ALAC},

                                                      // Video Extensions
                                                      {"mp4", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_MP4},
                                                      {"avi", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_AVI},
                                                      {"mov", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_MOV},
                                                      {"wmv", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_WMV},
                                                      {"mkv", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_MKV},
                                                      {"flv", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_FLV},
                                                      {"webm", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_WEBM},
                                                      {"mpeg", FS_FILETYPE_VIDEO_EXT, FS_FILEEXT_MPEG},

                                                      // Model Extensions
                                                      {"fbx", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_FBX},
                                                      {"obj", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_OBJ},
                                                      {"dae", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_DAE},
                                                      {"3ds", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_3DS},
                                                      {"blend", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_BLEND},
                                                      {"gltf", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_GLTF},
                                                      {"glb", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_GLB},
                                                      {"max", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_MAX},
                                                      {"mb", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_MB},
                                                      {"stl", FS_FILETYPE_MODEL_EXT, FS_FILEEXT_STL},

                                                      // Text Extensions
                                                      {"txt", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_TXT},
                                                      {"json", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_JSON},
                                                      {"xml", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_XML},
                                                      {"lua", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_LUA},
                                                      {"csv", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_CSV},
                                                      {"md", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_MD},
                                                      {"ini", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_INI},
                                                      {"cfg", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_CFG},
                                                      {"shader", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_SHADER},
                                                      {"vert", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_VERT},
                                                      {"frag", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_FRAG},

                                                      // Editor Extensions
                                                      {"pscn", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_PSCN},
                                                      {"ecst", FS_FILETYPE_TEXT_EXT, FS_FILEEXT_ECST}}};

PathInfo GetPathInfo(const std::string &path)
{
    PathInfo pathInfo;
    std::filesystem::path fsPath(path);
    std::error_code ec;
    if (std::filesystem::is_directory(fsPath, ec))
    {
        pathInfo.archeType = FS_ARCHETYPE_DIRECTORY;
    }
    else
    {
        pathInfo.archeType = FS_ARCHETYPE_FILE;
        std::string ext = fsPath.extension().string();
        ext.erase(std::remove(ext.begin(), ext.end(), '.'), ext.end());
        for (const FileExtension& fext : AllExtensions)
        {
            if (ext == fext.extension)
            {
                pathInfo.fileInfo = fext;
                break;
            }
        }
    }

    return pathInfo;
}

} // namespace duin
