#include "dnpch.h"
#include "VirtualIOStream.h"
#include <Duin/IO/Filesystem.h>
#include <Duin/Core/Debug/DNLog.h>

namespace duin::vio
{

io::IOStream FromFile(const std::string &virtualPath, const std::string &mode)
{
    std::string resolved = fs::MapVirtualToSystemPath(virtualPath);
    if (fs::IsPathInvalid(resolved))
    {
        DN_CORE_ERROR("vio::FromFile: invalid virtual path '{}'", virtualPath);
        return io::IOStream();
    }
    return io::IOStream::FromFile(resolved, mode);
}

void *LoadFile(const std::string &virtualPath, size_t *datasize)
{
    std::string resolved = fs::MapVirtualToSystemPath(virtualPath);
    if (fs::IsPathInvalid(resolved))
    {
        DN_CORE_ERROR("vio::LoadFile: invalid virtual path '{}'", virtualPath);
        return nullptr;
    }
    return io::IOStream::LoadFile(resolved, datasize);
}

bool SaveFile(const std::string &virtualPath, const void *data, size_t datasize)
{
    std::string resolved = fs::MapVirtualToSystemPath(virtualPath);
    if (fs::IsPathInvalid(resolved))
    {
        DN_CORE_ERROR("vio::SaveFile: invalid virtual path '{}'", virtualPath);
        return false;
    }
    return io::IOStream::SaveFile(resolved, data, datasize);
}

void FreeLoadedData(void *data)
{
    io::IOStream::FreeLoadedData(data);
}

} // namespace duin::vio
