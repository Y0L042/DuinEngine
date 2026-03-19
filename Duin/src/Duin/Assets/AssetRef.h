#pragma once

#include <string>
#include <optional>
#include "FileTypes.h"
#include "Duin/Core/Utils/UUID.h"

namespace duin
{
struct AssetRef
{
    static const std::string TAG_RPATH;
    static const std::string TAG_UUID;
    static const std::string TAG_FILETYPE;
    static const std::string TAG_FILEEXT;

    std::string rPath;
    std::optional<UUID> uuid;
    std::optional<FileType> fileType;
    std::optional<FileExt> fileExt;

    AssetRef() = default;
    AssetRef(const std::string &rPath) : rPath(rPath)
    {
    }
    AssetRef(const std::string &rPath, FileType fileType, FileExt fileExt)
        : rPath(rPath), uuid(UUID()), fileType(fileType), fileExt(fileExt)
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
        std::string rPath;
        std::optional<std::string> uuid;
        std::optional<FileType> fileType;
        std::optional<FileExt> fileExt;
    };
    AssetRef(const ReflectionType &impl)
        : rPath(impl.rPath), uuid(impl.uuid ? std::optional<UUID>(UUID::FromStringHex(*impl.uuid)) : std::nullopt),
          fileType(impl.fileType), fileExt(impl.fileExt)
    {
    }
    ReflectionType reflection() const
    {
        return {rPath, uuid ? std::optional<std::string>(uuid->ToStrHex()) : std::nullopt, fileType, fileExt};
    }
};
} // namespace duin