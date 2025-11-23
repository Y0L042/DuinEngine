#include <doctest.h>
#include <Duin/Core/Utils/UUID.h>
#include <string>
#include <unordered_set>
#include <sstream>
#include <cstdint>

namespace TestUUID
{
    TEST_SUITE("UUID") 
    {
        TEST_CASE("Default Constructor - Generates Valid UUID") {
            duin::UUID uuid1;
            duin::UUID uuid2;

            // UUIDs should be non-zero (valid)
            CHECK(uuid1 != duin::UUID::INVALID);
            CHECK(uuid2 != duin::UUID::INVALID);

            // Different UUIDs should be generated
            CHECK(uuid1 != uuid2);
        }

        TEST_CASE("Constructor with uint64_t") {
            SUBCASE("Zero value") {
                duin::UUID uuid(0);
                CHECK(uuid == duin::UUID::INVALID);
                CHECK(static_cast<uint64_t>(uuid) == 0);
            }

            SUBCASE("Specific value") {
                uint64_t value = 12345678901234567890ULL;
                duin::UUID uuid(value);
                CHECK(static_cast<uint64_t>(uuid) == value);
            }

            SUBCASE("Max value") {
                uint64_t value = UINT64_MAX;
                duin::UUID uuid(value);
                CHECK(static_cast<uint64_t>(uuid) == value);
            }
        }

        TEST_CASE("INVALID constant") {
            duin::UUID invalid = duin::UUID::INVALID;
            CHECK(static_cast<uint64_t>(invalid) == 0);
            
            duin::UUID zero(0);
            CHECK(invalid == zero);
        }

        TEST_CASE("Equality Operators") {
            duin::UUID uuid1(42);
            duin::UUID uuid2(42);
            duin::UUID uuid3(84);

            SUBCASE("operator==") {
                CHECK(uuid1 == uuid2);
                CHECK_FALSE(uuid1 == uuid3);
            }

            SUBCASE("operator!=") {
                CHECK(uuid1 != uuid3);
                CHECK_FALSE(uuid1 != uuid2);
            }
        }

        TEST_CASE("uint64_t conversion operator") {
            uint64_t value = 9876543210ULL;
            duin::UUID uuid(value);

            uint64_t converted = static_cast<uint64_t>(uuid);
            CHECK(converted == value);

            // Implicit conversion
            uint64_t implicit = uuid;
            CHECK(implicit == value);
        }

        TEST_CASE("ToStringDec - Decimal String Conversion") {
            SUBCASE("Zero value") {
                duin::UUID uuid(0);
                std::string str = duin::UUID::ToStringDec(uuid);
                CHECK(str == "0");
            }

            SUBCASE("Small value") {
                duin::UUID uuid(123);
                std::string str = duin::UUID::ToStringDec(uuid);
                CHECK(str == "123");
            }

            SUBCASE("Large value") {
                duin::UUID uuid(12345678901234567890ULL);
                std::string str = duin::UUID::ToStringDec(uuid);
                CHECK(str == "12345678901234567890");
            }

            SUBCASE("Max uint64_t value") {
                duin::UUID uuid(UINT64_MAX);
                std::string str = duin::UUID::ToStringDec(uuid);
                CHECK(str == "18446744073709551615");
            }
        }

        TEST_CASE("FromStringDec - Parse Decimal String") {
            SUBCASE("Zero value") {
                duin::UUID uuid = duin::UUID::FromStringDec("0");
                CHECK(static_cast<uint64_t>(uuid) == 0);
            }

            SUBCASE("Small value") {
                duin::UUID uuid = duin::UUID::FromStringDec("123");
                CHECK(static_cast<uint64_t>(uuid) == 123);
            }

            SUBCASE("Large value") {
                duin::UUID uuid = duin::UUID::FromStringDec("12345678901234567890");
                CHECK(static_cast<uint64_t>(uuid) == 12345678901234567890ULL);
            }

            SUBCASE("Max uint64_t value") {
                duin::UUID uuid = duin::UUID::FromStringDec("18446744073709551615");
                CHECK(static_cast<uint64_t>(uuid) == UINT64_MAX);
            }
        }

        TEST_CASE("ToStringHex - Hexadecimal String Conversion") {
            SUBCASE("Zero value") {
                duin::UUID uuid(0);
                std::string str = duin::UUID::ToStringHex(uuid);
                CHECK(str == "0x0");
            }

            SUBCASE("Small value") {
                duin::UUID uuid(255);
                std::string str = duin::UUID::ToStringHex(uuid);
                CHECK(str == "0xFF");
            }

            SUBCASE("Large value") {
                duin::UUID uuid(0xABCDEF1234567890ULL);
                std::string str = duin::UUID::ToStringHex(uuid);
                CHECK(str == "0xABCDEF1234567890");
            }

            SUBCASE("Contains uppercase hex digits") {
                duin::UUID uuid(0xabcdef);
                std::string str = duin::UUID::ToStringHex(uuid);
                // Should be uppercase
                bool out = str.find('A') != std::string::npos || str.find('B') != std::string::npos;
                CHECK(out);
            }
        }

        TEST_CASE("FromStringHex - Parse Hexadecimal String") {
            SUBCASE("Zero value") {
                duin::UUID uuid = duin::UUID::FromStringHex("0x0");
                CHECK(static_cast<uint64_t>(uuid) == 0);
            }

            SUBCASE("With 0x prefix") {
                duin::UUID uuid = duin::UUID::FromStringHex("0xFF");
                CHECK(static_cast<uint64_t>(uuid) == 255);
            }

            SUBCASE("With 0X prefix (uppercase)") {
                duin::UUID uuid = duin::UUID::FromStringHex("0XFF");
                CHECK(static_cast<uint64_t>(uuid) == 255);
            }

            SUBCASE("Without prefix") {
                duin::UUID uuid = duin::UUID::FromStringHex("FF");
                CHECK(static_cast<uint64_t>(uuid) == 255);
            }

            SUBCASE("Large hex value") {
                duin::UUID uuid = duin::UUID::FromStringHex("0xABCDEF1234567890");
                CHECK(static_cast<uint64_t>(uuid) == 0xABCDEF1234567890ULL);
            }

            SUBCASE("Lowercase hex digits") {
                duin::UUID uuid = duin::UUID::FromStringHex("0xabcdef");
                CHECK(static_cast<uint64_t>(uuid) == 0xABCDEF);
            }

            SUBCASE("Mixed case hex digits") {
                duin::UUID uuid = duin::UUID::FromStringHex("0xAbCdEf");
                CHECK(static_cast<uint64_t>(uuid) == 0xABCDEF);
            }

            SUBCASE("With dash delimiters") {
                duin::UUID uuid = duin::UUID::FromStringHex("ABCD-EF12-3456-7890");
                CHECK(static_cast<uint64_t>(uuid) == 0xABCDEF1234567890ULL);
            }

            SUBCASE("With 0x prefix and dash delimiters") {
                duin::UUID uuid = duin::UUID::FromStringHex("0xABCD-EF12-3456-7890");
                CHECK(static_cast<uint64_t>(uuid) == 0xABCDEF1234567890ULL);
            }
        }

        TEST_CASE("Round-trip Conversion - Decimal") {
            SUBCASE("Small value") {
                duin::UUID original(12345);
                std::string str = duin::UUID::ToStringDec(original);
                duin::UUID parsed = duin::UUID::FromStringDec(str);
                CHECK(original == parsed);
            }

            SUBCASE("Large value") {
                duin::UUID original(9876543210123456789ULL);
                std::string str = duin::UUID::ToStringDec(original);
                duin::UUID parsed = duin::UUID::FromStringDec(str);
                CHECK(original == parsed);
            }

            SUBCASE("Random generated UUID") {
                duin::UUID original;
                std::string str = duin::UUID::ToStringDec(original);
                duin::UUID parsed = duin::UUID::FromStringDec(str);
                CHECK(original == parsed);
            }
        }

        TEST_CASE("Round-trip Conversion - Hexadecimal") {
            SUBCASE("Small value") {
                duin::UUID original(0xABCD);
                std::string str = duin::UUID::ToStringHex(original);
                duin::UUID parsed = duin::UUID::FromStringHex(str);
                CHECK(original == parsed);
            }

            SUBCASE("Large value") {
                duin::UUID original(0x123456789ABCDEF0ULL);
                std::string str = duin::UUID::ToStringHex(original);
                duin::UUID parsed = duin::UUID::FromStringHex(str);
                CHECK(original == parsed);
            }

            SUBCASE("Random generated UUID") {
                duin::UUID original;
                std::string str = duin::UUID::ToStringHex(original);
                duin::UUID parsed = duin::UUID::FromStringHex(str);
                CHECK(original == parsed);
            }
        }

        TEST_CASE("Hash Function") {
            duin::UUID uuid1(42);
            duin::UUID uuid2(42);
            duin::UUID uuid3(84);

            std::hash<duin::UUID> hasher;

            SUBCASE("Same UUIDs produce same hash") {
                CHECK(hasher(uuid1) == hasher(uuid2));
            }

            SUBCASE("Different UUIDs produce different hash (usually)") {
                // Note: Hash collisions are possible but unlikely for these values
                CHECK(hasher(uuid1) != hasher(uuid3));
            }

            SUBCASE("Works with unordered containers") {
                std::unordered_set<duin::UUID> uuidSet;
                uuidSet.insert(uuid1);
                uuidSet.insert(uuid2);
                uuidSet.insert(uuid3);

                CHECK(uuidSet.size() == 2); // uuid1 and uuid2 are the same
                CHECK(uuidSet.find(uuid1) != uuidSet.end());
                CHECK(uuidSet.find(uuid3) != uuidSet.end());
            }
        }

        TEST_CASE("Uniqueness Test") {
            const int count = 1000;
            std::unordered_set<duin::UUID> uuids;

            for (int i = 0; i < count; ++i) {
                duin::UUID uuid;
                uuids.insert(uuid);
            }

            // All generated UUIDs should be unique
            CHECK(uuids.size() == count);
        }

        TEST_CASE("Stream Operator") {
            SUBCASE("Output to stream") {
                duin::UUID uuid(12345);
                std::ostringstream oss;
                oss << uuid;
                CHECK(oss.str() == "12345");
            }

            SUBCASE("Large value stream output") {
                duin::UUID uuid(9876543210ULL);
                std::ostringstream oss;
                oss << uuid;
                CHECK(oss.str() == "9876543210");
            }
        }

        TEST_CASE("Cross-format Conversion") {
            SUBCASE("Dec to Hex and back") {
                duin::UUID original(255);
                std::string dec = duin::UUID::ToStringDec(original);
                std::string hex = duin::UUID::ToStringHex(original);
                
                CHECK(dec == "255");
                CHECK(hex == "0xFF");
                
                duin::UUID fromDec = duin::UUID::FromStringDec(dec);
                duin::UUID fromHex = duin::UUID::FromStringHex(hex);
                
                CHECK(fromDec == original);
                CHECK(fromHex == original);
                CHECK(fromDec == fromHex);
            }
        }

        TEST_CASE("Edge Cases") {
            SUBCASE("INVALID UUID comparisons") {
                duin::UUID invalid1 = duin::UUID::INVALID;
                duin::UUID invalid2(0);
                duin::UUID valid(1);

                CHECK(invalid1 == invalid2);
                CHECK(invalid1 != valid);
            }

            SUBCASE("Minimum and maximum values") {
                duin::UUID minUUID(0);
                duin::UUID maxUUID(UINT64_MAX);

                CHECK(minUUID == duin::UUID::INVALID);
                CHECK(maxUUID != duin::UUID::INVALID);
                CHECK(minUUID != maxUUID);
            }
        }
    }
}
