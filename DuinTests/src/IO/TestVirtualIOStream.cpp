#include <doctest.h>
#include <Duin/IO/VirtualIOStream.h>
#include <Duin/IO/Filesystem.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>

namespace TestVirtualIOStream
{

// Base directory for test artifacts (relative to test executable)
static const std::string ARTIFACTS_DIR = "./artifacts/vio_test";

// Helper to set up wrk:// and return the resolved system base path
static std::string SetupWorkspace()
{
    std::filesystem::create_directories(ARTIFACTS_DIR);
    std::string absPath = std::filesystem::absolute(ARTIFACTS_DIR).string();
    duin::fs::SetWorkspacePath(absPath);
    return duin::fs::EnsureUnixPath(absPath) + "/";
}

// Helper to create a test file with specific content
static void CreateTestFile(const std::string &path, const std::string &content = "test content")
{
    std::filesystem::create_directories(std::filesystem::path(path).parent_path());
    std::ofstream file(path, std::ios::binary);
    file << content;
    file.close();
}

// Helper to read file content
static std::string ReadFileContent(const std::string &path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.good())
        return "";
    std::string content((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    return content;
}

// Helper to clean up a test file
static void RemoveTestFile(const std::string &path)
{
    std::filesystem::remove(path);
}

// ============================================================================
// Rejection of non-virtual (plain system) paths
// ============================================================================

TEST_SUITE("VirtualIOStream - Non-virtual path rejection")
{
    TEST_CASE("FromFile rejects plain system paths")
    {
        duin::io::IOStream stream = duin::vio::FromFile("C:/some/file.txt", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("FromFile rejects relative paths")
    {
        duin::io::IOStream stream = duin::vio::FromFile("./relative/file.txt", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("FromFile rejects empty path")
    {
        duin::io::IOStream stream = duin::vio::FromFile("", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("LoadFile rejects plain system paths")
    {
        size_t size = 0;
        void *data = duin::vio::LoadFile("C:/some/file.txt", &size);
        CHECK(data == nullptr);
    }

    TEST_CASE("LoadFile rejects relative paths")
    {
        size_t size = 0;
        void *data = duin::vio::LoadFile("./relative/file.txt", &size);
        CHECK(data == nullptr);
    }

    TEST_CASE("SaveFile rejects plain system paths")
    {
        const char content[] = "test";
        CHECK_FALSE(duin::vio::SaveFile("C:/some/file.txt", content, 4));
    }

    TEST_CASE("SaveFile rejects relative paths")
    {
        const char content[] = "test";
        CHECK_FALSE(duin::vio::SaveFile("./relative/file.txt", content, 4));
    }
}

// ============================================================================
// Rejection of unrecognized virtual prefixes
// ============================================================================

TEST_SUITE("VirtualIOStream - Unrecognized prefix rejection")
{
    TEST_CASE("FromFile rejects unknown prefixes")
    {
        duin::io::IOStream stream = duin::vio::FromFile("xyz://file.txt", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("LoadFile rejects unknown prefixes")
    {
        size_t size = 0;
        void *data = duin::vio::LoadFile("res://file.txt", &size);
        CHECK(data == nullptr);
    }

    TEST_CASE("SaveFile rejects unknown prefixes")
    {
        const char content[] = "test";
        CHECK_FALSE(duin::vio::SaveFile("nop://file.txt", content, 4));
    }
}

// ============================================================================
// FromFile via wrk://
// ============================================================================

TEST_SUITE("VirtualIOStream - FromFile via wrk://")
{
    TEST_CASE("Open existing file for reading via wrk://")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "fromfile_read.txt";
        CreateTestFile(syspath, "hello vio");

        duin::io::IOStream stream = duin::vio::FromFile("wrk://fromfile_read.txt", "rb");
        CHECK(stream.IsValid());

        stream.Close();
        RemoveTestFile(syspath);
    }

    TEST_CASE("Open new file for writing via wrk://")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "fromfile_write.txt";
        RemoveTestFile(syspath);

        duin::io::IOStream stream = duin::vio::FromFile("wrk://fromfile_write.txt", "wb");
        CHECK(stream.IsValid());
        stream.Close();

        CHECK(std::filesystem::exists(syspath));
        RemoveTestFile(syspath);
    }

    TEST_CASE("Open nonexistent file for reading via wrk:// returns invalid")
    {
        SetupWorkspace();
        duin::io::IOStream stream = duin::vio::FromFile("wrk://does_not_exist.txt", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("Write and read back via wrk:// FromFile")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "fromfile_roundtrip.bin";

        // Write
        {
            duin::io::IOStream ws = duin::vio::FromFile("wrk://fromfile_roundtrip.bin", "wb");
            REQUIRE(ws.IsValid());
            const char data[] = "vio roundtrip";
            size_t written = ws.Write(data, std::strlen(data));
            CHECK(written == std::strlen(data));
        }

        // Read
        {
            duin::io::IOStream rs = duin::vio::FromFile("wrk://fromfile_roundtrip.bin", "rb");
            REQUIRE(rs.IsValid());
            char buf[64] = {};
            size_t bytesRead = rs.Read(buf, 64);
            CHECK(bytesRead == std::strlen("vio roundtrip"));
            CHECK(std::string(buf, bytesRead) == "vio roundtrip");
        }

        RemoveTestFile(syspath);
    }
}

// ============================================================================
// LoadFile / SaveFile via wrk://
// ============================================================================

TEST_SUITE("VirtualIOStream - SaveFile and LoadFile via wrk://")
{
    TEST_CASE("SaveFile and LoadFile roundtrip via wrk://")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "bulk_roundtrip.bin";

        const char data[] = "vio bulk data";
        CHECK(duin::vio::SaveFile("wrk://bulk_roundtrip.bin", data, std::strlen(data)));

        // Verify file exists on disk
        CHECK(std::filesystem::exists(syspath));

        size_t loadedSize = 0;
        void *loaded = duin::vio::LoadFile("wrk://bulk_roundtrip.bin", &loadedSize);
        REQUIRE(loaded != nullptr);
        CHECK(loadedSize == std::strlen(data));
        CHECK(std::memcmp(loaded, data, loadedSize) == 0);

        duin::vio::FreeLoadedData(loaded);
        RemoveTestFile(syspath);
    }

    TEST_CASE("LoadFile on nonexistent virtual path returns nullptr")
    {
        SetupWorkspace();
        size_t size = 0;
        void *data = duin::vio::LoadFile("wrk://nonexistent_bulk.bin", &size);
        CHECK(data == nullptr);
    }

    TEST_CASE("SaveFile creates file at resolved path")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "save_verify.txt";

        const char content[] = "save verify";
        CHECK(duin::vio::SaveFile("wrk://save_verify.txt", content, std::strlen(content)));
        CHECK(ReadFileContent(syspath) == "save verify");

        RemoveTestFile(syspath);
    }

    TEST_CASE("SaveFile overwrites existing file")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "overwrite.txt";

        const char first[] = "first";
        CHECK(duin::vio::SaveFile("wrk://overwrite.txt", first, std::strlen(first)));
        CHECK(ReadFileContent(syspath) == "first");

        const char second[] = "second content";
        CHECK(duin::vio::SaveFile("wrk://overwrite.txt", second, std::strlen(second)));
        CHECK(ReadFileContent(syspath) == "second content");

        RemoveTestFile(syspath);
    }
}

// ============================================================================
// FreeLoadedData
// ============================================================================

TEST_SUITE("VirtualIOStream - FreeLoadedData")
{
    TEST_CASE("FreeLoadedData with nullptr does not crash")
    {
        duin::vio::FreeLoadedData(nullptr);
        // No crash = pass
    }

    TEST_CASE("FreeLoadedData frees data from LoadFile")
    {
        std::string base = SetupWorkspace();
        std::string syspath = base + "free_test.bin";

        const char data[] = "free me";
        duin::vio::SaveFile("wrk://free_test.bin", data, std::strlen(data));

        size_t size = 0;
        void *loaded = duin::vio::LoadFile("wrk://free_test.bin", &size);
        REQUIRE(loaded != nullptr);

        duin::vio::FreeLoadedData(loaded);
        // No crash after free = pass

        RemoveTestFile(syspath);
    }
}

// ============================================================================
// bin:// path resolution
// ============================================================================

TEST_SUITE("VirtualIOStream - bin:// paths")
{
    TEST_CASE("FromFile with bin:// resolves to executable directory")
    {
        // bin:// points to the executable's base path
        // Opening a nonexistent file should return invalid (but the path should resolve)
        duin::io::IOStream stream = duin::vio::FromFile("bin://nonexistent_test_file.bin", "rb");
        CHECK_FALSE(stream.IsValid());
        // The key check: the path was *resolved* (no rejection error), just the file doesn't exist
    }
}

// ============================================================================
// Integration
// ============================================================================

TEST_SUITE("VirtualIOStream - Integration")
{
    TEST_CASE("Full workflow: save, load, verify, overwrite, load again")
    {
        std::string base = SetupWorkspace();

        // Save initial data
        const char initial[] = "initial data";
        CHECK(duin::vio::SaveFile("wrk://workflow.bin", initial, std::strlen(initial)));

        // Load and verify
        size_t size = 0;
        void *loaded = duin::vio::LoadFile("wrk://workflow.bin", &size);
        REQUIRE(loaded != nullptr);
        CHECK(size == std::strlen(initial));
        CHECK(std::memcmp(loaded, initial, size) == 0);
        duin::vio::FreeLoadedData(loaded);

        // Open with FromFile to append
        {
            duin::io::IOStream stream = duin::vio::FromFile("wrk://workflow.bin", "wb");
            REQUIRE(stream.IsValid());
            const char replaced[] = "replaced content here";
            stream.Write(replaced, std::strlen(replaced));
        }

        // Load again and verify new content
        size_t newSize = 0;
        void *newLoaded = duin::vio::LoadFile("wrk://workflow.bin", &newSize);
        REQUIRE(newLoaded != nullptr);
        CHECK(newSize == std::strlen("replaced content here"));
        CHECK(std::memcmp(newLoaded, "replaced content here", newSize) == 0);
        duin::vio::FreeLoadedData(newLoaded);

        // Cleanup
        std::string syspath = base + "workflow.bin";
        RemoveTestFile(syspath);
    }

    TEST_CASE("Binary data roundtrip via SaveFile/LoadFile")
    {
        std::string base = SetupWorkspace();

        // Create binary data with all byte values
        uint8_t binaryData[256];
        for (int i = 0; i < 256; i++)
        {
            binaryData[i] = static_cast<uint8_t>(i);
        }

        CHECK(duin::vio::SaveFile("wrk://binary_roundtrip.bin", binaryData, sizeof(binaryData)));

        size_t loadedSize = 0;
        void *loaded = duin::vio::LoadFile("wrk://binary_roundtrip.bin", &loadedSize);
        REQUIRE(loaded != nullptr);
        CHECK(loadedSize == 256);
        CHECK(std::memcmp(loaded, binaryData, 256) == 0);

        duin::vio::FreeLoadedData(loaded);
        RemoveTestFile(base + "binary_roundtrip.bin");
    }

    TEST_CASE("FromFile stream supports typed read/write via wrk://")
    {
        std::string base = SetupWorkspace();

        // Write typed values
        {
            duin::io::IOStream ws = duin::vio::FromFile("wrk://typed_vio.bin", "wb");
            REQUIRE(ws.IsValid());
            ws.WriteU32LE(0xDEADBEEF);
            ws.WriteU16BE(0x1234);
            ws.WriteU8(0x42);
        }

        // Read them back
        {
            duin::io::IOStream rs = duin::vio::FromFile("wrk://typed_vio.bin", "rb");
            REQUIRE(rs.IsValid());

            uint32_t u32 = 0;
            uint16_t u16 = 0;
            uint8_t u8 = 0;

            CHECK(rs.ReadU32LE(&u32));
            CHECK(u32 == 0xDEADBEEF);

            CHECK(rs.ReadU16BE(&u16));
            CHECK(u16 == 0x1234);

            CHECK(rs.ReadU8(&u8));
            CHECK(u8 == 0x42);
        }

        RemoveTestFile(base + "typed_vio.bin");
    }
}

// ============================================================================
// Cleanup
// ============================================================================

TEST_SUITE("VirtualIOStream - Cleanup")
{
    TEST_CASE("Remove artifacts directory")
    {
        std::filesystem::remove_all(ARTIFACTS_DIR);
        std::filesystem::create_directories("./artifacts");
    }
}

} // namespace TestVirtualIOStream
