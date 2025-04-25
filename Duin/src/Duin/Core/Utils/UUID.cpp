#include "dnpch.h"
#include "UUID.H"

#include <random>
#include <cstdint>
#include <string>
#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace duin {
	static std::random_device randomDevice;
	static std::mt19937_64 engine(randomDevice());
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
        uint64_t value = 0;
        std::string str = string;
        if (str.size() >= 2 && str[0] == '0' && 
            (str[1] == 'x' || str[1] == 'X')) {
            str = str.substr(2);
        }

        try {
            value = std::stoull(str, nullptr, 16);
        } catch (...) {
            DN_CORE_FATAL("Error when converting UUID {}!", string);
        }

        return UUID(value);
    }
}

