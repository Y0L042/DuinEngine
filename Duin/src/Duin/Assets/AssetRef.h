#pragma once

#include <string>
#include "FileTypes.h"
#include "Duin/Core/Utils/UUID.h"

namespace duin
{
struct AssetRef
{
    static const std::string TAG_UUID;
    static const std::string TAG_HINTPATH;
    static const std::string TAG_FILETYPE;
    static const std::string TAG_FILEEXT;

    UUID uuid = UUID::INVALID;
    std::string hintPath;
    FileType fileType;
    FileExt fileExt;

    AssetRef() = default;
    AssetRef(const std::string &hintPath, FileType fileType, FileExt fileExt)
        : uuid(UUID()), hintPath(hintPath), fileType(fileType), fileExt(fileExt)
    {
    }
    ~AssetRef() = default;

    bool IsValid() const
    {
        return uuid != UUID::INVALID;
    }

    bool operator==(const AssetRef &other) const
    {
        return uuid == other.uuid;
    }

    bool operator!=(const AssetRef &other) const
    {
        return !(*this == other);
    }

    using ReflectionType = struct AssetRefImpl
    {
        std::string uuid;
        std::string hintPath;
        FileType fileType;
        FileExt fileExt;
    };
    AssetRef(const ReflectionType &impl)
        : uuid(UUID::FromStringHex(impl.uuid)), hintPath(impl.hintPath), fileType(impl.fileType), fileExt(impl.fileExt)
    {
    }
    ReflectionType reflection() const
    {
        return {uuid.ToStrHex(), hintPath, fileType, fileExt};
    }
};
} // namespace duin