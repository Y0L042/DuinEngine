#pragma once

#include "Duin/Core/Utils/UUID.h"
#include "FileTypes.h"

namespace duin
{
struct Asset
{
    UUID uuid;
    ArcheType archeType = FS_ARCHETYPE_INVALID;
    FileType fileType = FS_FILETYPE_INVALID_EXT;
    FileExt fileExt = FS_FILEEXT_NULL;
    std::string path = "";
};
} // namespace duin