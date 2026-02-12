/**
 * @file UUID.h
 * @brief Defines the UUID class for generating and managing unique identifiers.
 * @author DuinEngine Contributors
 * @date 2024
 *
 * This file provides the UUID class, which represents a 64-bit universally unique identifier.
 * It supports generation, conversion to/from decimal and hexadecimal strings, and hashing
 * for use in standard containers.
 *
 * @example
 * @code
 * #include <Duin/Core/Utils/UUID.h>
 *
 * // Generate a new random UUID
 * duin::UUID id1;
 * duin::UUID id2;
 *
 * // Create UUID from uint64_t value
 * duin::UUID id3(42);
 *
 * // Convert to strings
 * std::string decStr = duin::UUID::ToStringDec(id1);
 * std::string hexStr = duin::UUID::ToStringHex(id1);
 *
 * // Parse from strings
 * duin::UUID fromDec = duin::UUID::FromStringDec("12345");
 * duin::UUID fromHex = duin::UUID::FromStringHex("0xABCD");
 *
 * // Use in containers
 * std::unordered_map<duin::UUID, std::string> map;
 * map[id1] = "Object1";
 * @endcode
 */

#pragma once

#include <functional>
#include <cstdint>
#include <string>

namespace duin
{

/**
 * @class UUID
 * @brief A 64-bit universally unique identifier.
 *
 * UUID provides a lightweight unique identifier system based on 64-bit integers.
 * It supports random generation, conversion to/from string formats (decimal and hexadecimal),
 * and hashing for use in standard containers like std::unordered_map.
 *
 * Thread-safety note: UUID generation uses a shared random engine. While the generation
 * itself is not thread-safe due to the shared engine state, each generated UUID is unique
 * with extremely high probability.
 */
class UUID
{
  public:
    /**
     * @brief Invalid/null UUID constant with value 0.
     *
     * Use this to represent an uninitialized or invalid UUID state.
     *
     * @code
     * duin::UUID id;
     * if (id == duin::UUID::INVALID) {
     *     // Handle invalid case
     * }
     * @endcode
     */
    static const UUID INVALID;

    /**
     * @brief Default constructor. Generates a new random UUID.
     *
     * Creates a UUID with a randomly generated 64-bit value using
     * std::mt19937_64 random engine seeded from std::random_device.
     *
     * @note Each call generates a unique ID with extremely high probability.
     */
    UUID();

    /**
     * @brief Constructs a UUID from a specific uint64_t value.
     *
     * @param uuid The 64-bit value to use as the UUID.
     *
     * @code
     * duin::UUID id(42);
     * duin::UUID invalid(0); // Same as UUID::INVALID
     * @endcode
     */
    UUID(uint64_t uuid);

    /**
     * @brief Destructor.
     */
    ~UUID();

    /**
     * @brief Converts a UUID to a decimal string representation.
     *
     * @param uuid The UUID to convert.
     * @return Decimal string representation of the UUID.
     *
     * @code
     * duin::UUID id(12345);
     * std::string str = duin::UUID::ToStringDec(id); // "12345"
     * @endcode
     */
    static std::string ToStringDec(UUID uuid);

    /**
     * @brief Converts a UUID to a hexadecimal string representation.
     *
     * The output format is "0x" followed by uppercase hexadecimal digits.
     *
     * @param uuid The UUID to convert.
     * @return Hexadecimal string representation with "0x" prefix.
     *
     * @code
     * duin::UUID id(255);
     * std::string str = duin::UUID::ToStringHex(id); // "0xFF"
     * @endcode
     */
    static std::string ToStringHex(UUID uuid);

    /**
     * @brief Parses a UUID from a decimal string.
     *
     * @param string Decimal string representation of a UUID.
     * @return Parsed UUID.
     * @throws Logs fatal error if string is invalid or out of range.
     *
     * @code
     * duin::UUID id = duin::UUID::FromStringDec("12345");
     * @endcode
     */
    static UUID FromStringDec(const std::string &string);

    /**
     * @brief Parses a UUID from a hexadecimal string.
     *
     * Accepts strings with or without "0x" or "0X" prefix.
     * Hex digits can be uppercase or lowercase.
     *
     * @param string Hexadecimal string representation of a UUID.
     * @return Parsed UUID.
     * @throws Logs fatal error if string is invalid.
     *
     * @code
     * duin::UUID id1 = duin::UUID::FromStringHex("0xFF");
     * duin::UUID id2 = duin::UUID::FromStringHex("FF");    // Also valid
     * duin::UUID id3 = duin::UUID::FromStringHex("0XaBcD"); // Also valid
     * @endcode
     */
    static UUID FromStringHex(const std::string &string);

    std::string ToStrDec();
    std::string ToStrHex();

    /**
     * @brief Equality comparison operator.
     *
     * @param uuid UUID to compare with.
     * @return True if UUIDs are equal, false otherwise.
     */
    bool operator==(const UUID &uuid) const
    {
        return this->uuid_ == uuid.uuid_;
    }

    /**
     * @brief Inequality comparison operator.
     *
     * @param uuid UUID to compare with.
     * @return True if UUIDs are not equal, false otherwise.
     */
    bool operator!=(const UUID &uuid) const
    {
        return this->uuid_ != uuid.uuid_;
    }

    /**
     * @brief Conversion operator to uint64_t.
     *
     * Allows implicit conversion to uint64_t for compatibility.
     *
     * @return The underlying uint64_t value.
     */
    operator uint64_t() const
    {
        return uuid_;
    }

  private:
    uint64_t uuid_; ///< The underlying 64-bit unique identifier value.
};

} // namespace duin

/**
 * @brief Hash specialization for duin::UUID.
 *
 * Enables use of UUID in hash-based containers like std::unordered_map and std::unordered_set.
 */
namespace std
{
template <>
struct hash<duin::UUID>
{
    /**
     * @brief Computes hash value for a UUID.
     *
     * @param uuid The UUID to hash.
     * @return Hash value as size_t.
     */
    std::size_t operator()(const duin::UUID &uuid) const
    {
        return std::hash<uint64_t>()(static_cast<uint64_t>(uuid));
    }
};
} // namespace std
