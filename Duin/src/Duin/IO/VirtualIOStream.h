/**
 * @file VirtualIOStream.h
 * @brief Virtual-path wrappers around duin::io::IOStream file operations.
 *
 * Provides free functions in the duin::vio namespace that resolve virtual path prefixes
 * (bin://, app://, usr://, wrk://) to system paths via fs::MapVirtualToSystemPath,
 * then delegate to the corresponding duin::io::IOStream static methods.
 *
 * @see IOStream.h
 * @see VirtualFilesystem.h
 * @see fs::MapVirtualToSystemPath
 */

#pragma once

#include <Duin/IO/IOStream.h>
#include <string>

namespace duin::vio
{

/**
 * @brief Create an IOStream from a virtual file path.
 *
 * Resolves the virtual path (bin://, app://, usr://, wrk://) to a system path,
 * then opens the file.
 *
 * @param virtualPath Virtual path to the file (e.g., "wrk://data/save.bin")
 * @param mode File mode string (e.g., "rb", "wb", "r+b")
 * @return IOStream wrapping the opened file, or invalid stream on failure
 *
 * @see io::IOStream::FromFile
 * @see fs::MapVirtualToSystemPath
 */
io::IOStream FromFile(const std::string &virtualPath, const std::string &mode);

/**
 * @brief Load all data from a virtual file path.
 *
 * Resolves the virtual path to a system path, then loads the entire file into memory.
 *
 * @param virtualPath Virtual path to the file (e.g., "bin://assets/texture.png")
 * @param datasize Output parameter receiving the size of the loaded data
 * @return Pointer to the loaded data, or nullptr on failure. Free with FreeLoadedData()
 *
 * @see io::IOStream::LoadFile
 * @see fs::MapVirtualToSystemPath
 */
void *LoadFile(const std::string &virtualPath, size_t *datasize);

/**
 * @brief Save data to a virtual file path.
 *
 * Resolves the virtual path to a system path, then writes the data to the file.
 *
 * @param virtualPath Virtual path to the file (e.g., "usr://config/settings.dat")
 * @param data Pointer to the data to write
 * @param datasize Size of the data in bytes
 * @return true on success, false on failure
 *
 * @see io::IOStream::SaveFile
 * @see fs::MapVirtualToSystemPath
 */
bool SaveFile(const std::string &virtualPath, const void *data, size_t datasize);

/**
 * @brief Free data allocated by LoadFile().
 *
 * Convenience passthrough to io::IOStream::FreeLoadedData().
 *
 * @param data Pointer to the data to free
 *
 * @see LoadFile()
 * @see io::IOStream::FreeLoadedData
 */
void FreeLoadedData(void *data);

} // namespace duin::vio
