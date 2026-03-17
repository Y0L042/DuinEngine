/**
 * @file IOStream.h
 * @brief C++ RAII wrapper around SDL3's SDL_IOStream API.
 *
 * Provides a move-only, type-safe stream I/O class with automatic resource cleanup.
 * Supports file, memory, dynamic memory, and custom streams, as well as endian-aware
 * typed reads/writes and bulk file operations.
 *
 * @see https://wiki.libsdl.org/SDL3/CategoryIOStream
 */

#pragma once

#include <SDL3/SDL_iostream.h>
#include <string>
#include <cstdint>
#include <cstdarg>

namespace duin::io
{

/**
 * @brief Seek origin for IOStream operations.
 *
 * Maps to SDL_IO_SEEK_SET, SDL_IO_SEEK_CUR, SDL_IO_SEEK_END.
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_IOWhence
 */
enum class IOWhence
{
    Set, ///< Seek from the beginning of the stream
    Cur, ///< Seek relative to the current position
    End  ///< Seek relative to the end of the stream
};

/**
 * @brief Status of an IOStream.
 *
 * Maps to SDL_IO_STATUS_* values.
 *
 * @see https://wiki.libsdl.org/SDL3/SDL_IOStatus
 */
enum class IOStatus
{
    Ready,     ///< Stream is ready for I/O
    Error,     ///< An error occurred
    EOF_,      ///< End of file/stream reached
    NotReady,  ///< Stream is not ready (non-blocking)
    Readonly,  ///< Stream is read-only
    Writeonly  ///< Stream is write-only
};

/**
 * @class IOStream
 * @brief RAII, move-only wrapper around SDL3's SDL_IOStream.
 *
 * Owns an SDL_IOStream pointer and automatically closes it on destruction.
 * Use static factory methods to create instances. Copy is deleted; move transfers ownership.
 *
 * @see https://wiki.libsdl.org/SDL3/CategoryIOStream
 */
class IOStream
{
  public:
    // --- Static Factories ---

    /**
     * @brief Create an IOStream from a file path.
     *
     * @param file Path to the file to open
     * @param mode File mode string (e.g., "rb", "wb", "r+b")
     * @return IOStream wrapping the opened file, or invalid stream on failure
     *
     * @note Logs DN_CORE_ERROR with SDL_GetError() on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOFromFile
     *
     * Example:
     * @code
     * duin::IOStream stream = duin::IOStream::FromFile("data.bin", "rb");
     * if (stream.IsValid()) {
     *     // Read from stream
     * }
     * @endcode
     */
    static IOStream FromFile(const std::string &file, const std::string &mode);

    /**
     * @brief Create an IOStream from a read/write memory buffer.
     *
     * @param mem Pointer to the memory buffer
     * @param size Size of the memory buffer in bytes
     * @return IOStream wrapping the memory buffer, or invalid stream on failure
     *
     * @note The memory must remain valid for the lifetime of the IOStream
     * @note Logs DN_CORE_ERROR with SDL_GetError() on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOFromMem
     */
    static IOStream FromMem(void *mem, size_t size);

    /**
     * @brief Create an IOStream from a read-only memory buffer.
     *
     * @param mem Pointer to the const memory buffer
     * @param size Size of the memory buffer in bytes
     * @return IOStream wrapping the memory buffer, or invalid stream on failure
     *
     * @note The memory must remain valid for the lifetime of the IOStream
     * @note Logs DN_CORE_ERROR with SDL_GetError() on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOFromConstMem
     */
    static IOStream FromConstMem(const void *mem, size_t size);

    /**
     * @brief Create an IOStream backed by dynamically-allocated memory.
     *
     * The stream grows as data is written to it.
     *
     * @return IOStream wrapping dynamic memory, or invalid stream on failure
     *
     * @note Logs DN_CORE_ERROR with SDL_GetError() on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOFromDynamicMem
     */
    static IOStream FromDynamicMem();

    /**
     * @brief Create a custom IOStream with a user-provided interface.
     *
     * @param iface Pointer to an SDL_IOStreamInterface defining custom I/O callbacks
     * @param userdata User data pointer passed to the callbacks
     * @return IOStream wrapping the custom interface, or invalid stream on failure
     *
     * @note Logs DN_CORE_ERROR with SDL_GetError() on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_OpenIO
     */
    static IOStream Open(const SDL_IOStreamInterface *iface, void *userdata);

    // --- Static Bulk Operations ---

    /**
     * @brief Load all data from an IOStream.
     *
     * @param src The IOStream to read from
     * @param datasize Output parameter receiving the size of the loaded data
     * @param closeWhenDone If true, the stream is closed after loading and src is invalidated
     * @return Pointer to the loaded data, or nullptr on failure. Free with FreeLoadedData()
     *
     * @note When closeWhenDone is true, src.m_stream is set to nullptr to prevent double-close
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_LoadFile_IO
     * @see FreeLoadedData()
     */
    static void *LoadFileIO(IOStream &src, size_t *datasize, bool closeWhenDone);

    /**
     * @brief Save data to an IOStream.
     *
     * @param src The IOStream to write to
     * @param data Pointer to the data to write
     * @param datasize Size of the data in bytes
     * @param closeWhenDone If true, the stream is closed after saving and src is invalidated
     * @return true on success, false on failure
     *
     * @note When closeWhenDone is true, src.m_stream is set to nullptr to prevent double-close
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_SaveFile_IO
     */
    static bool SaveFileIO(IOStream &src, const void *data, size_t datasize, bool closeWhenDone);

    /**
     * @brief Load all data from a file path.
     *
     * @param file Path to the file to load
     * @param datasize Output parameter receiving the size of the loaded data
     * @return Pointer to the loaded data, or nullptr on failure. Free with FreeLoadedData()
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_LoadFile
     * @see FreeLoadedData()
     */
    static void *LoadFile(const std::string &file, size_t *datasize);

    /**
     * @brief Save data to a file path.
     *
     * @param file Path to the file to save to
     * @param data Pointer to the data to write
     * @param datasize Size of the data in bytes
     * @return true on success, false on failure
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_SaveFile
     */
    static bool SaveFile(const std::string &file, const void *data, size_t datasize);

    /**
     * @brief Free data allocated by LoadFileIO() or LoadFile().
     *
     * @param data Pointer to the data to free
     *
     * @see LoadFileIO()
     * @see LoadFile()
     */
    static void FreeLoadedData(void *data);

    // --- Lifecycle ---

    /**
     * @brief Default constructor. Creates an invalid (null) stream.
     */
    IOStream();

    /**
     * @brief Destructor. Closes the stream if valid.
     */
    ~IOStream();

    /**
     * @brief Move constructor. Transfers ownership from other.
     * @param other The IOStream to move from (will be left invalid)
     */
    IOStream(IOStream &&other) noexcept;

    /**
     * @brief Move assignment. Transfers ownership from other.
     * @param other The IOStream to move from (will be left invalid)
     * @return Reference to this IOStream
     */
    IOStream &operator=(IOStream &&other) noexcept;

    // Copy is deleted
    IOStream(const IOStream &) = delete;
    IOStream &operator=(const IOStream &) = delete;

    // --- State ---

    /**
     * @brief Check if the stream is valid (non-null).
     * @return true if the stream handle is valid
     */
    bool IsValid() const;

    /**
     * @brief Get the current status of the stream.
     * @return IOStatus reflecting the stream's state
     *
     * @note Returns IOStatus::Error if the stream is invalid
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_GetIOStatus
     */
    IOStatus GetStatus() const;

    /**
     * @brief Get the total size of the stream in bytes.
     * @return Size in bytes, or -1 on error or if the stream is invalid
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_GetIOSize
     */
    int64_t GetSize() const;

    /**
     * @brief Get the current read/write position in the stream.
     * @return Current position in bytes, or -1 on error or if the stream is invalid
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_TellIO
     */
    int64_t Tell() const;

    /**
     * @brief Get the raw SDL_IOStream pointer for interop.
     * @return The underlying SDL_IOStream pointer (may be nullptr)
     */
    SDL_IOStream *GetHandle() const;

    // --- Core Operations ---

    /**
     * @brief Read data from the stream.
     *
     * @param ptr Destination buffer
     * @param size Number of bytes to read
     * @return Number of bytes actually read, or 0 on error/invalid stream
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadIO
     */
    size_t Read(void *ptr, size_t size);

    /**
     * @brief Write data to the stream.
     *
     * @param ptr Source buffer
     * @param size Number of bytes to write
     * @return Number of bytes actually written, or 0 on error/invalid stream
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteIO
     */
    size_t Write(const void *ptr, size_t size);

    /**
     * @brief Seek to a position in the stream.
     *
     * @param offset Byte offset relative to whence
     * @param whence Origin for the seek operation
     * @return New absolute position in bytes, or -1 on error/invalid stream
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_SeekIO
     */
    int64_t Seek(int64_t offset, IOWhence whence);

    /**
     * @brief Flush any buffered write data to the stream.
     * @return true on success, false on error or if the stream is invalid
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_FlushIO
     */
    bool Flush();

    /**
     * @brief Close the stream and release the handle.
     *
     * After closing, the stream becomes invalid (IsValid() returns false).
     *
     * @return true on success, false on error
     *
     * @note Safe to call on an already-closed or invalid stream (returns true)
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_CloseIO
     */
    bool Close();

    // --- Typed Reads (endian-aware) ---

    /**
     * @brief Read an unsigned 8-bit integer.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU8
     */
    bool ReadU8(uint8_t *value);

    /**
     * @brief Read a signed 8-bit integer.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS8
     */
    bool ReadS8(int8_t *value);

    /**
     * @brief Read an unsigned 16-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU16LE
     */
    bool ReadU16LE(uint16_t *value);

    /**
     * @brief Read an unsigned 16-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU16BE
     */
    bool ReadU16BE(uint16_t *value);

    /**
     * @brief Read a signed 16-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS16LE
     */
    bool ReadS16LE(int16_t *value);

    /**
     * @brief Read a signed 16-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS16BE
     */
    bool ReadS16BE(int16_t *value);

    /**
     * @brief Read an unsigned 32-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU32LE
     */
    bool ReadU32LE(uint32_t *value);

    /**
     * @brief Read an unsigned 32-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU32BE
     */
    bool ReadU32BE(uint32_t *value);

    /**
     * @brief Read a signed 32-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS32LE
     */
    bool ReadS32LE(int32_t *value);

    /**
     * @brief Read a signed 32-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS32BE
     */
    bool ReadS32BE(int32_t *value);

    /**
     * @brief Read an unsigned 64-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU64LE
     */
    bool ReadU64LE(uint64_t *value);

    /**
     * @brief Read an unsigned 64-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadU64BE
     */
    bool ReadU64BE(uint64_t *value);

    /**
     * @brief Read a signed 64-bit integer in little-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS64LE
     */
    bool ReadS64LE(int64_t *value);

    /**
     * @brief Read a signed 64-bit integer in big-endian byte order.
     * @param value Output parameter
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_ReadS64BE
     */
    bool ReadS64BE(int64_t *value);

    // --- Typed Writes (endian-aware) ---

    /**
     * @brief Write an unsigned 8-bit integer.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU8
     */
    bool WriteU8(uint8_t value);

    /**
     * @brief Write a signed 8-bit integer.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS8
     */
    bool WriteS8(int8_t value);

    /**
     * @brief Write an unsigned 16-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU16LE
     */
    bool WriteU16LE(uint16_t value);

    /**
     * @brief Write an unsigned 16-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU16BE
     */
    bool WriteU16BE(uint16_t value);

    /**
     * @brief Write a signed 16-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS16LE
     */
    bool WriteS16LE(int16_t value);

    /**
     * @brief Write a signed 16-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS16BE
     */
    bool WriteS16BE(int16_t value);

    /**
     * @brief Write an unsigned 32-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU32LE
     */
    bool WriteU32LE(uint32_t value);

    /**
     * @brief Write an unsigned 32-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU32BE
     */
    bool WriteU32BE(uint32_t value);

    /**
     * @brief Write a signed 32-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS32LE
     */
    bool WriteS32LE(int32_t value);

    /**
     * @brief Write a signed 32-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS32BE
     */
    bool WriteS32BE(int32_t value);

    /**
     * @brief Write an unsigned 64-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU64LE
     */
    bool WriteU64LE(uint64_t value);

    /**
     * @brief Write an unsigned 64-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteU64BE
     */
    bool WriteU64BE(uint64_t value);

    /**
     * @brief Write a signed 64-bit integer in little-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS64LE
     */
    bool WriteS64LE(int64_t value);

    /**
     * @brief Write a signed 64-bit integer in big-endian byte order.
     * @param value The value to write
     * @return true on success, false on error/invalid stream
     * @see https://wiki.libsdl.org/SDL3/SDL_WriteS64BE
     */
    bool WriteS64BE(int64_t value);

    // --- Formatted Write ---

    /**
     * @brief Write formatted output to the stream (printf-style).
     *
     * @param fmt Format string (printf-style)
     * @param ... Arguments matching the format string
     * @return Number of bytes written, or 0 on error/invalid stream
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOprintf
     */
    size_t Printf(const char *fmt, ...);

    /**
     * @brief Write formatted output to the stream (vprintf-style).
     *
     * @param fmt Format string (printf-style)
     * @param ap Variable argument list
     * @return Number of bytes written, or 0 on error/invalid stream
     *
     * @see https://wiki.libsdl.org/SDL3/SDL_IOvprintf
     */
    size_t VPrintf(const char *fmt, va_list ap);

  private:
    SDL_IOStream *m_stream = nullptr; ///< Owned SDL_IOStream handle

    /**
     * @brief Private constructor for factory methods.
     * @param stream Raw SDL_IOStream pointer to take ownership of
     */
    explicit IOStream(SDL_IOStream *stream);
};

} // namespace duin
