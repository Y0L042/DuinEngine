#include "FileTypes.h"
#include <string>
#include <array>

const std::array<FileExtension, 49> AllExtensions = {{// Image Extensions
                                                      {"png", IMAGE_EXT, FILEEXT_PNG},
                                                      {"jpg", IMAGE_EXT, FILEEXT_JPG},
                                                      {"jpeg", IMAGE_EXT, FILEEXT_JPEG},
                                                      {"bmp", IMAGE_EXT, FILEEXT_BMP},
                                                      {"tga", IMAGE_EXT, FILEEXT_TGA},
                                                      {"gif", IMAGE_EXT, FILEEXT_GIF},
                                                      {"hdr", IMAGE_EXT, FILEEXT_HDR},
                                                      {"tiff", IMAGE_EXT, FILEEXT_TIFF},
                                                      {"dds", IMAGE_EXT, FILEEXT_DDS},
                                                      {"ico", IMAGE_EXT, FILEEXT_ICO},
                                                      {"svg", IMAGE_EXT, FILEEXT_SVG},

                                                      // Audio Extensions
                                                      {"wav", AUDIO_EXT, FILEEXT_WAV},
                                                      {"mp3", AUDIO_EXT, FILEEXT_MP3},
                                                      {"ogg", AUDIO_EXT, FILEEXT_OGG},
                                                      {"flac", AUDIO_EXT, FILEEXT_FLAC},
                                                      {"aac", AUDIO_EXT, FILEEXT_AAC},
                                                      {"wma", AUDIO_EXT, FILEEXT_WMA},
                                                      {"aiff", AUDIO_EXT, FILEEXT_AIFF},
                                                      {"alac", AUDIO_EXT, FILEEXT_ALAC},

                                                      // Video Extensions
                                                      {"mp4", VIDEO_EXT, FILEEXT_MP4},
                                                      {"avi", VIDEO_EXT, FILEEXT_AVI},
                                                      {"mov", VIDEO_EXT, FILEEXT_MOV},
                                                      {"wmv", VIDEO_EXT, FILEEXT_WMV},
                                                      {"mkv", VIDEO_EXT, FILEEXT_MKV},
                                                      {"flv", VIDEO_EXT, FILEEXT_FLV},
                                                      {"webm", VIDEO_EXT, FILEEXT_WEBM},
                                                      {"mpeg", VIDEO_EXT, FILEEXT_MPEG},

                                                      // Model Extensions
                                                      {"fbx", MODEL_EXT, FILEEXT_FBX},
                                                      {"obj", MODEL_EXT, FILEEXT_OBJ},
                                                      {"dae", MODEL_EXT, FILEEXT_DAE},
                                                      {"3ds", MODEL_EXT, FILEEXT_3DS},
                                                      {"blend", MODEL_EXT, FILEEXT_BLEND},
                                                      {"gltf", MODEL_EXT, FILEEXT_GLTF},
                                                      {"glb", MODEL_EXT, FILEEXT_GLB},
                                                      {"max", MODEL_EXT, FILEEXT_MAX},
                                                      {"mb", MODEL_EXT, FILEEXT_MB},
                                                      {"stl", MODEL_EXT, FILEEXT_STL},

                                                      // Text Extensions
                                                      {"txt", TEXT_EXT, FILEEXT_TXT},
                                                      {"json", TEXT_EXT, FILEEXT_JSON},
                                                      {"xml", TEXT_EXT, FILEEXT_XML},
                                                      {"lua", TEXT_EXT, FILEEXT_LUA},
                                                      {"csv", TEXT_EXT, FILEEXT_CSV},
                                                      {"md", TEXT_EXT, FILEEXT_MD},
                                                      {"ini", TEXT_EXT, FILEEXT_INI},
                                                      {"cfg", TEXT_EXT, FILEEXT_CFG},
                                                      {"shader", TEXT_EXT, FILEEXT_SHADER},
                                                      {"vert", TEXT_EXT, FILEEXT_VERT},
                                                      {"frag", TEXT_EXT, FILEEXT_FRAG},
                                                      // Editor Extensions
                                                      {"ecst", TEXT_EXT, FILEEXT_ECST}}};

ArcheType GetArcheType(const std::string &path)
{
    return ArcheType();
}

FileType GetFileType(const std::string &extension)
{
    return FileType();
}
