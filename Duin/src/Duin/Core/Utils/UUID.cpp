/**
 * @file UUID.cpp
 * @brief Implementation of the UUID class for generating and managing unique identifiers.
 * @author DuinEngine Contributors
 * @date 2024
 *
 * This file contains the implementation of the UUID class, which provides
 * 64-bit universally unique identifiers with support for random generation,
 * string conversion (decimal and hexadecimal), and hashing for use in containers.
 */

#include "dnpch.h"
#include "UUID.H"

#include <random>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace duin {
	/// Random device for seeding the random number generator
	static std::random_device randomDevice;
	
	/// Mersenne Twister 64-bit random number generator
	static std::mt19937_64 engine(randomDevice());
	
	/// Uniform distribution for generating random uint64_t values
	static std::uniform_int_distribution<uint64_t> uniformDistribution;

    const UUID UUID::INVALID = 0;

	UUID::UUID()
		: uuid_(uniformDistribution(engine))
	{
	}

	UUID::UUID(uint64_t uuid)
		: uuid_(uuid)
	{
	}

	UUID::~UUID()
	{
	}

    std::string UUID::ToStringDec(UUID uuid)
    {
        std::stringstream ss_dec;
        ss_dec << uuid;
        std::string decimal_str = ss_dec.str();

        return decimal_str;
    }

    UUID UUID::FromStringDec(const std::string& string)
    {
        uint64_t value = 0;
        try {
            value = std::stoull(string, nullptr, 10);
        } catch (const std::invalid_argument& e) {
            DN_CORE_FATAL("Invalid UUID {}!", string);
        } catch (const std::out_of_range& e) {
            DN_CORE_FATAL("UUID too large {}!", string);
        }

        return UUID(value);
    }

    std::string UUID::ToStringHex(UUID uuid)
    {
        std::stringstream ss_hex_upper;
        ss_hex_upper << "0x" << std::uppercase << std::hex << uuid;
        std::string hex_str_upper = ss_hex_upper.str();

        return hex_str_upper;
	}

    UUID UUID::FromStringHex(const std::string& string)
    {
        uint64_t value = INVALID;
        std::string str = string;
        
        // Check for empty string
        if (str.empty()) 
        {
            DN_CORE_ERROR("UUID::FromStringHex - Empty string provided!");
            return UUID(INVALID);
        }
        
        // Remove "0x" or "0X" prefix if present
        if (str.size() >= 2 && str[0] == '0' && 
            (str[1] == 'x' || str[1] == 'X')) 
        {
            str = str.substr(2);
        }

        // Remove all "-" delimiters
        str.erase(std::remove(str.begin(), str.end(), '-'), str.end());

        // Check if string is empty after removing prefix and delimiters
        if (str.empty()) 
        {
            DN_CORE_ERROR("UUID::FromStringHex - No valid hexadecimal digits in string '{}'!", string);
            return UUID(INVALID);
        }

        // Check if string is too long for uint64_t (max 16 hex digits)
        if (str.length() > 16) 
        {
            DN_CORE_ERROR("UUID::FromStringHex - Hex string '{}' is too long ({} chars, max 16 for 64-bit UUID)!", string, str.length());
            return UUID(INVALID);
        }

        // Validate that string contains only valid hexadecimal characters
        for (char c : str) 
        {
            if (!((c >= '0' && c <= '9') || 
                  (c >= 'a' && c <= 'f') || 
                  (c >= 'A' && c <= 'F'))) 
            {
                DN_CORE_ERROR("UUID::FromStringHex - Invalid hexadecimal character '{}' in string '{}'!", c, string);
                return UUID(INVALID);
            }
        }

        try 
        {
            value = std::stoull(str, nullptr, 16);
        } catch (const std::invalid_argument& e) 
        {
            DN_CORE_FATAL("UUID::FromStringHex - Invalid argument when parsing '{}': {}", string, e.what());
            return UUID(INVALID);
        } catch (const std::out_of_range& e) 
        {
            DN_CORE_FATAL("UUID::FromStringHex - Value out of range when parsing '{}': {}", string, e.what());
            return UUID(INVALID);
        } catch (...) 
        {
            DN_CORE_FATAL("UUID::FromStringHex - Unknown error when parsing '{}'!", string);
            return UUID(INVALID);
        }

        return UUID(value);
    }
}

