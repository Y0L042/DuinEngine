/**
 * @file IOStream.cpp
 * @brief Implementation of the duin::IOStream RAII wrapper around SDL3's SDL_IOStream.
 *
 * @see IOStream.h
 * @see https://wiki.libsdl.org/SDL3/CategoryIOStream
 */

#include "dnpch.h"
#include "IOStream.h"
#include <Duin/Core/Debug/DNLog.h>

namespace duin::io
{

// --- Helper: Convert IOWhence to SDL_IOWhence ---
static SDL_IOWhence ToSDLWhence(IOWhence whence)
{
    switch (whence)
    {
    case IOWhence::Set: return SDL_IO_SEEK_SET;
    case IOWhence::Cur: return SDL_IO_SEEK_CUR;
    case IOWhence::End: return SDL_IO_SEEK_END;
    default:            return SDL_IO_SEEK_SET;
    }
}

// --- Helper: Convert SDL_IOStatus to IOStatus ---
static IOStatus FromSDLStatus(SDL_IOStatus status)
{
    switch (status)
    {
    case SDL_IO_STATUS_READY:     return IOStatus::Ready;
    case SDL_IO_STATUS_ERROR:     return IOStatus::Error;
    case SDL_IO_STATUS_EOF:       return IOStatus::EOF_;
    case SDL_IO_STATUS_NOT_READY: return IOStatus::NotReady;
    case SDL_IO_STATUS_READONLY:  return IOStatus::Readonly;
    case SDL_IO_STATUS_WRITEONLY: return IOStatus::Writeonly;
    default:                      return IOStatus::Error;
    }
}

// --- Private Constructor ---

IOStream::IOStream(SDL_IOStream *stream)
    : m_stream(stream)
{
}

// --- Static Factories ---

IOStream IOStream::FromFile(const std::string &file, const std::string &mode)
{
    SDL_IOStream *stream = SDL_IOFromFile(file.c_str(), mode.c_str());
    if (!stream)
    {
        DN_CORE_ERROR("IOStream::FromFile failed for '{}' (mode '{}'): {}", file, mode, SDL_GetError());
    }
    return IOStream(stream);
}

IOStream IOStream::FromMem(void *mem, size_t size)
{
    SDL_IOStream *stream = SDL_IOFromMem(mem, size);
    if (!stream)
    {
        DN_CORE_ERROR("IOStream::FromMem failed: {}", SDL_GetError());
    }
    return IOStream(stream);
}

IOStream IOStream::FromConstMem(const void *mem, size_t size)
{
    SDL_IOStream *stream = SDL_IOFromConstMem(mem, size);
    if (!stream)
    {
        DN_CORE_ERROR("IOStream::FromConstMem failed: {}", SDL_GetError());
    }
    return IOStream(stream);
}

IOStream IOStream::FromDynamicMem()
{
    SDL_IOStream *stream = SDL_IOFromDynamicMem();
    if (!stream)
    {
        DN_CORE_ERROR("IOStream::FromDynamicMem failed: {}", SDL_GetError());
    }
    return IOStream(stream);
}

IOStream IOStream::Open(const SDL_IOStreamInterface *iface, void *userdata)
{
    SDL_IOStream *stream = SDL_OpenIO(iface, userdata);
    if (!stream)
    {
        DN_CORE_ERROR("IOStream::Open failed: {}", SDL_GetError());
    }
    return IOStream(stream);
}

// --- Static Bulk Operations ---

void *IOStream::LoadFileIO(IOStream &src, size_t *datasize, bool closeWhenDone)
{
    if (!src.m_stream)
    {
        DN_CORE_WARN("IOStream::LoadFileIO called on invalid stream");
        return nullptr;
    }
    void *data = SDL_LoadFile_IO(src.m_stream, datasize, closeWhenDone ? true : false);
    if (closeWhenDone)
    {
        src.m_stream = nullptr;
    }
    return data;
}

bool IOStream::SaveFileIO(IOStream &src, const void *data, size_t datasize, bool closeWhenDone)
{
    if (!src.m_stream)
    {
        DN_CORE_WARN("IOStream::SaveFileIO called on invalid stream");
        return false;
    }
    bool result = SDL_SaveFile_IO(src.m_stream, data, datasize, closeWhenDone ? true : false);
    if (closeWhenDone)
    {
        src.m_stream = nullptr;
    }
    return result;
}

void *IOStream::LoadFile(const std::string &file, size_t *datasize)
{
    void *data = SDL_LoadFile(file.c_str(), datasize);
    if (!data)
    {
        DN_CORE_ERROR("IOStream::LoadFile failed for '{}': {}", file, SDL_GetError());
    }
    return data;
}

bool IOStream::SaveFile(const std::string &file, const void *data, size_t datasize)
{
    bool result = SDL_SaveFile(file.c_str(), data, datasize);
    if (!result)
    {
        DN_CORE_ERROR("IOStream::SaveFile failed for '{}': {}", file, SDL_GetError());
    }
    return result;
}

void IOStream::FreeLoadedData(void *data)
{
    SDL_free(data);
}

// --- Lifecycle ---

IOStream::IOStream()
    : m_stream(nullptr)
{
}

IOStream::~IOStream()
{
    Close();
}

IOStream::IOStream(IOStream &&other) noexcept
    : m_stream(other.m_stream)
{
    other.m_stream = nullptr;
}

IOStream &IOStream::operator=(IOStream &&other) noexcept
{
    if (this != &other)
    {
        Close();
        m_stream = other.m_stream;
        other.m_stream = nullptr;
    }
    return *this;
}

// --- State ---

bool IOStream::IsValid() const
{
    return m_stream != nullptr;
}

IOStatus IOStream::GetStatus() const
{
    if (!m_stream)
    {
        return IOStatus::Error;
    }
    return FromSDLStatus(SDL_GetIOStatus(m_stream));
}

int64_t IOStream::GetSize() const
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::GetSize called on invalid stream");
        return -1;
    }
    return SDL_GetIOSize(m_stream);
}

int64_t IOStream::Tell() const
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Tell called on invalid stream");
        return -1;
    }
    return SDL_TellIO(m_stream);
}

SDL_IOStream *IOStream::GetHandle() const
{
    return m_stream;
}

// --- Core Operations ---

size_t IOStream::Read(void *ptr, size_t size)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Read called on invalid stream");
        return 0;
    }
    return SDL_ReadIO(m_stream, ptr, size);
}

size_t IOStream::Write(const void *ptr, size_t size)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Write called on invalid stream");
        return 0;
    }
    return SDL_WriteIO(m_stream, ptr, size);
}

int64_t IOStream::Seek(int64_t offset, IOWhence whence)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Seek called on invalid stream");
        return -1;
    }
    return SDL_SeekIO(m_stream, offset, ToSDLWhence(whence));
}

bool IOStream::Flush()
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Flush called on invalid stream");
        return false;
    }
    return SDL_FlushIO(m_stream);
}

bool IOStream::Close()
{
    if (!m_stream)
    {
        return true;
    }
    bool result = SDL_CloseIO(m_stream);
    m_stream = nullptr;
    return result;
}

// --- Typed Reads ---

bool IOStream::ReadU8(uint8_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU8 called on invalid stream");
        return false;
    }
    return SDL_ReadU8(m_stream, value);
}

bool IOStream::ReadS8(int8_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS8 called on invalid stream");
        return false;
    }
    return SDL_ReadS8(m_stream, value);
}

bool IOStream::ReadU16LE(uint16_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU16LE called on invalid stream");
        return false;
    }
    return SDL_ReadU16LE(m_stream, value);
}

bool IOStream::ReadU16BE(uint16_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU16BE called on invalid stream");
        return false;
    }
    return SDL_ReadU16BE(m_stream, value);
}

bool IOStream::ReadS16LE(int16_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS16LE called on invalid stream");
        return false;
    }
    return SDL_ReadS16LE(m_stream, value);
}

bool IOStream::ReadS16BE(int16_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS16BE called on invalid stream");
        return false;
    }
    return SDL_ReadS16BE(m_stream, value);
}

bool IOStream::ReadU32LE(uint32_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU32LE called on invalid stream");
        return false;
    }
    return SDL_ReadU32LE(m_stream, value);
}

bool IOStream::ReadU32BE(uint32_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU32BE called on invalid stream");
        return false;
    }
    return SDL_ReadU32BE(m_stream, value);
}

bool IOStream::ReadS32LE(int32_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS32LE called on invalid stream");
        return false;
    }
    return SDL_ReadS32LE(m_stream, value);
}

bool IOStream::ReadS32BE(int32_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS32BE called on invalid stream");
        return false;
    }
    return SDL_ReadS32BE(m_stream, value);
}

bool IOStream::ReadU64LE(uint64_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU64LE called on invalid stream");
        return false;
    }
    return SDL_ReadU64LE(m_stream, value);
}

bool IOStream::ReadU64BE(uint64_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadU64BE called on invalid stream");
        return false;
    }
    return SDL_ReadU64BE(m_stream, value);
}

bool IOStream::ReadS64LE(int64_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS64LE called on invalid stream");
        return false;
    }
    return SDL_ReadS64LE(m_stream, value);
}

bool IOStream::ReadS64BE(int64_t *value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::ReadS64BE called on invalid stream");
        return false;
    }
    return SDL_ReadS64BE(m_stream, value);
}

// --- Typed Writes ---

bool IOStream::WriteU8(uint8_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU8 called on invalid stream");
        return false;
    }
    return SDL_WriteU8(m_stream, value);
}

bool IOStream::WriteS8(int8_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS8 called on invalid stream");
        return false;
    }
    return SDL_WriteS8(m_stream, value);
}

bool IOStream::WriteU16LE(uint16_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU16LE called on invalid stream");
        return false;
    }
    return SDL_WriteU16LE(m_stream, value);
}

bool IOStream::WriteU16BE(uint16_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU16BE called on invalid stream");
        return false;
    }
    return SDL_WriteU16BE(m_stream, value);
}

bool IOStream::WriteS16LE(int16_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS16LE called on invalid stream");
        return false;
    }
    return SDL_WriteS16LE(m_stream, value);
}

bool IOStream::WriteS16BE(int16_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS16BE called on invalid stream");
        return false;
    }
    return SDL_WriteS16BE(m_stream, value);
}

bool IOStream::WriteU32LE(uint32_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU32LE called on invalid stream");
        return false;
    }
    return SDL_WriteU32LE(m_stream, value);
}

bool IOStream::WriteU32BE(uint32_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU32BE called on invalid stream");
        return false;
    }
    return SDL_WriteU32BE(m_stream, value);
}

bool IOStream::WriteS32LE(int32_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS32LE called on invalid stream");
        return false;
    }
    return SDL_WriteS32LE(m_stream, value);
}

bool IOStream::WriteS32BE(int32_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS32BE called on invalid stream");
        return false;
    }
    return SDL_WriteS32BE(m_stream, value);
}

bool IOStream::WriteU64LE(uint64_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU64LE called on invalid stream");
        return false;
    }
    return SDL_WriteU64LE(m_stream, value);
}

bool IOStream::WriteU64BE(uint64_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteU64BE called on invalid stream");
        return false;
    }
    return SDL_WriteU64BE(m_stream, value);
}

bool IOStream::WriteS64LE(int64_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS64LE called on invalid stream");
        return false;
    }
    return SDL_WriteS64LE(m_stream, value);
}

bool IOStream::WriteS64BE(int64_t value)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::WriteS64BE called on invalid stream");
        return false;
    }
    return SDL_WriteS64BE(m_stream, value);
}

// --- Formatted Write ---

size_t IOStream::Printf(const char *fmt, ...)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::Printf called on invalid stream");
        return 0;
    }
    va_list ap;
    va_start(ap, fmt);
    size_t result = SDL_IOvprintf(m_stream, fmt, ap);
    va_end(ap);
    return result;
}

size_t IOStream::VPrintf(const char *fmt, va_list ap)
{
    if (!m_stream)
    {
        DN_CORE_WARN("IOStream::VPrintf called on invalid stream");
        return 0;
    }
    return SDL_IOvprintf(m_stream, fmt, ap);
}

} // namespace duin
