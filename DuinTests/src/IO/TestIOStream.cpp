#include <doctest.h>
#include <Duin/IO/IOStream.h>
#include <fstream>
#include <string>
#include <filesystem>
#include <cstring>
#include <vector>

namespace TestIOStream
{

// Base directory for test artifacts (relative to test executable)
static const std::string ARTIFACTS_DIR = "./artifacts";

// Helper to ensure artifacts directory exists
static void EnsureArtifactsDir()
{
    std::filesystem::create_directories(ARTIFACTS_DIR);
}

// Helper to create a test file with specific content
static void CreateTestFile(const std::string &path, const std::string &content = "test content")
{
    EnsureArtifactsDir();
    std::ofstream file(path, std::ios::binary);
    file << content;
    file.close();
}

// Helper to create a binary test file with raw bytes
static void CreateBinaryFile(const std::string &path, const void *data, size_t size)
{
    EnsureArtifactsDir();
    std::ofstream file(path, std::ios::binary);
    file.write(static_cast<const char *>(data), size);
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
// Lifecycle & RAII
// ============================================================================

TEST_SUITE("IOStream - Lifecycle")
{
    TEST_CASE("Default constructor creates invalid stream")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.IsValid());
        CHECK(stream.GetHandle() == nullptr);
    }

    TEST_CASE("Destructor closes stream without error")
    {
        std::string path = ARTIFACTS_DIR + "/lifecycle_dtor.bin";
        CreateTestFile(path, "hello");
        {
            duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "rb");
            CHECK(stream.IsValid());
        }
        // Stream should be closed, no crash or leak
        RemoveTestFile(path);
    }

    TEST_CASE("Move constructor transfers ownership")
    {
        std::string path = ARTIFACTS_DIR + "/lifecycle_move_ctor.bin";
        CreateTestFile(path, "move test");

        duin::io::IOStream original = duin::io::IOStream::FromFile(path, "rb");
        CHECK(original.IsValid());

        duin::io::IOStream moved(std::move(original));
        CHECK(moved.IsValid());
        CHECK_FALSE(original.IsValid());

        moved.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("Move assignment transfers ownership")
    {
        std::string path = ARTIFACTS_DIR + "/lifecycle_move_assign.bin";
        CreateTestFile(path, "assign test");

        duin::io::IOStream original = duin::io::IOStream::FromFile(path, "rb");
        CHECK(original.IsValid());

        duin::io::IOStream target;
        target = std::move(original);
        CHECK(target.IsValid());
        CHECK_FALSE(original.IsValid());

        target.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("Move assignment closes existing stream before taking new one")
    {
        std::string path1 = ARTIFACTS_DIR + "/lifecycle_move_close1.bin";
        std::string path2 = ARTIFACTS_DIR + "/lifecycle_move_close2.bin";
        CreateTestFile(path1, "first");
        CreateTestFile(path2, "second");

        duin::io::IOStream stream1 = duin::io::IOStream::FromFile(path1, "rb");
        duin::io::IOStream stream2 = duin::io::IOStream::FromFile(path2, "rb");
        CHECK(stream1.IsValid());
        CHECK(stream2.IsValid());

        // stream1's underlying handle should be closed before taking stream2's
        stream1 = std::move(stream2);
        CHECK(stream1.IsValid());
        CHECK_FALSE(stream2.IsValid());

        stream1.Close();
        RemoveTestFile(path1);
        RemoveTestFile(path2);
    }

    TEST_CASE("Self move-assignment is safe")
    {
        std::string path = ARTIFACTS_DIR + "/lifecycle_self_move.bin";
        CreateTestFile(path, "self");

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "rb");
        CHECK(stream.IsValid());

        // Suppress compiler warning about self-move
        duin::io::IOStream &ref = stream;
        stream = std::move(ref);
        CHECK(stream.IsValid());

        stream.Close();
        RemoveTestFile(path);
    }
}

// ============================================================================
// Static Factories
// ============================================================================

TEST_SUITE("IOStream - FromFile")
{
    TEST_CASE("Open existing file for reading")
    {
        std::string path = ARTIFACTS_DIR + "/fromfile_read.bin";
        CreateTestFile(path, "read me");

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "rb");
        CHECK_MESSAGE(stream.IsValid(), "Stream is invalid!");

        stream.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("Open new file for writing")
    {
        std::string path = ARTIFACTS_DIR + "/fromfile_write.bin";
        RemoveTestFile(path);

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "wb");
        CHECK(stream.IsValid());
        stream.Close();

        CHECK(std::filesystem::exists(path));
        RemoveTestFile(path);
    }

    TEST_CASE("Open nonexistent file for reading returns invalid")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromFile(ARTIFACTS_DIR + "/does_not_exist.bin", "rb");
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("Open file for read+write")
    {
        std::string path = ARTIFACTS_DIR + "/fromfile_rw.bin";
        CreateTestFile(path, "rw data");

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "r+b");
        CHECK(stream.IsValid());

        stream.Close();
        RemoveTestFile(path);
    }
}

TEST_SUITE("IOStream - FromMem")
{
    TEST_CASE("Create from mutable memory buffer")
    {
        char buffer[64] = "memory buffer data";
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, sizeof(buffer));
        CHECK(stream.IsValid());
    }

    TEST_CASE("Read from memory buffer")
    {
        const char data[] = "hello mem";
        char buffer[64];
        std::memcpy(buffer, data, sizeof(data));

        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, sizeof(data));
        CHECK(stream.IsValid());

        char readBuf[64] = {};
        size_t bytesRead = stream.Read(readBuf, sizeof(data));
        CHECK(bytesRead == sizeof(data));
        CHECK(std::string(readBuf) == "hello mem");
    }

    TEST_CASE("Write to memory buffer")
    {
        char buffer[64] = {};
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, sizeof(buffer));
        CHECK(stream.IsValid());

        const char data[] = "written";
        size_t written = stream.Write(data, sizeof(data));
        CHECK(written == sizeof(data));
        CHECK(std::string(buffer) == "written");
    }
}

TEST_SUITE("IOStream - FromConstMem")
{
    TEST_CASE("Create from const memory buffer")
    {
        const char data[] = "const data";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, sizeof(data));
        CHECK(stream.IsValid());
    }

    TEST_CASE("Read from const memory buffer")
    {
        const char data[] = "read only";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, sizeof(data));

        char readBuf[64] = {};
        size_t bytesRead = stream.Read(readBuf, sizeof(data));
        CHECK(bytesRead == sizeof(data));
        CHECK(std::string(readBuf) == "read only");
    }
}

TEST_SUITE("IOStream - FromDynamicMem")
{
    TEST_CASE("Create dynamic memory stream")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.IsValid());
    }

    TEST_CASE("Write to dynamic memory stream grows it")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.IsValid());

        const char data[] = "dynamic write test data";
        size_t written = stream.Write(data, std::strlen(data));
        CHECK(written == std::strlen(data));
        CHECK(stream.Tell() == static_cast<int64_t>(std::strlen(data)));
    }

    TEST_CASE("Write then seek and read back from dynamic memory")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();

        const char data[] = "roundtrip";
        stream.Write(data, std::strlen(data));
        stream.Seek(0, duin::io::IOWhence::Set);

        char readBuf[64] = {};
        size_t bytesRead = stream.Read(readBuf, std::strlen(data));
        CHECK(bytesRead == std::strlen(data));
        CHECK(std::string(readBuf, bytesRead) == "roundtrip");
    }
}

// ============================================================================
// State Queries
// ============================================================================

TEST_SUITE("IOStream - State")
{
    TEST_CASE("IsValid on default stream")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("IsValid on valid stream")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.IsValid());
    }

    TEST_CASE("GetStatus on invalid stream returns Error")
    {
        duin::io::IOStream stream;
        CHECK(stream.GetStatus() == duin::io::IOStatus::Error);
    }

    TEST_CASE("GetStatus on valid stream returns Ready")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.GetStatus() == duin::io::IOStatus::Ready);
    }

    TEST_CASE("GetSize on memory stream")
    {
        const char data[] = "size test";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, 9);
        CHECK(stream.GetSize() == 9);
    }

    TEST_CASE("GetSize on invalid stream returns -1")
    {
        duin::io::IOStream stream;
        CHECK(stream.GetSize() == -1);
    }

    TEST_CASE("Tell returns current position")
    {
        const char data[] = "tell test data";
        char buffer[64];
        std::memcpy(buffer, data, sizeof(data));
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, sizeof(data));

        CHECK(stream.Tell() == 0);
        stream.Read(buffer, 4);
        CHECK(stream.Tell() == 4);
    }

    TEST_CASE("Tell on invalid stream returns -1")
    {
        duin::io::IOStream stream;
        CHECK(stream.Tell() == -1);
    }

    TEST_CASE("GetHandle returns nullptr for default stream")
    {
        duin::io::IOStream stream;
        CHECK(stream.GetHandle() == nullptr);
    }

    TEST_CASE("GetHandle returns non-null for valid stream")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.GetHandle() != nullptr);
    }
}

// ============================================================================
// Core Operations: Read, Write, Seek, Flush, Close
// ============================================================================

TEST_SUITE("IOStream - Read/Write")
{
    TEST_CASE("Write and read back via file")
    {
        std::string path = ARTIFACTS_DIR + "/rw_file.bin";
        EnsureArtifactsDir();

        // Write
        {
            duin::io::IOStream ws = duin::io::IOStream::FromFile(path, "wb");
            REQUIRE(ws.IsValid());
            const char data[] = "file roundtrip";
            size_t written = ws.Write(data, std::strlen(data));
            CHECK(written == std::strlen(data));
        }

        // Read
        {
            duin::io::IOStream rs = duin::io::IOStream::FromFile(path, "rb");
            REQUIRE(rs.IsValid());
            char buf[64] = {};
            size_t bytesRead = rs.Read(buf, 64);
            CHECK(bytesRead == std::strlen("file roundtrip"));
            CHECK(std::string(buf, bytesRead) == "file roundtrip");
        }

        RemoveTestFile(path);
    }

    TEST_CASE("Read on invalid stream returns 0")
    {
        duin::io::IOStream stream;
        char buf[16];
        CHECK(stream.Read(buf, sizeof(buf)) == 0);
    }

    TEST_CASE("Write on invalid stream returns 0")
    {
        duin::io::IOStream stream;
        const char data[] = "test";
        CHECK(stream.Write(data, sizeof(data)) == 0);
    }

    TEST_CASE("Partial read at end of stream")
    {
        const char data[] = "short";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, 5);

        char buf[64] = {};
        size_t bytesRead = stream.Read(buf, 64);
        CHECK(bytesRead == 5);
        CHECK(std::string(buf, 5) == "short");
    }
}

TEST_SUITE("IOStream - Seek")
{
    TEST_CASE("Seek Set moves to absolute position")
    {
        const char data[] = "abcdefghij";
        char buffer[16];
        std::memcpy(buffer, data, sizeof(data));
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, 10);

        int64_t pos = stream.Seek(5, duin::io::IOWhence::Set);
        CHECK(pos == 5);
        CHECK(stream.Tell() == 5);
    }

    TEST_CASE("Seek Cur moves relative to current position")
    {
        const char data[] = "abcdefghij";
        char buffer[16];
        std::memcpy(buffer, data, sizeof(data));
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, 10);

        stream.Seek(3, duin::io::IOWhence::Set);
        int64_t pos = stream.Seek(2, duin::io::IOWhence::Cur);
        CHECK(pos == 5);
    }

    TEST_CASE("Seek End moves relative to end")
    {
        const char data[] = "abcdefghij";
        char buffer[16];
        std::memcpy(buffer, data, sizeof(data));
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, 10);

        int64_t pos = stream.Seek(-3, duin::io::IOWhence::End);
        CHECK(pos == 7);
    }

    TEST_CASE("Seek to beginning and read")
    {
        const char data[] = "rewind me";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, 9);

        char buf[16] = {};
        stream.Read(buf, 9);
        CHECK(stream.Tell() == 9);

        stream.Seek(0, duin::io::IOWhence::Set);
        CHECK(stream.Tell() == 0);

        char buf2[16] = {};
        stream.Read(buf2, 9);
        CHECK(std::string(buf2, 9) == "rewind me");
    }

    TEST_CASE("Seek on invalid stream returns -1")
    {
        duin::io::IOStream stream;
        CHECK(stream.Seek(0, duin::io::IOWhence::Set) == -1);
    }
}

TEST_SUITE("IOStream - Flush")
{
    TEST_CASE("Flush on valid writable stream succeeds")
    {
        std::string path = ARTIFACTS_DIR + "/flush_test.bin";
        EnsureArtifactsDir();

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "wb");
        REQUIRE(stream.IsValid());
        const char data[] = "flush";
        stream.Write(data, 5);
        CHECK(stream.Flush());

        stream.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("Flush on invalid stream returns false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.Flush());
    }
}

TEST_SUITE("IOStream - Close")
{
    TEST_CASE("Close valid stream succeeds")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.IsValid());
        CHECK(stream.Close());
        CHECK_FALSE(stream.IsValid());
    }

    TEST_CASE("Close already-closed stream returns true")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        stream.Close();
        CHECK(stream.Close());
    }

    TEST_CASE("Close on default (null) stream returns true")
    {
        duin::io::IOStream stream;
        CHECK(stream.Close());
    }

    TEST_CASE("Operations on closed stream behave as invalid")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        stream.Close();

        CHECK_FALSE(stream.IsValid());
        CHECK(stream.GetStatus() == duin::io::IOStatus::Error);
        CHECK(stream.GetSize() == -1);
        CHECK(stream.Tell() == -1);
        CHECK(stream.Seek(0, duin::io::IOWhence::Set) == -1);
        CHECK_FALSE(stream.Flush());

        char buf[16];
        CHECK(stream.Read(buf, sizeof(buf)) == 0);
        CHECK(stream.Write(buf, sizeof(buf)) == 0);
    }
}

// ============================================================================
// Typed Reads & Writes (endian-aware)
// ============================================================================

TEST_SUITE("IOStream - Typed Read/Write U8 S8")
{
    TEST_CASE("Write and read U8")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU8(0xAB));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint8_t val = 0;
        CHECK(stream.ReadU8(&val));
        CHECK(val == 0xAB);
    }

    TEST_CASE("Write and read S8")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS8(-42));
        stream.Seek(0, duin::io::IOWhence::Set);

        int8_t val = 0;
        CHECK(stream.ReadS8(&val));
        CHECK(val == -42);
    }

    TEST_CASE("ReadU8 on invalid stream returns false")
    {
        duin::io::IOStream stream;
        uint8_t val;
        CHECK_FALSE(stream.ReadU8(&val));
    }

    TEST_CASE("WriteU8 on invalid stream returns false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.WriteU8(0));
    }

    TEST_CASE("ReadS8 on invalid stream returns false")
    {
        duin::io::IOStream stream;
        int8_t val;
        CHECK_FALSE(stream.ReadS8(&val));
    }

    TEST_CASE("WriteS8 on invalid stream returns false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.WriteS8(0));
    }
}

TEST_SUITE("IOStream - Typed Read/Write 16-bit")
{
    TEST_CASE("Write and read U16LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU16LE(0x1234));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint16_t val = 0;
        CHECK(stream.ReadU16LE(&val));
        CHECK(val == 0x1234);
    }

    TEST_CASE("Write and read U16BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU16BE(0x1234));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint16_t val = 0;
        CHECK(stream.ReadU16BE(&val));
        CHECK(val == 0x1234);
    }

    TEST_CASE("Write and read S16LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS16LE(-12345));
        stream.Seek(0, duin::io::IOWhence::Set);

        int16_t val = 0;
        CHECK(stream.ReadS16LE(&val));
        CHECK(val == -12345);
    }

    TEST_CASE("Write and read S16BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS16BE(-12345));
        stream.Seek(0, duin::io::IOWhence::Set);

        int16_t val = 0;
        CHECK(stream.ReadS16BE(&val));
        CHECK(val == -12345);
    }

    TEST_CASE("LE and BE produce different byte orderings for U16")
    {
        duin::io::IOStream leStream = duin::io::IOStream::FromDynamicMem();
        duin::io::IOStream beStream = duin::io::IOStream::FromDynamicMem();

        leStream.WriteU16LE(0x0102);
        beStream.WriteU16BE(0x0102);

        leStream.Seek(0, duin::io::IOWhence::Set);
        beStream.Seek(0, duin::io::IOWhence::Set);

        uint8_t leBytes[2] = {}, beBytes[2] = {};
        leStream.Read(leBytes, 2);
        beStream.Read(beBytes, 2);

        // LE: low byte first (0x02, 0x01)
        CHECK(leBytes[0] == 0x02);
        CHECK(leBytes[1] == 0x01);

        // BE: high byte first (0x01, 0x02)
        CHECK(beBytes[0] == 0x01);
        CHECK(beBytes[1] == 0x02);
    }

    TEST_CASE("16-bit reads on invalid stream return false")
    {
        duin::io::IOStream stream;
        uint16_t u16;
        int16_t s16;
        CHECK_FALSE(stream.ReadU16LE(&u16));
        CHECK_FALSE(stream.ReadU16BE(&u16));
        CHECK_FALSE(stream.ReadS16LE(&s16));
        CHECK_FALSE(stream.ReadS16BE(&s16));
    }

    TEST_CASE("16-bit writes on invalid stream return false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.WriteU16LE(0));
        CHECK_FALSE(stream.WriteU16BE(0));
        CHECK_FALSE(stream.WriteS16LE(0));
        CHECK_FALSE(stream.WriteS16BE(0));
    }
}

TEST_SUITE("IOStream - Typed Read/Write 32-bit")
{
    TEST_CASE("Write and read U32LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU32LE(0xDEADBEEF));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint32_t val = 0;
        CHECK(stream.ReadU32LE(&val));
        CHECK(val == 0xDEADBEEF);
    }

    TEST_CASE("Write and read U32BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU32BE(0xDEADBEEF));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint32_t val = 0;
        CHECK(stream.ReadU32BE(&val));
        CHECK(val == 0xDEADBEEF);
    }

    TEST_CASE("Write and read S32LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS32LE(-123456789));
        stream.Seek(0, duin::io::IOWhence::Set);

        int32_t val = 0;
        CHECK(stream.ReadS32LE(&val));
        CHECK(val == -123456789);
    }

    TEST_CASE("Write and read S32BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS32BE(-123456789));
        stream.Seek(0, duin::io::IOWhence::Set);

        int32_t val = 0;
        CHECK(stream.ReadS32BE(&val));
        CHECK(val == -123456789);
    }

    TEST_CASE("LE and BE produce different byte orderings for U32")
    {
        duin::io::IOStream leStream = duin::io::IOStream::FromDynamicMem();
        duin::io::IOStream beStream = duin::io::IOStream::FromDynamicMem();

        leStream.WriteU32LE(0x01020304);
        beStream.WriteU32BE(0x01020304);

        leStream.Seek(0, duin::io::IOWhence::Set);
        beStream.Seek(0, duin::io::IOWhence::Set);

        uint8_t leBytes[4] = {}, beBytes[4] = {};
        leStream.Read(leBytes, 4);
        beStream.Read(beBytes, 4);

        // LE: least significant byte first
        CHECK(leBytes[0] == 0x04);
        CHECK(leBytes[1] == 0x03);
        CHECK(leBytes[2] == 0x02);
        CHECK(leBytes[3] == 0x01);

        // BE: most significant byte first
        CHECK(beBytes[0] == 0x01);
        CHECK(beBytes[1] == 0x02);
        CHECK(beBytes[2] == 0x03);
        CHECK(beBytes[3] == 0x04);
    }

    TEST_CASE("32-bit reads on invalid stream return false")
    {
        duin::io::IOStream stream;
        uint32_t u32;
        int32_t s32;
        CHECK_FALSE(stream.ReadU32LE(&u32));
        CHECK_FALSE(stream.ReadU32BE(&u32));
        CHECK_FALSE(stream.ReadS32LE(&s32));
        CHECK_FALSE(stream.ReadS32BE(&s32));
    }

    TEST_CASE("32-bit writes on invalid stream return false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.WriteU32LE(0));
        CHECK_FALSE(stream.WriteU32BE(0));
        CHECK_FALSE(stream.WriteS32LE(0));
        CHECK_FALSE(stream.WriteS32BE(0));
    }
}

TEST_SUITE("IOStream - Typed Read/Write 64-bit")
{
    TEST_CASE("Write and read U64LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU64LE(0x0102030405060708ULL));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint64_t val = 0;
        CHECK(stream.ReadU64LE(&val));
        CHECK(val == 0x0102030405060708ULL);
    }

    TEST_CASE("Write and read U64BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteU64BE(0x0102030405060708ULL));
        stream.Seek(0, duin::io::IOWhence::Set);

        uint64_t val = 0;
        CHECK(stream.ReadU64BE(&val));
        CHECK(val == 0x0102030405060708ULL);
    }

    TEST_CASE("Write and read S64LE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS64LE(-1234567890123456789LL));
        stream.Seek(0, duin::io::IOWhence::Set);

        int64_t val = 0;
        CHECK(stream.ReadS64LE(&val));
        CHECK(val == -1234567890123456789LL);
    }

    TEST_CASE("Write and read S64BE")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.WriteS64BE(-1234567890123456789LL));
        stream.Seek(0, duin::io::IOWhence::Set);

        int64_t val = 0;
        CHECK(stream.ReadS64BE(&val));
        CHECK(val == -1234567890123456789LL);
    }

    TEST_CASE("LE and BE produce different byte orderings for U64")
    {
        duin::io::IOStream leStream = duin::io::IOStream::FromDynamicMem();
        duin::io::IOStream beStream = duin::io::IOStream::FromDynamicMem();

        leStream.WriteU64LE(0x0102030405060708ULL);
        beStream.WriteU64BE(0x0102030405060708ULL);

        leStream.Seek(0, duin::io::IOWhence::Set);
        beStream.Seek(0, duin::io::IOWhence::Set);

        uint8_t leBytes[8] = {}, beBytes[8] = {};
        leStream.Read(leBytes, 8);
        beStream.Read(beBytes, 8);

        // LE: least significant byte first
        CHECK(leBytes[0] == 0x08);
        CHECK(leBytes[7] == 0x01);

        // BE: most significant byte first
        CHECK(beBytes[0] == 0x01);
        CHECK(beBytes[7] == 0x08);
    }

    TEST_CASE("64-bit reads on invalid stream return false")
    {
        duin::io::IOStream stream;
        uint64_t u64;
        int64_t s64;
        CHECK_FALSE(stream.ReadU64LE(&u64));
        CHECK_FALSE(stream.ReadU64BE(&u64));
        CHECK_FALSE(stream.ReadS64LE(&s64));
        CHECK_FALSE(stream.ReadS64BE(&s64));
    }

    TEST_CASE("64-bit writes on invalid stream return false")
    {
        duin::io::IOStream stream;
        CHECK_FALSE(stream.WriteU64LE(0));
        CHECK_FALSE(stream.WriteU64BE(0));
        CHECK_FALSE(stream.WriteS64LE(0));
        CHECK_FALSE(stream.WriteS64BE(0));
    }
}

TEST_SUITE("IOStream - Mixed Typed Read/Write")
{
    TEST_CASE("Write multiple typed values then read them back in order")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();

        CHECK(stream.WriteU8(0xFF));
        CHECK(stream.WriteS16LE(-1000));
        CHECK(stream.WriteU32BE(42));
        CHECK(stream.WriteS64LE(9999999999LL));

        stream.Seek(0, duin::io::IOWhence::Set);

        uint8_t u8 = 0;
        int16_t s16 = 0;
        uint32_t u32 = 0;
        int64_t s64 = 0;

        CHECK(stream.ReadU8(&u8));
        CHECK(u8 == 0xFF);

        CHECK(stream.ReadS16LE(&s16));
        CHECK(s16 == -1000);

        CHECK(stream.ReadU32BE(&u32));
        CHECK(u32 == 42);

        CHECK(stream.ReadS64LE(&s64));
        CHECK(s64 == 9999999999LL);
    }

    TEST_CASE("Total bytes written matches expected size for mixed types")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();

        stream.WriteU8(1);       // 1 byte
        stream.WriteU16LE(2);    // 2 bytes
        stream.WriteU32LE(3);    // 4 bytes
        stream.WriteU64LE(4);    // 8 bytes

        CHECK(stream.Tell() == 15); // 1 + 2 + 4 + 8
    }
}

// ============================================================================
// Formatted Write
// ============================================================================

TEST_SUITE("IOStream - Printf")
{
    TEST_CASE("Printf writes formatted string")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();

        size_t written = stream.Printf("Hello %s, number %d", "world", 42);
        CHECK(written > 0);

        stream.Seek(0, duin::io::IOWhence::Set);
        char buf[64] = {};
        stream.Read(buf, written);
        CHECK(std::string(buf, written) == "Hello world, number 42");
    }

    TEST_CASE("Printf on invalid stream returns 0")
    {
        duin::io::IOStream stream;
        CHECK(stream.Printf("test %d", 1) == 0);
    }

    TEST_CASE("Printf with empty format string")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        size_t written = stream.Printf("");
        CHECK(written == 0);
    }

    TEST_CASE("Printf multiple calls accumulate")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();

        stream.Printf("abc");
        stream.Printf("def");
        stream.Printf("ghi");

        stream.Seek(0, duin::io::IOWhence::Set);
        char buf[16] = {};
        stream.Read(buf, 9);
        CHECK(std::string(buf, 9) == "abcdefghi");
    }
}

// ============================================================================
// Static Bulk Operations
// ============================================================================

TEST_SUITE("IOStream - Bulk LoadFile/SaveFile")
{
    TEST_CASE("SaveFile and LoadFile roundtrip")
    {
        std::string path = ARTIFACTS_DIR + "/bulk_roundtrip.bin";
        EnsureArtifactsDir();

        const char data[] = "bulk test data";
        CHECK(duin::io::IOStream::SaveFile(path, data, std::strlen(data)));

        size_t loadedSize = 0;
        void *loaded = duin::io::IOStream::LoadFile(path, &loadedSize);
        REQUIRE(loaded != nullptr);
        CHECK(loadedSize == std::strlen(data));
        CHECK(std::memcmp(loaded, data, loadedSize) == 0);

        duin::io::IOStream::FreeLoadedData(loaded);
        RemoveTestFile(path);
    }

    TEST_CASE("LoadFile on nonexistent file returns nullptr")
    {
        size_t size = 0;
        void *data = duin::io::IOStream::LoadFile(ARTIFACTS_DIR + "/nonexistent_bulk.bin", &size);
        CHECK(data == nullptr);
    }

    TEST_CASE("SaveFile to invalid path returns false")
    {
        const char data[] = "test";
        CHECK_FALSE(duin::io::IOStream::SaveFile("/nonexistent_dir_xyz/file.bin", data, 4));
    }
}

TEST_SUITE("IOStream - Bulk LoadFileIO/SaveFileIO")
{
    TEST_CASE("SaveFileIO with closeWhenDone invalidates stream")
    {
        std::string path = ARTIFACTS_DIR + "/bulk_io_save.bin";
        EnsureArtifactsDir();

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "wb");
        REQUIRE(stream.IsValid());

        const char data[] = "save io data";
        CHECK(duin::io::IOStream::SaveFileIO(stream, data, std::strlen(data), true));
        CHECK_FALSE(stream.IsValid());

        RemoveTestFile(path);
    }

    TEST_CASE("SaveFileIO without closeWhenDone keeps stream valid")
    {
        std::string path = ARTIFACTS_DIR + "/bulk_io_save_keep.bin";
        EnsureArtifactsDir();

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "wb");
        REQUIRE(stream.IsValid());

        const char data[] = "keep open";
        CHECK(duin::io::IOStream::SaveFileIO(stream, data, std::strlen(data), false));
        CHECK(stream.IsValid());

        stream.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("LoadFileIO with closeWhenDone invalidates stream")
    {
        std::string path = ARTIFACTS_DIR + "/bulk_io_load.bin";
        CreateTestFile(path, "load io data");

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "rb");
        REQUIRE(stream.IsValid());

        size_t size = 0;
        void *data = duin::io::IOStream::LoadFileIO(stream, &size, true);
        REQUIRE(data != nullptr);
        CHECK(size == std::strlen("load io data"));
        CHECK_FALSE(stream.IsValid());

        duin::io::IOStream::FreeLoadedData(data);
        RemoveTestFile(path);
    }

    TEST_CASE("LoadFileIO without closeWhenDone keeps stream valid")
    {
        std::string path = ARTIFACTS_DIR + "/bulk_io_load_keep.bin";
        CreateTestFile(path, "keep stream");

        duin::io::IOStream stream = duin::io::IOStream::FromFile(path, "rb");
        REQUIRE(stream.IsValid());

        size_t size = 0;
        void *data = duin::io::IOStream::LoadFileIO(stream, &size, false);
        REQUIRE(data != nullptr);
        CHECK(stream.IsValid());

        duin::io::IOStream::FreeLoadedData(data);
        stream.Close();
        RemoveTestFile(path);
    }

    TEST_CASE("SaveFileIO on invalid stream returns false")
    {
        duin::io::IOStream stream;
        const char data[] = "test";
        CHECK_FALSE(duin::io::IOStream::SaveFileIO(stream, data, 4, false));
    }

    TEST_CASE("LoadFileIO on invalid stream returns nullptr")
    {
        duin::io::IOStream stream;
        size_t size = 0;
        CHECK(duin::io::IOStream::LoadFileIO(stream, &size, false) == nullptr);
    }
}

// ============================================================================
// Edge Cases & Integration
// ============================================================================

TEST_SUITE("IOStream - Edge Cases")
{
    TEST_CASE("Zero-length read returns 0")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        char buf[1];
        CHECK(stream.Read(buf, 0) == 0);
    }

    TEST_CASE("Zero-length write returns 0")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.Write("x", 0) == 0);
    }

    TEST_CASE("FreeLoadedData with nullptr does not crash")
    {
        duin::io::IOStream::FreeLoadedData(nullptr);
        // No crash = pass
    }

    TEST_CASE("Multiple seek operations in sequence")
    {
        char buffer[32] = "abcdefghijklmnopqrstuvwxyz";
        duin::io::IOStream stream = duin::io::IOStream::FromMem(buffer, 26);

        stream.Seek(10, duin::io::IOWhence::Set);
        CHECK(stream.Tell() == 10);

        stream.Seek(5, duin::io::IOWhence::Cur);
        CHECK(stream.Tell() == 15);

        stream.Seek(-1, duin::io::IOWhence::End);
        CHECK(stream.Tell() == 25);

        stream.Seek(0, duin::io::IOWhence::Set);
        CHECK(stream.Tell() == 0);
    }

    TEST_CASE("Large binary data roundtrip via file")
    {
        std::string path = ARTIFACTS_DIR + "/large_binary.bin";
        EnsureArtifactsDir();

        // Create a 4KB buffer with a pattern
        std::vector<uint8_t> original(4096);
        for (size_t i = 0; i < original.size(); i++)
        {
            original[i] = static_cast<uint8_t>(i & 0xFF);
        }

        // Write
        {
            duin::io::IOStream ws = duin::io::IOStream::FromFile(path, "wb");
            REQUIRE(ws.IsValid());
            size_t written = ws.Write(original.data(), original.size());
            CHECK(written == original.size());
        }

        // Read back
        {
            duin::io::IOStream rs = duin::io::IOStream::FromFile(path, "rb");
            REQUIRE(rs.IsValid());
            CHECK(rs.GetSize() == 4096);

            std::vector<uint8_t> readBack(4096);
            size_t bytesRead = rs.Read(readBack.data(), readBack.size());
            CHECK(bytesRead == 4096);
            CHECK(original == readBack);
        }

        RemoveTestFile(path);
    }

    TEST_CASE("Write typed values to file and read back")
    {
        std::string path = ARTIFACTS_DIR + "/typed_file.bin";
        EnsureArtifactsDir();

        // Write various typed values
        {
            duin::io::IOStream ws = duin::io::IOStream::FromFile(path, "wb");
            REQUIRE(ws.IsValid());
            ws.WriteU8(0x42);
            ws.WriteU16LE(0xBEEF);
            ws.WriteU32BE(0xCAFEBABE);
            ws.WriteS64LE(-9876543210LL);
        }

        // Read them back
        {
            duin::io::IOStream rs = duin::io::IOStream::FromFile(path, "rb");
            REQUIRE(rs.IsValid());

            uint8_t u8 = 0;
            uint16_t u16 = 0;
            uint32_t u32 = 0;
            int64_t s64 = 0;

            CHECK(rs.ReadU8(&u8));
            CHECK(u8 == 0x42);

            CHECK(rs.ReadU16LE(&u16));
            CHECK(u16 == 0xBEEF);

            CHECK(rs.ReadU32BE(&u32));
            CHECK(u32 == 0xCAFEBABE);

            CHECK(rs.ReadS64LE(&s64));
            CHECK(s64 == -9876543210LL);
        }

        RemoveTestFile(path);
    }

    TEST_CASE("Const memory stream size is accurate")
    {
        const char data[] = "exact size";
        duin::io::IOStream stream = duin::io::IOStream::FromConstMem(data, 10);
        CHECK(stream.GetSize() == 10);
        CHECK(stream.Tell() == 0);
    }

    TEST_CASE("Dynamic memory stream Tell advances with writes")
    {
        duin::io::IOStream stream = duin::io::IOStream::FromDynamicMem();
        CHECK(stream.Tell() == 0);

        stream.WriteU32LE(1);
        CHECK(stream.Tell() == 4);

        stream.WriteU32LE(2);
        CHECK(stream.Tell() == 8);

        stream.WriteU8(3);
        CHECK(stream.Tell() == 9);
    }
}

} // namespace TestIOStream
